#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_


#include <frc\WPILib.h>
#include "Const.h"
#include "OperatorInputs.h"
#include "Drivetrain.h"
#include "Gyro.h"
#include "DrivePID.h"
#include "Lifter.h"
#include "Intake.h"


using namespace frc;


class Robot : public TimedRobot
{
public:
	virtual void RobotInit();
	virtual void RobotPeriodic();
	virtual void AutonomousInit();
	virtual void AutonomousPeriodic();
	virtual void TeleopInit();
	virtual void TeleopPeriodic();
	virtual void TestInit();
	virtual void TestPeriodic();
	virtual void DisabledInit();
	virtual void DisabledPeriodic();

protected:
	DriverStation *m_driverstation;
	Compressor *m_compressor;
	OperatorInputs *m_operatorinputs;
	DriveTrain *m_drivetrain;
	DualGyro *m_gyro;
	DrivePID *m_drivepid;
	Lifter *m_lifter;
	Intake *m_intake;
};


#endif /* SRC_ROBOT_H_ */
