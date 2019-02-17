/**
 *  Const.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_CONST_H_
#define SRC_CONST_H_


extern bool Debug;                  // Set to true to enable additional debugging


// OperatorInputs
//   Controllers
#define INP_DUAL 1
#define INP_JOYSTICK -1
#define INP_XBOX_1 0
#define INP_XBOX_2 1
//   Set to 1.0 or -1.0
#define INVERT_Y_AXIS 1.0
#define INVERT_X_AXIS -1.0
//   XBox Controller Buttons
#define A_BUTTON  1
#define B_BUTTON  2
#define X_BUTTON  3
#define Y_BUTTON  4
#define LEFT_BUMPER  5
#define RIGHT_BUMPER  6
#define BACK_BUTTON  7
#define START_BUTTON  8
#define L3_BUTTON 9
#define R3_BUTTON 10
//   XBox Triggers -- Changed for 2016, previously XBOX triggers were both on a single axis
#define XBOX_LEFT_TRIGGER_AXIS  2
#define XBOX_RIGHT_TRIGGER_AXIS  3
#define LEFT_TRIGGER_MIN  0.5
#define LEFT_TRIGGER_MAX  1.0
#define RIGHT_TRIGGER_MIN  0.5
#define RIGHT_TRIGGER_MAX  1.0
#define JOYSTICK_X_AXIS  0
#define JOYSTICK_Y_AXIS  1
#define AXIS0_LEFT_MIN -1.0
#define AXIS0_LEFT_MAX -0.75
#define AXIS0_RIGHT_MIN 0.75
#define AXIS0_RIGHT_MAX 1.0
#define AXIS1_BACK_MIN -1.0
#define AXIS1_BACK_MAX -0.75
#define AXIS1_FORWARD_MIN 0.75
#define AXIS1_FORWARD_MAX 1.0
//	 Controller Dead Zones
#define DEADZONE_Y  0.18
#define DEADZONE_X  0.18
#define DEADZONE_Z  0.18


// Drivetrain
//   Direction
#define DT_DEFAULT_DIRECTION -1.0
//   Inverts
#define INVERT_LEFT -1.0
#define INVERT_RIGHT 1.0		// 2017 code is 1, WPILlib DifferentialDrive is -1 (adjusted in DriveTrain::Drive())
//   Talons ports
#define CAN_LEFT_PORT_1 2       // Changed for test bot 2/9/19
#define CAN_LEFT_PORT_2 0
#define CAN_LEFT_PORT_3 -1
#define CAN_RIGHT_PORT_1 1
#define CAN_RIGHT_PORT_2 3
#define CAN_RIGHT_PORT_3 -1
//   Talon parameters
#define MOTOR_CURRENT_LIMIT 20
//   Shifter
#define PCM_SHIFT_MODULE 0
#define PCM_SHIFT_PORT_LOW 0
#define CHILD_PROOF_SPEED 0.75
#define FLIP_HIGH_GEAR true
//   Ramping
#define RAMPING_RATE_PERIOD 0.10
#define RAMPING_RATE_MIN 0.6
#define RAMPING_RATE_MAX 4.0
#define X_SCALING 1.0
#define Y_SCALING 1.0
#define LEFT_MOTOR_SCALING 1
#define RIGHT_MOTOR_SCALING 1
#define LOWSPEED_MODIFIER_X 0.75
#define LOWSPEED_MODIFIER_Y 0.25
//   Encoders
#define ENC_PRESENT_1 true
#define ENC_TYPE_1 FeedbackDevice::QuadEncoder
#define ENC_PRESENT_2 false
#define ENC_TYPE_2 FeedbackDevice::QuadEncoder
#define CODES_PER_REV 5400.0
#define CODES_PER_INCH 286.479
#define WHEEL_DIAMETER 6.0
#define WHEEL_TRACK 23.50


// Compressor
#define PCM_COMPRESSOR_SOLENOID -1      // 0


// Gyro
#define CAN_GYRO1 -1                    // 0
#define CAN_GYRO2 -1


// Lifter
//   Motors
#define CAN_LIFTER_MOTOR1 6             // 4
#define CAN_LIFTER_MOTOR2 -1            // 5
//   Solenoids
#define PCM_LIFTER_MODULE 0			    // 0
#define PCM_LIFTER_SOLENOID 2		    // 2
//   Constants
#define LIF_RAISESPEED -0.5
#define LIF_LOWERSPEED 0.375			// 0.75
#define LIF_LIFTERMAX 24800			    // 24050
#define LIF_LIFTERMIN 300								/// minimum cutoff
#define LIF_LIFTERMINSPD (LIF_LIFTERMAX * 0.1)			/// 5% of max height
#define LIF_LIFTERSTART (LIF_LIFTERMAX * 0.47)			/// ~50% of max height
#define LIF_LIFTERMAXSPD (LIF_LIFTERMAX	* 0.95)			/// 5% of max height
#define LIF_LIFTERHOLD -0.06
#define LIF_HATCH_MID 7000
#define LIF_HATCH_HIGH 14000
#define LIF_CARGO_LOW 6000
#define LIF_CARGO_MID 12000
#define LIF_CARGO_HIGH 18000
#define LIF_SLACK 200
#define LIF_DEADZONE_Y 0.5


// Intake
//   Motors
#define PWM_INTAKE_SPARKCARGO 0
#define PWM_INTAKE_SPARKVAC 1
//   Solenoids
#define PCM_INTAKE_MODULE 0
#define PCM_INTAKE_SOLENOIDCARGO 1
#define PCM_INTAKE_SOLENOIDHATCH 3
#define PCM_INTAKE_SOLENOIDVAC1 4
#define PCM_INTAKE_SOLENOIDVAC2 5	
#define PCM_INTAKE_SOLENOIDVAC3 6
#define PCM_INTAKE_SOLENOIDVAC4 7
//   Digital Inputs
#define DIO_INTAKE_CARGOSENSOR -1       // 0
//   Cargo Constants
#define INT_CARGO_INGEST_WAIT 0.5
#define INT_CARGO_INGEST_SPEED 0.7
#define INT_CARGO_EJECT_WAIT 1.5
#define INT_CARGO_EJECT_SPEED -1.0
//   Hatch Constants
#define INT_VACUUM_WAIT 0.250
#define INT_VACUUM_POW 0.550


#endif /* SRC_CONST_H_ */
