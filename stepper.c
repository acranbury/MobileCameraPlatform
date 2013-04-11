/* Stepper motor functions */


/* Initialize stepper motor control */
void stepper_init(void) {
    
    // Set initial delay time and step type
    stepper_delay = STEPPER_DELAY_INIT;
    stepper_step_type = STEPPER_STEP_INIT;
    
    // Enable timer module if not already enabled
    if(!(TSCR1 & TSCR1_TEN_MASK)) EnableTimer;
    
    TC_OC(TC_STEPPER);  // Channel set to output compare
    SET_OC_ACTION(TC_STEPPER,OC_OFF);   // Do not change output line after output compare triggered, this is controlled manually
    
    TC(TC_STEPPER) = TCNT + TIMER_DELTA(stepper_delay); // Preset OC channel
    TC_INT_ENABLE(TC_STEPPER);  // Enable timer channel interrupts
    
    SET_BITS(STEPPER_PORT_DDR,STEPPER_COIL_BITS);   // Set stepper coil lines as outputs
    SET_BITS(STEPPER_LIMIT_INPUT_EN,STEPPER_LIMIT_INPUT_EN_MASK);   // Set limit switch ports as inputs
}

/* Calibrate stepper motor limits */
void stepper_calibrate(void) {
    static byte stepper_calibrated_left = 0;
    
    // Check if stepper limits have been found
    while(!(stepper_calibrated)) {
        if( !STEPPER_LIMIT_LEFT && !stepper_calibrated_left ) {
            // Left limit switch hit
            stepper_position = 0;   // set lower limit (fully left)
            stepper_calibrated_left = 1;
            stepper_reverse();
        } else if( !STEPPER_LIMIT_RIGHT ) {
            // Right limit switch hit
            stepper_limit = stepper_position;   // set upper limit (fully right)
            stepper_calibrated = 1;
            stepper_reverse();
        }
    }
    
    stepper_set_pos(stepper_limit/2);   // Set stepper to center point
}

/* Reverese stepper direction */
void stepper_reverse(void) {
    if(stepper_step_type == HALF_STEP_LEFT || stepper_step_type == HALF_STEP_RIGHT)
        // Step type is half-step
        stepper_step_type = (char)((stepper_step_type == HALF_STEP_LEFT) ? HALF_STEP_RIGHT : HALF_STEP_LEFT);
    else
        // Step type is full-step
        stepper_step_type = (char)((stepper_step_type == FULL_STEP_LEFT) ? FULL_STEP_RIGHT : FULL_STEP_LEFT);
}

/* Move stepper to position */
void stepper_set_pos(word setpoint) {
    if(setpoint <= stepper_limit && stepper_limit != 0) { // Range is from 0 to stepper_limit (after calibration)
        
        // Check which direction to pan
        // Reverse direction if:
        // - setpoint is to the right of current position and current step type is to the left (negative)
        // - setpoint is to the left of current position and current step type is to the right (positive)
        if((setpoint > stepper_position && stepper_step_type < 0) || (setpoint < stepper_position && stepper_step_type > 0))
            stepper_reverse();
        
        stepper_setpoint = setpoint;
    }
}

/* Set delay in ms between steps */
void stepper_set_delay(byte delay) {
    if(delay <= 524)    // Timer channel is 16bits, delay is multiplied by 125 (1ms count with prescaler of 64)
        stepper_delay = delay;
}

/*****************************************************************************/

/* Stepper motor control interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_STEPPER)) void StepperISR(void) {
    static byte stepper_count = 0;
    
    TC(TC_STEPPER) += TIMER_DELTA(stepper_delay);   // Acknowledge interrupt and set delta for next event
    
    FORCE_BITS(STEPPER_PORT,STEPPER_COIL_BITS,(STEPPER_PATTERN[stepper_count] << 4));   // Change bit pattern on stepper coils
    
    /* ADD CHECKING OF LIMITS IN HERE SO THAT STEPPER DOES NOT TRY TO GO PAST PHYSICAL LIMITS */
    if(stepper_position != stepper_setpoint || stepper_calibrated == 0) {
        /* Stepper not at set point or not calibrated */
        stepper_count = (stepper_count + stepper_step_type) & STEPPER_COUNT_MASK; // Set offset to next step pattern and mask off bits to keep value in range
        stepper_position += stepper_step_type;
    } else {
        /* Stepper is at set point */
        if(stepper_count % 2) CLR_BITS(STEPPER_PORT,STEPPER_COIL_BITS); // Turn off coils on odd numbers (50% duty cycle)
    }
}
