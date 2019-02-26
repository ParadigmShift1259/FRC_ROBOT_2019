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

AutoMode automode = kAutoDefault;


void Robot::RobotInit()
{
	m_chooser.SetDefaultOption(kszAutoDefault, kszAutoDefault);
	m_chooser.AddOption(kszAutoLeft, kszAutoLeft);
	m_chooser.AddOption(kszAutoCenter, kszAutoCenter);
	m_chooser.AddOption(kszAutoRight, kszAutoRight);
	SmartDashboard::PutData("Auto Modes", &m_chooser);

	m_driverstation = &DriverStation::GetInstance();

	m_operatorinputs = new OperatorInputs();
	m_gyrodrive = new GyroDrive(m_operatorinputs);
	m_pneumatics = new Pneumatics();
	m_intake = new Intake(m_driverstation, m_operatorinputs);
	m_lifter = new Lifter(m_driverstation, m_operatorinputs, m_intake);
	m_autonomous = new Autonomous(m_operatorinputs, m_gyrodrive, m_lifter, m_intake);
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
	m_timer.Start();
	m_timer.Reset();

	ReadChooser();

	m_gyrodrive->Init();
	m_pneumatics->Init();
	m_autonomous->Init();
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
	if (!StartedInAuto)
	{
		DriverStation::ReportError("TeleopInit");

		m_pneumatics->Init();
		m_gyrodrive->Init();
		m_intake->Init();
		m_lifter->Init();
	}
	StartedInAuto = false;
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
	if (!StartedInAuto)
	{
		DriverStation::ReportError("DisabledInit");

		m_pneumatics->Stop();
		m_gyrodrive->Stop();
		m_intake->Stop();
		m_lifter->Stop();
	}
}


void Robot::DisabledPeriodic()
{
	if (StartedInAuto && m_timer.Get() > 2.0)
	{
		StartedInAuto = false;
		DisabledInit();
	}
	
	ReadChooser();

	m_gyrodrive->Disabled();
}


void Robot::ReadChooser()
{
	m_autoSelected = m_chooser.GetSelected();

	automode = kAutoDefault;
	if (m_autoSelected == kszAutoLeft)
		automode = kAutoLeft;
	else
	if (m_autoSelected == kszAutoCenter)
		automode = kAutoCenter;
	else
	if (m_autoSelected == kszAutoRight)
		automode = kAutoRight;

	SmartDashboard::PutNumber("AU1_automode", automode);
}


int main()
{
	return frc::StartRobot<Robot>();
}
