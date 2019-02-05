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


Direction direction = kNothing;
Side side = kRight;


void Robot::RobotInit()
{
	m_directionchooser.AddDefault(kszNothing, kszNothing);
	m_directionchooser.AddObject(kszCargoLoading, kszCargoLoading);
	m_directionchooser.AddObject(kszCargoShip, kszCargoShip);
	m_directionchooser.AddObject(kszRocketClose, kszRocketClose);
	m_directionchooser.AddObject(kszRocketMedium, kszRocketMedium);
	m_directionchooser.AddObject(kszRocketFar, kszRocketFar);
	frc::SmartDashboard::PutData("Directions", &m_directionchooser);

	m_sidechooser.AddDefault(kszLeft, kszLeft);
	m_sidechooser.AddDefault(kszRight, kszRight);
	frc::SmartDashboard::PutData("Side", &m_sidechooser);

	m_driverstation = &DriverStation::GetInstance();
	m_compressor = nullptr;
	if (PCM_COMPRESSOR_SOLENOID != -1)
		m_compressor = new Compressor(PCM_COMPRESSOR_SOLENOID);

	m_operatorinputs = new OperatorInputs();
	m_gyrodrive = new GyroDrive(m_operatorinputs);
	m_autonomous = new Autonomous(m_operatorinputs, m_gyrodrive);
	m_lifter = new Lifter(m_driverstation, m_operatorinputs);
	m_intake = new Intake(m_driverstation, m_operatorinputs, m_lifter);
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

	if (m_compressor != nullptr)
		m_compressor->Stop();
	m_gyrodrive->Init();
	m_autonomous->Init();
//	m_lifter->Init();
//	m_intake->Init();
}


void Robot::AutonomousPeriodic()
{
	m_gyrodrive->Loop();
	m_autonomous->Loop();
	ReadChooser();
//	m_lifter->Loop();
//	m_intake->CargoLoop();
//	m_intake->HatchLoop();
}


void Robot::TestInit()
{
	DriverStation::ReportError("TestInit");
}


void Robot::TestPeriodic()
{
}


void Robot::TeleopInit()
{
	DriverStation::ReportError("TeleopInit");

	if (m_compressor != nullptr)
		m_compressor->Start();
	m_gyrodrive->Init();
	m_autonomous->Init();
//	m_lifter->Init();
//	m_intake->Init();
}


void Robot::TeleopPeriodic()
{
	m_gyrodrive->Loop();
	m_autonomous->Loop();
	ReadChooser();
//	m_lifter->Loop();
//	m_intake->CargoLoop();
//	m_intake->HatchLoop();
}


void Robot::DisabledInit()
{
	DriverStation::ReportError("DisabledInit");

	if (m_compressor != nullptr)
		m_compressor->Stop();
	m_gyrodrive->Stop();
	m_autonomous->Stop();
//	m_lifter->Stop();
//	m_intake->Stop();
}


void Robot::DisabledPeriodic()
{
	m_gyrodrive->Disabled();
}


void Robot::ReadChooser()
{
	m_directionSelected = m_directionchooser.GetSelected();

	if (m_directionSelected == kszNothing)
		direction = kNothing;
	if (m_directionSelected == kszCargoLoading)
		direction = kCargoLoading;
	if (m_directionSelected == kszCargoShip)
		direction = kCargoShip;
	if (m_directionSelected == kszRocketClose)
		direction = kRocketClose;
	if (m_directionSelected == kszRocketMedium)
		direction = kRocketMedium;
	if (m_directionSelected == kszRocketFar)
		direction = kRocketFar;

	m_sideSelected = m_sidechooser.GetSelected();

	if (m_sideSelected == kszLeft)
		side = kLeft;
	if (m_sideSelected == kszRight)
		side = kRight;

	SmartDashboard::PutNumber("Direction", direction);
	SmartDashboard::PutNumber("Side", side);
}


int main()
{
	return frc::StartRobot<Robot>();
}
