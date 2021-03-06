/*
 * Lifter.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: Yreffoeg
 */


#include "Lifter.h"
#include "Const.h"


Lifter::Lifter(DriverStation *ds, OperatorInputs *inputs)
{
	m_ds = ds;
	m_inputs = inputs;

	m_motor = nullptr;
	m_position = 0;
	m_raisespeed = LIF_RAISESPEED;
	m_lowerspeed = LIF_LOWERSPEED;
	m_liftermin = LIF_LIFTERMIN;
	m_liftermax = LIF_LIFTERMAX;
	m_lifterminspd = LIF_LIFTERMINSPD;
	m_liftermaxspd = LIF_LIFTERMAXSPD;
	m_stage = kIdle;

	if (CAN_LIFTER_MOTOR != -1)
	{
		m_motor = new WPI_TalonSRX(CAN_LIFTER_MOTOR);
		m_motor->Set(ControlMode::PercentOutput, 0);
		m_motor->SetNeutralMode(NeutralMode::Brake);
		m_motor->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);
	}
}


Lifter::~Lifter()
{
	if (m_motor != nullptr)
		delete m_motor;
}


void Lifter::Init()
{
	if (m_motor == nullptr)
		return;

	DriverStation::ReportError("LifterInit");

	// do initialization for auto mode
	if (m_ds->IsAutonomous())
	{
		m_motor->SetSelectedSensorPosition(LIF_LIFTERSTART, 0, 0);
	}

	// do initialization for any mode
	m_motor->StopMotor();
	m_stage = kIdle;
}


void Lifter::Loop()
{
	if (m_motor == nullptr)
		return;

	m_position = m_motor->GetSelectedSensorPosition(0);

	/// if left bumper and Y override position sensor and raise lift
	if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && !m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
	{
		m_motor->Set(m_raisespeed * 0.5);
	}
	else
	/// if Y raise list only if not at max position
	if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && (m_position < m_liftermax)  && !m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))		/// raise lifter - positive
	{
		if (m_position > m_liftermaxspd)
			m_motor->Set(m_raisespeed * 0.5);
		else
			m_motor->Set(m_raisespeed);
	}
	else
	/// if left bumper and X override position sensor and lower lift
	if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
	{
		m_motor->Set(m_lowerspeed * 0.5);
		m_motor->SetSelectedSensorPosition(0, 0, 0);
	}
	else
	/// if X lower lift only if not at min position
	if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && (m_position > m_liftermin))		/// lower lifter - negative
	{
		if (m_position < m_lifterminspd)
			m_motor->Set(m_lowerspeed * 0.5);
		else
			m_motor->Set(m_lowerspeed);
	}
	else
	/// if x and less than or at min position stop lift
	if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && (m_position <= m_liftermin))
	{
		m_motor->StopMotor();
		m_motor->SetSelectedSensorPosition(0, 0, 0);
	}
	/// no buttons pressed so check for staging of lifter (pre game)
	else
	{
		switch (m_stage)
		{
		case kIdle:
			if (m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
			{
				m_motor->SetSelectedSensorPosition(0, 0, 0);
				m_stage = kRaise;
			}
			else
			{
				if (m_position > m_liftermin)
					m_motor->Set(-0.05);
				else
					m_motor->StopMotor();
				//m_motor->StopMotor();
			}
			break;

		case kRaise:
			if (m_position < LIF_LIFTERSTART)
				m_motor->Set(m_raisespeed * 0.5);
			else
			{
				m_motor->StopMotor();
				m_stage = kIdle;
			}
			break;
		}
	}

	SmartDashboard::PutNumber("LI1_liftermin", m_liftermin);
	SmartDashboard::PutNumber("LI2_liftermax", m_liftermax);
	SmartDashboard::PutNumber("LI3_position", m_position);
}


void Lifter::TestLoop()
{
	if (m_motor == nullptr)
		return;

	m_position = m_motor->GetSelectedSensorPosition(0);

	if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))		/// raise lifter - positive
	{
		m_motor->Set(m_raisespeed * 0.5);
	}
	else
	if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))		/// lower lifter - negative
	{
		m_motor->Set(m_lowerspeed * 0.5);
	}
	else
	{
		m_motor->StopMotor();
	}

	SmartDashboard::PutNumber("LI1_liftermin", m_liftermin);
	SmartDashboard::PutNumber("LI2_liftermax", m_liftermax);
	SmartDashboard::PutNumber("LI3_position", m_position);
}


void Lifter::Stop()
{
	if (m_motor == nullptr)
		return;

	m_motor->StopMotor();
}


void Lifter::ResetPosition()
{
	if (m_motor == nullptr)
		return;

	m_motor->SetSelectedSensorPosition(0, 0, 0);
	m_position = 0;
}


bool Lifter::IsBottom()
{
	if (m_position <= m_liftermin)
		return true;
	else
		return false;
}


bool Lifter::MoveBottom()
{
	if (m_position > m_liftermin)		/// lower lifter - negative
	{
		if (m_position < m_lifterminspd)
			m_motor->Set(m_lowerspeed * 0.5);
		else
			m_motor->Set(m_lowerspeed);
		return false;
	}
	else
	{
		m_motor->Set(0);
		return true;
	}
}


bool Lifter::AutoRaise()
{
	if (m_position < m_liftermax)		/// raise lifter - positive
	{
		if (m_position > m_liftermaxspd)
			m_motor->Set(m_raisespeed * 0.5);
		else
			m_motor->Set(m_raisespeed);
		return false;
	}
	else
	{
		m_motor->Set(0);
		return true;
	}
}


bool Lifter::AutoRaiseSwitch()
{
	if (m_position < LIF_LIFTERSWITCH)		/// raise lifter - positive
	{
		m_motor->Set(m_raisespeed);
		return false;
	}
	else
	{
		m_motor->Set(0);
		return true;
	}
}
