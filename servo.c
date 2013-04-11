/* Servo motor functions */


/* Initialize servo timer channel */
void servo_init(void) {
    
    // Set initial waveform on and off times
    // Global interrupts masked off just in case, but servo_init() should be called before global interrupts are enabled for the first time
    DisableInterrupts;
        servo_on_time = SERVO_INIT_ON_TIME;
        servo_off_time = (SERVO_PERIOD * OC_DELTA_1US) - servo_on_time;
    EnableInterrupts;
    
    // Enable timer module if not already enabled
    if(!(TSCR1 & TSCR1_TEN_MASK)) EnableTimer;
    
    TC_OC(TC_SERVO);    // Channel set to output compare
    SET_OC_ACTION(TC_SERVO,OC_TOGGLE);  // Toggle output line after output compare triggered
    
    TC(TC_SERVO) = TCNT + SERVO_INIT_ON_TIME;   // Preset OC channel
    TC_INT_ENABLE(TC_SERVO);    // Enable timer channel interrupts
}

/* Set servo arm to angle */
void servo_angle(byte angle) {
    word on_time, off_time;
    
    // Bound to limits
    if(angle > SERVO_LIMIT_ANGLE) angle = SERVO_LIMIT_ANGLE;
    //if(angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;  // <-- not need if using unsigned
    
    // Calculate pulse times
    //on_time = ((angle - SERVO_MIN_ANGLE) * SERVO_GAIN) + SERVO_MIN_ON_TIME;   // <-- not needed if using unsigned (0 to 180 degrees)
    on_time = ((SERVO_MAX_ANGLE - angle) * SERVO_GAIN) + SERVO_MIN_ON_TIME;
    off_time = (SERVO_PERIOD * OC_DELTA_1US) - on_time;
    
    servo_set_times(on_time, off_time); // Update the new pulse times
}

/* Set servo on and off times */
static void servo_set_times(word on_time, word off_time) {
    // Critical region; servo_on_time and servo_off_time are used by the servo ISR
    DisableInterrupts;
        servo_on_time = on_time;
        servo_off_time = off_time;
    EnableInterrupts;
}

/*****************************************************************************/

/* Servo output compare interrupt handler */
interrupt VECTOR_NUM(TC_VECTOR(TC_SERVO)) void Servo_ISR(void) {
    static byte tog = 0;
    
    // Acknowledge interrupt and set OC for next event
    // First event time should be the servo on time (tog initially false)
    TC(TC_SERVO) += (tog) ? servo_off_time : servo_on_time;
    
    tog = (tog) ? 0 : 1;
}
