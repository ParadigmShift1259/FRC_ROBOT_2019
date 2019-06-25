/**
 *  Drivetrain.cpp
 *  Date: 6/25/19
 *  Last Edited By: Geoffrey Xue
 */


#include <Drivetrain.h>
#include "Const.h"
#include <cmath>


using namespace std;
using namespace rev;


Drivetrain::Drivetrain(OperatorInputs *inputs, 
					   CANSparkMax *leftmain, CANSparkMax *left2, CANSparkMax *left3,
					   CANSparkMax *rightmain, CANSparkMax *right2, CANSparkMax *right3)
{
	m_inputs = inputs;
	
	m_leftmain = leftmain;
	m_left2 = left2;
	m_left3 = left3;
	m_rightmain = rightmain;
	m_right2 = right2;
	m_right3 = right3;

	m_run = false;
}


Drivetrain::~Drivetrain()
{
	if (m_leftmain != nullptr)
	{	delete m_leftmain;	}
	if (m_left2 != nullptr)
	{	delete m_left2;	}
	if (m_left3 != nullptr)
	{ delete m_left3;	}
	if (m_rightmain != nullptr)
	{	delete m_rightmain;	}
	if (m_right2 != nullptr)
	{	delete m_right2;	}
	if (m_right3 != nullptr)
	{	delete	m_right3;	}
}


void Drivetrain::Init()
{
	// assigning motors to follow one main left and one main right motor
	if (CAN_LEFT_PORT_1 == -1 || CAN_LEFT_PORT_2 == -1 ||
		CAN_RIGHT_PORT_1 == -1 || CAN_RIGHT_PORT_2 == -1)
	{
		DriverStation::ReportError("4WD Drivetrain Ports not assigned correctly");
		m_run = false;
	}
	else
	{
		m_leftmain = new CANSparkMax(CAN_LEFT_PORT_1, CANSparkMax::MotorType::kBrushless);
		m_left2 = new CANSparkMax(CAN_LEFT_PORT_2, CANSparkMax::MotorType::kBrushless);
		m_left2->Follow(*m_leftmain);
	
		m_rightmain = new CANSparkMax(CAN_RIGHT_PORT_1, CANSparkMax::MotorType::kBrushless);
		m_right2 = new CANSparkMax(CAN_RIGHT_PORT_3, CANSparkMax::MotorType::kBrushless);
		m_right2->Follow(*m_rightmain);
		
		m_run = true;
	}

	if (SIX_WHEEL_DRIVE)
	{
		if (CAN_LEFT_PORT_3 == -1 || CAN_RIGHT_PORT_3 == -1)
		{
			DriverStation::ReportError("6WD 2 Drivetrain Ports not assigned correctly");
			m_run = false;
		}
		else
		{
			m_left3 = new CANSparkMax(CAN_LEFT_PORT_3, CANSparkMax::MotorType::kBrushless);
			m_left3->Follow(*m_leftmain);

			m_right3 = new CANSparkMax(CAN_RIGHT_PORT_3, CANSparkMax::MotorType::kBrushless);
			m_right3->Follow(*m_rightmain);

			m_run = true;
		}
	}

	// check if robot is runnable
	if (!m_run)
	{	return;	}

	// inverting sides
	if (REVERSE_DIRECTION)
	{	m_rightmain->SetInverted(true);	}
	else
	{	m_leftmain->SetInverted(true);	}
	
	// setting current limit and enabling voltage compensation on main motors
	m_leftmain->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);
	m_rightmain->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);

	//m_leftmain->EnableVoltageCompensation(MOTOR_VOLTAGE_COMPENSATION);
	//m_rightmain->EnableVoltageCompensation(MOTOR_VOLTAGE_COMPENSATION);

	// setting brake mode for idling
	m_leftmain->SetIdleMode(CANSparkMax::IdleMode::kBrake);
	m_rightmain->SetIdleMode(CANSparkMax::IdleMode::kBrake);

	// setting ramp rate
	m_leftmain->SetClosedLoopRampRate(MOTOR_RAMP_RATE_TIME);
	m_rightmain->SetClosedLoopRampRate(MOTOR_RAMP_RATE_TIME);
}


void Drivetrain::Loop()
{
	if (!m_run)
	{	return;	}

	if (Debug)
	{	ExperimentalData();	}

	double x = m_inputs->xBoxLeftX(0 * INP_DUAL);
	double y = m_inputs->xBoxLeftY(0 * INP_DUAL);

	// drive logic here
}


void Drivetrain::Stop()
{
}

// prints interesting data to dashboard or shuffleboard
void Drivetrain::ExperimentalData()
{
	// temperature
	SmartDashboard::PutNumber("DT_Left1 Temperature", m_leftmain->GetMotorTemperature());
	SmartDashboard::PutNumber("DT_Left2 Temperature", m_left2->GetMotorTemperature());

	SmartDashboard::PutNumber("DT_Right1 Temperature", m_rightmain->GetMotorTemperature());
	SmartDashboard::PutNumber("DT_Right2 Temperature", m_right2->GetMotorTemperature());

	if (SIX_WHEEL_DRIVE)
	{
		SmartDashboard::PutNumber("DT_Left3 Temperature", m_left3->GetMotorTemperature());
		SmartDashboard::PutNumber("DT_Right3 Temperature", m_right3->GetMotorTemperature());
	}
}