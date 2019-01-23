#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_


#include <frc\WPILib.h>
#include "Const.h"
#include "OperatorInputs.h"
#include "Drivetrain.h"
#include "Gyro.h"
#include "Pneumatic.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>


using namespace frc;


class Robot : public TimedRobot
{
public:
	static void VisionThread();
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
	OperatorInputs *m_operatorinputs;
	DriveTrain *m_drivetrain;
	DualGyro *m_gyro;	
	Pneumatic *m_pneumatic;
	DriverStation *m_driverstation;
};


#endif /* SRC_ROBOT_H_ */
