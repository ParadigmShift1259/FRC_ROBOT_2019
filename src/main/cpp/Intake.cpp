/*
 * Intake.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: Jival
 */

#include <Intake.h>
#include "Const.h"


Intake::Intake(DriverStation *ds, OperatorInputs *inputs, Lifter *lifter)
{
	m_motor = nullptr;
	m_solenoid1 = nullptr;
	m_solenoid2 = nullptr;
    m_solenoid3 = nullptr;
    m_solenoid4 = nullptr;
    m_spark1 = nullptr;
    m_spark2 = nullptr;
	m_ds = ds;
	m_inputs = inputs;
	m_lifter = lifter;
	m_waittime = PCM_PNEUMATIC_WAIT;

	if (CAN_INTAKE_MOTOR != -1)
	{
		m_motor = new WPI_TalonSRX(CAN_INTAKE_MOTOR);
		m_motor->Set(ControlMode::PercentOutput, 0);
		m_motor->SetNeutralMode(NeutralMode::Brake);
	}

	if (PCM_INTAKE_SOLENOID1 != -1)
		m_solenoid1 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOID1);
	if (PCM_INTAKE_SOLENOID2 != -1)
		m_solenoid2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOID2);
	if (PCM_INTAKE_SOLENOID3 != -1)
		m_solenoid2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOID3);
	if (PCM_INTAKE_SOLENOID4 != -1)
		m_solenoid2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOID4);
	
	if (SPARK1 != -1)
		m_spark1 = new Spark(SPARK1);
	if (SPARK2 != -1)
    	m_spark2 = new Spark(SPARK2);
	
	m_cargosensor = new DigitalInput(DIO_INTAKE_CARGOSENSOR);
	m_stage = kOpen;
}


Intake::~Intake()
{
	if (m_motor != nullptr)
		delete m_motor;
	if (m_solenoid1 != nullptr)
        delete m_solenoid1;
    if (m_solenoid2 != nullptr)
        delete m_solenoid2;
    if (m_solenoid3 != nullptr)
        delete m_solenoid3;
    if (m_solenoid4 != nullptr)
        delete m_solenoid4;
    if (m_spark1 != nullptr)
        delete m_spark1;
    if (m_spark2 != nullptr)
        delete m_spark2;
}



void Intake::Init()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;
	
	DriverStation::ReportError("IntakeInit");

	m_motor->StopMotor();
	m_solenoid1->Set(false);
	m_solenoid2->Set(false);
	m_solenoid3->Set(false);
	m_solenoid4->Set(false);
    m_spark1->Set(0);
    m_spark2->Set(0);
}


void Intake::Loop()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;

	/// check A Button to record state of button toggle (used in kBox)
	bool xboxabuttontoggle = m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL);

	bool xboxybuttontoggle = m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0);

    switch (m_stage)
    {
    case kOpen:
    	DriverStation::ReportError("kOpen");

		m_motor->StopMotor();
        m_solenoid1->Set(false);
        m_solenoid2->Set(false);
        m_solenoid3->Set(false);
        m_solenoid4->Set(false);
        m_spark1->Set(0);
        m_spark2->Set(0);
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_stage = kCapture;
        }
        break;

    case kCapture:
    	DriverStation::ReportError("kCapture");

		m_motor->StopMotor();
        m_solenoid1->Set(false);
        m_solenoid2->Set(false);
        m_solenoid3->Set(false);
        m_solenoid4->Set(false);
        m_spark1->Set(0.3);
        m_spark2->Set(0.3);
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
			m_motor->StopMotor();
            m_solenoid1 -> Set(true);
            m_solenoid2 -> Set(true);
            m_solenoid3 -> Set(true);
            m_solenoid4 -> Set(true);
            m_timer.Reset();
            m_stage = kClose;
        }
        else
        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_waittime += .05;
        }
        else
        if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL) && (m_waittime > PCM_PNEUMATIC_WAIT))
        {
            m_waittime -= .05;
        }
        break;

    case kClose:
    	DriverStation::ReportError("kClose");

		m_motor->StopMotor();
        m_spark1->Set(0);
        m_spark2->Set(0);
        if (m_timer.Get() > m_waittime)
        {
			m_motor->StopMotor();
            m_solenoid1 -> Set(false);
            m_solenoid2 -> Set(false);
            m_solenoid3 -> Set(false);
            m_solenoid4 -> Set(false);
            m_stage = kOpen;
        }
        break;
    }

	SmartDashboard::PutNumber("IN1_motor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN3_solenoid1", m_solenoid1->Get());
	SmartDashboard::PutNumber("IN4_solenoid2", m_solenoid2->Get());
	SmartDashboard::PutNumber("IN5_solenoid3", m_solenoid3->Get());
	SmartDashboard::PutNumber("IN6_solenoid4", m_solenoid4->Get());
	SmartDashboard::PutNumber("IN7_stage", m_stage);
	SmartDashboard::PutNumber("IN8_waittime", m_waittime);
	SmartDashboard::PutNumber("IN9_cargosensor", m_cargosensor->Get());
}

void Intake::TestLoop()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;
	
	m_motor->StopMotor();
	m_solenoid1->Set(false);
	m_solenoid2->Set(false);
	m_solenoid3->Set(false);
	m_solenoid4->Set(false);
	m_spark1->Set(0);
	m_spark2->Set(0);
	
	SmartDashboard::PutNumber("IN1_motor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN3_solenoid1", m_solenoid1->Get());
	SmartDashboard::PutNumber("IN4_solenoid2", m_solenoid2->Get());
	SmartDashboard::PutNumber("IN5_solenoid3", m_solenoid3->Get());
	SmartDashboard::PutNumber("IN6_solenoid4", m_solenoid4->Get());
	SmartDashboard::PutNumber("IN7_stage", m_stage);
	SmartDashboard::PutNumber("IN8_waittime", m_waittime);
	SmartDashboard::PutNumber("IN9_cargosensor", m_cargosensor->Get());
}


void Intake::Stop()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;

	m_motor->StopMotor();
	m_solenoid1->Set(false);
	m_solenoid2->Set(false);
	m_solenoid3->Set(false);
	m_solenoid4->Set(false);
	m_spark1->Set(0);
	m_spark2->Set(0);
	m_timer.Stop();
}