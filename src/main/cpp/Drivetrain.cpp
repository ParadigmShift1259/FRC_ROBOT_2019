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
                       CANSparkMax *left1, CANSparkMax *left2, CANSparkMax *left3,
                       CANSparkMax *right1, CANSparkMax *right2, CANSparkMax *right3)
{
    m_inputs = inputs;
    
    m_left1 = left1;
    m_left2 = left2;
    m_left3 = left3;
    m_right1 = right1;
    m_right2 = right2;
    m_right3 = right3;

    m_leftenc = nullptr;
    m_rightenc = nullptr;

    m_drive = nullptr;

    m_inited = false;
    m_ramprate = MOTOR_RAMP_RATE_TIME;
}


Drivetrain::~Drivetrain()
{
    if (m_left1 != nullptr)
    	delete m_left1;
    if (m_left2 != nullptr)
    	delete m_left2;
    if (m_left3 != nullptr)
       delete m_left3;
    if (m_right1 != nullptr)
    	delete m_right1;
    if (m_right2 != nullptr)
    	delete m_right2;
    if (m_right3 != nullptr)
        delete	m_right3;
}


void Drivetrain::Init()
{
    // assigning motors to follow one main left and one main right motor
    if ((CAN_LEFT_PORT_1 == -1) || (CAN_LEFT_PORT_2 == -1) ||
        (CAN_RIGHT_PORT_1 == -1) || (CAN_RIGHT_PORT_2 == -1))
    {
        DriverStation::ReportError("4WD Drivetrain Ports not assigned correctly");
        m_inited = false;
        return;
    }

    m_left1 = new CANSparkMax(CAN_LEFT_PORT_1, CANSparkMax::MotorType::kBrushless);
    m_left2 = new CANSparkMax(CAN_LEFT_PORT_2, CANSparkMax::MotorType::kBrushless);
    m_left2->Follow(*m_left1);

    m_right1 = new CANSparkMax(CAN_RIGHT_PORT_1, CANSparkMax::MotorType::kBrushless);
    m_right2 = new CANSparkMax(CAN_RIGHT_PORT_3, CANSparkMax::MotorType::kBrushless);
    m_right2->Follow(*m_right1);
    
    m_inited = true;


    if (SIX_WHEEL_DRIVE)
    {
        if ((CAN_LEFT_PORT_3 == -1) || (CAN_RIGHT_PORT_3 == -1))
        {
            DriverStation::ReportError("6WD 2 Drivetrain Ports not assigned correctly");
            m_inited = false;
            return;
        }

        m_left3 = new CANSparkMax(CAN_LEFT_PORT_3, CANSparkMax::MotorType::kBrushless);
        m_left3->Follow(*m_left1);

        m_right3 = new CANSparkMax(CAN_RIGHT_PORT_3, CANSparkMax::MotorType::kBrushless);
        m_right3->Follow(*m_right1);

        m_inited = true;
    }

    // inverting sides
    if (INVERT_LEFT)
        m_left1->SetInverted(true);
    
    if (INVERT_RIGHT)
        m_right1->SetInverted(true);

    
    // setting current limit and enabling voltage compensation on main motors
    m_left1->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);
    m_left2->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);
    if (m_left3)
        m_left3->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);

    m_right1->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);
    m_right2->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);
    if (m_right3)
        m_right3->SetSmartCurrentLimit(MOTOR_CURRENT_LIMIT);

    m_left1->EnableVoltageCompensation(true);
    m_left2->EnableVoltageCompensation(true);
    if (m_left3)
        m_left3->EnableVoltageCompensation(true);

    m_right1->EnableVoltageCompensation(true);
    m_right2->EnableVoltageCompensation(true);

    // setting brake mode for idling
    SetBrakeMode();

    // setting ramp rate
    SetRampRate(MOTOR_RAMP_RATE_TIME);

    m_leftenc = new CANEncoder(*m_left1);
    m_rightenc = new CANEncoder(*m_right1);

    m_drive = new DifferentialDrive(*m_left1, *m_right1);
}


void Drivetrain::Loop()
{
    if (!m_inited)
    {	return;	}

    if (Debug)
    {	ExperimentalData();	}

    double x = m_inputs->xBoxLeftX(0 * INP_DUAL);
    double y = m_inputs->xBoxLeftY(0 * INP_DUAL);

    m_drive->ArcadeDrive(y, x);
}


void Drivetrain::Stop()
{
}


void Drivetrain::SetRampRate(double rate)
{
    if (!m_inited)
        return;

    m_ramprate = rate;
    m_left1->SetOpenLoopRampRate(m_ramprate);
    m_right1->SetOpenLoopRampRate(m_ramprate);
}


void Drivetrain::SetBrakeMode()
{
    if (!m_inited)
        return;
    
    m_left1->SetIdleMode(CANSparkMax::IdleMode::kBrake);
    m_left2->SetIdleMode(CANSparkMax::IdleMode::kBrake);
    if (m_left3)
        m_left3->SetIdleMode(CANSparkMax::IdleMode::kBrake);

    m_right1->SetIdleMode(CANSparkMax::IdleMode::kBrake);
    m_right2->SetIdleMode(CANSparkMax::IdleMode::kBrake);
    if (m_right3)
        m_right3->SetIdleMode(CANSparkMax::IdleMode::kBrake);
}

void Drivetrain::SetCoastMode()
{
    if (!m_inited)
        return;

    m_left1->SetIdleMode(CANSparkMax::IdleMode::kCoast);
    m_left2->SetIdleMode(CANSparkMax::IdleMode::kCoast);
    if (m_left3)
        m_left3->SetIdleMode(CANSparkMax::IdleMode::kCoast);

    m_right1->SetIdleMode(CANSparkMax::IdleMode::kCoast);
    m_right2->SetIdleMode(CANSparkMax::IdleMode::kCoast);
    if (m_right3)
        m_right3->SetIdleMode(CANSparkMax::IdleMode::kCoast);
}

// prints interesting data to dashboard or shuffleboard
void Drivetrain::ExperimentalData()
{
    // temperature
    SmartDashboard::PutNumber("DT_Left1 Temperature", m_left1->GetMotorTemperature());
    SmartDashboard::PutNumber("DT_Left2 Temperature", m_left2->GetMotorTemperature());

    SmartDashboard::PutNumber("DT_Right1 Temperature", m_right1->GetMotorTemperature());
    SmartDashboard::PutNumber("DT_Right2 Temperature", m_right2->GetMotorTemperature());

    if (SIX_WHEEL_DRIVE)
    {
        SmartDashboard::PutNumber("DT_Left3 Temperature", m_left3->GetMotorTemperature());
        SmartDashboard::PutNumber("DT_Right3 Temperature", m_right3->GetMotorTemperature());
    }

    // encoders
    SmartDashboard::PutNumber("DT_LeftENC Position", m_leftenc->GetPosition());
    SmartDashboard::PutNumber("DT_LeftENC Velocity", m_leftenc->GetVelocity());

    SmartDashboard::PutNumber("DT_RightENC Position", m_rightenc->GetPosition());
    SmartDashboard::PutNumber("DT_RightENC Velocity", m_rightenc->GetVelocity());
}