/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/


#include <iostream>
#include <string>
#include <frc/LiveWindow/LiveWindow.h>
#include <frc/SmartDashboard/SendableChooser.h>
#include <frc/SmartDashboard/SmartDashboard.h>
#include "Robot.h"


bool Debug = true;
bool StartedInAuto = false;


void Robot::RobotInit()
{
	m_driverstation = &DriverStation::GetInstance();

	m_operatorinputs = new OperatorInputs();
	m_gyrodrive = new GyroDrive(m_operatorinputs);
	m_pneumatics = new Pneumatics();
	m_autonomous = new Autonomous(m_operatorinputs, m_gyrodrive);
	m_lifter = new Lifter(m_driverstation, m_operatorinputs);
	m_intake = new Intake(m_driverstation, m_operatorinputs, m_lifter, m_drivepid);
}


void Robot::RobotPeriodic()
{
}


/*
 * This autonomous (along with the chooser code above) shows how to
 * select
 * between different autonomous modes using the dashboard. The sendable
 * chooser code works with the Java SmartDashboard. If you prefer the
 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
 * GetString line to get the auto name from the text box below the Gyro.
 *
 * You can add additional auto modes by adding additional comparisons to
 * the
 * if-else structure below with additional strings. If using the
 * SendableChooser make sure to add them to the chooser code above as
 * well.
 */
void Robot::AutonomousInit()
{
	DriverStation::ReportError("AutonomousInit");

	StartedInAuto = true;

	m_pneumatics->Init();
	m_gyrodrive->Init();
	m_intake->Init();
	m_lifter->Init();
}


void Robot::AutonomousPeriodic()
{
	m_gyrodrive->Loop();
	m_intake->Loop();
	m_lifter->Loop();
}


void Robot::TestInit()				// Do not use
{
	DriverStation::ReportError("TestInit");
}


void Robot::TestPeriodic()			// Do not use
{
}


void Robot::TeleopInit()
{
	DriverStation::ReportError("TeleopInit");

	m_pneumatics->Init();
	m_gyrodrive->Init();
	m_intake->Init();
	m_lifter->Init();
}


void Robot::TeleopPeriodic()
{
	m_pneumatics->Loop();
	m_gyrodrive->Loop();
	m_intake->Loop();
	m_lifter->Loop();
}


void Robot::DisabledInit()
{
	DriverStation::ReportError("DisabledInit");

	StartedInAuto = false;			// WIP

	m_pneumatics->Stop();
	m_gyrodrive->Stop();
	m_intake->Stop();
	m_lifter->Stop();
}


void Robot::DisabledPeriodic()
{
	m_gyrodrive->Disabled();
}


int main()
{
	return frc::StartRobot<Robot>();
}
