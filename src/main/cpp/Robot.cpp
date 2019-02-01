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


void Robot::RobotInit()
{
	m_driverstation = &DriverStation::GetInstance();
	m_compressor = nullptr;
	if (PCM_COMPRESSOR_SOLENOID != -1)
		m_compressor = new Compressor(PCM_COMPRESSOR_SOLENOID);

	m_operatorinputs = new OperatorInputs();
	m_drivetrain = new DriveTrain(m_operatorinputs);
	m_gyro = new DualGyro(CAN_GYRO1, CAN_GYRO2);
	m_drivepid = new DrivePID(m_drivetrain, m_gyro, m_operatorinputs);
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
	m_drivetrain->Init(DriveTrain::DriveMode::kFollower);
	m_gyro->Init();
	m_lifter->Init();
	m_intake->Init();
}


void Robot::AutonomousPeriodic()
{
	m_drivetrain->Loop();
	m_gyro->Loop();
	m_drivepid->Loop();
	m_lifter->Loop();
	m_intake->CargoLoop();
	m_intake->HatchLoop();
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
	m_drivetrain->Init(DriveTrain::DriveMode::kFollower);
	m_gyro->Init();
	m_lifter->Init();
	m_intake->Init();
}


void Robot::TeleopPeriodic()
{
	m_drivetrain->Loop();
	m_gyro->Loop();
	m_drivepid->Loop();
	m_lifter->Loop();
	m_intake->CargoLoop();
	m_intake->HatchLoop();
}


void Robot::DisabledInit()
{
	DriverStation::ReportError("DisabledInit");

	if (m_compressor != nullptr)
		m_compressor->Stop();
	m_drivetrain->Stop();
	m_gyro->Stop();
	m_drivepid->Stop();
	m_lifter->Stop();
	m_intake->Stop();
}


void Robot::DisabledPeriodic()
{
	m_gyro->Loop();
	m_drivepid->Loop();
}


int main()
{
	return frc::StartRobot<Robot>();
}
