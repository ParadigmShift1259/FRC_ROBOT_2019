#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_


#include <frc\WPILib.h>
#include "Const.h"
#include "OperatorInputs.h"
#include "GyroDrive.h"
#include "Autonomous.h"
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
	GyroDrive *m_gyrodrive;
	Autonomous *m_autonomous;
	Lifter *m_lifter;
	Intake *m_intake;
	DrivePID *m_drivepid;
};


#endif /* SRC_ROBOT_H_ */
