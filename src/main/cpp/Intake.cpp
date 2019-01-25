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
	m_ds = ds;
	m_inputs = inputs;
	m_lifter = lifter;

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
	m_stage = kTemporary;
}


Intake::~Intake()
{
	if (m_motor != nullptr)
		delete m_motor;
	if (m_solenoid1 != nullptr)
		delete m_solenoid1;
	if (m_solenoid2 != nullptr)
		delete m_solenoid2;
}



void Intake::Init()
{
	if (m_motor == nullptr || m_solenoid1 == nullptr || m_solenoid2 == nullptr)
		return;

	DriverStation::ReportError("IntakeInit");

	m_motor->StopMotor();
	m_solenoid1->Set(false);
	m_solenoid2->Set(false);
}


void Intake::Loop()
{
	if (m_motor == nullptr || m_solenoid1 == nullptr || m_solenoid2 == nullptr)
		return;

	/// check A Button to record state of button toggle (used in kBox)
	bool xboxabuttontoggle = m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL);

	bool xboxybuttontoggle = m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0);

	switch (m_stage)
	{
	case kTemporary:
		m_motor->StopMotor();
		m_solenoid1->Set(false);
		m_solenoid2->Set(false);				/// motors are off by default
			/// check for lifter to be on the bottom

				/// open intake arms
				/// lifter is at bottom, go to ingest stage
	};

	SmartDashboard::PutNumber("IN1_leftmotor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN3_solenoid1", m_solenoid1->Get());
	SmartDashboard::PutNumber("IN5_stage", m_stage);
}

void Intake::TestLoop()
{
	if ((m_motor == nullptr) || (m_solenoid1 == nullptr) || (m_solenoid2 == nullptr))
		return;

	SmartDashboard::PutNumber("IN1_motor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN3_solenoid1", m_solenoid1->Get());
	SmartDashboard::PutNumber("IN3_solenoid2", m_solenoid2->Get());
	SmartDashboard::PutNumber("IN5_stage", m_stage);
}


void Intake::Stop()
{
	if (m_motor == nullptr || m_solenoid1 == nullptr || m_solenoid2 == nullptr)
		return;

	m_motor->StopMotor();
	m_solenoid1->Set(false);
	m_solenoid2->Set(false);
	m_timer.Stop();
}