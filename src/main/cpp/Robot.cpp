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
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>


<<<<<<< HEAD
AutoMode automode = kAutoStraight;


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
//	thread visionThread(VisionThread);
//	visionThread.detach();
	
	m_chooser.AddDefault(kszAutoDefault, kszAutoDefault);
	m_chooser.AddObject(kszAutoCenterSwitch1, kszAutoCenterSwitch1);
	m_chooser.AddObject(kszAutoCenterSwitch3, kszAutoCenterSwitch3);
	m_chooser.AddObject(kszAutoLeftScale2X, kszAutoLeftScale2X);
	m_chooser.AddObject(kszAutoRightScale2X, kszAutoRightScale2X);
	m_chooser.AddObject(kszAutoRightScale2X, kszAutoRightScale2X);
	m_chooser.AddObject(kszAutoLeftScale1P, kszAutoLeftScale1P);
	m_chooser.AddObject(kszAutoRightScale1P, kszAutoRightScale1P);
	m_chooser.AddObject(kszAutoLeftSwitch, kszAutoLeftSwitch);
	m_chooser.AddObject(kszAutoRightSwitch, kszAutoRightSwitch);
	m_chooser.AddObject(kszAutoTestMode, kszAutoTestMode);
	frc::SmartDashboard::PutData("Auto Modes", &m_chooser);

=======
void Robot::RobotInit()
{
>>>>>>> c07b734475960ac1ee09eb43745eec6d26161ac4
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


int main()
{
	return frc::StartRobot<Robot>();
}
