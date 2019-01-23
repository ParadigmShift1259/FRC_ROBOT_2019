/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/


#include "Robot.h"
#include <iostream>
#include <string>
#include <frc/LiveWindow/LiveWindow.h>
#include <frc/SmartDashboard/SendableChooser.h>
#include <frc/SmartDashboard/SmartDashboard.h>


using namespace std;


void Robot::VisionThread()
{
	cs::UsbCamera camera = CameraServer::GetInstance()->StartAutomaticCapture();
	camera.SetResolution(320, 240);
	cs::CvSink cvSink = CameraServer::GetInstance()->GetVideo();
	cs::CvSource outputStreamStd = CameraServer::GetInstance()->PutVideo("Gray", 320, 240);
	cv::Mat source;
	cv::Mat output;
	while (true) 
	{
		cvSink.GrabFrame(source);
		//cvtColor(source, output, cv::COLOR_BGR2GRAY);
		outputStreamStd.PutFrame(source);
		Wait(0.02);
	}
}


void Robot::RobotInit()
{
	thread visionThread(VisionThread);
	visionThread.detach();

	m_operatorinputs = new OperatorInputs();
	m_drivetrain = new DriveTrain(m_operatorinputs);
	m_gyro = new DualGyro(CAN_GYRO_1, CAN_GYRO_2);
	m_pneumatic = new Pneumatic(m_driverstation, m_operatorinputs);
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
}


void Robot::AutonomousPeriodic()
{
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

	m_drivetrain->Init(DriveTrain::DriveMode::kDiscrete);
	m_gyro->Init();
	m_pneumatic->Init();
}


void Robot::TeleopPeriodic()
{
	m_drivetrain->Loop();
	m_gyro->Loop();
	m_pneumatic->Loop();
}


void Robot::DisabledInit()
{
	m_drivetrain->Stop();
	m_gyro->Stop();
	m_pneumatic->Stop();
}


void Robot::DisabledPeriodic()
{
	m_gyro->Loop();
}


#ifndef RUNNING_FRC_TESTS
int main()
{
	return StartRobot<Robot>();
}
#endif
