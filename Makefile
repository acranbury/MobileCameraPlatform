#Makefile for MobileCameraPlatform

# Robot travels right angle triangle, waiting at each corner for a DTMF tone.
# A total time limit was required for the demo, and each triangle length was specified.
straight-lines-demo:
	gcc linux/traight-lines-demo.c linux/serial.c linux/dtmf.c -o straight-lines-demo

# Robot travels a 1m radius circle counter clockwise, rotates 180 degrees, and goes back.
# Different time limits were set for each direction, to demo the PID speed control law implementation.
circles-demo:
	gcc linux/circles-demo.c linux/serial.c linux/dtmf.c -o circles-demo

# A program to allow manual control of the robot from the joystick/gamepad.
joystick:
	gcc linux/joystick.c linux/serial.c linux/dtmf.c linux/image.c -pthread -lrt -o joystick

# Removes all object and executable files.
clean:
	rm -rf *.o circles-demo straight-lines-demo joystick
