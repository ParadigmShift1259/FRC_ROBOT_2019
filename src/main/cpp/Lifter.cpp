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

	m_lowposition = 0;
	m_mediumposition = 0;
	m_highposition = 0;
	m_selectedposition = 0;

	m_manualraising = false;
	m_autoraising = false;

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

	m_manualraising = false;
	m_autoraising = false;
}


void Lifter::ManualLoop()
{
	if (m_motor == nullptr)
		return;

	m_position = m_motor->GetSelectedSensorPosition(0);

	/// if left bumper and Y override position sensor and raise lift
	if ((m_inputs->xBoxRightY(1 * INP_DUAL) < -0.5) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && !m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
	{
		m_motor->Set(m_raisespeed * 0.5);
		m_manualraising = true;
	}
	else
	/// if Y raise list only if not at max position
	if ((m_inputs->xBoxRightY(1 * INP_DUAL) < -0.5) && (m_position < m_liftermax)  && !m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))		/// raise lifter - positive
	{
		if (m_position > m_liftermaxspd)
			m_motor->Set(m_raisespeed * 0.5);
		else
			m_motor->Set(m_raisespeed);
		m_manualraising = true;
	}
	else
	/// if left bumper and X override position sensor and lower lift
	if ((m_inputs->xBoxRightY(1 * INP_DUAL) > 0.5) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
	{
		m_motor->Set(m_lowerspeed * 0.5);
		m_motor->SetSelectedSensorPosition(0, 0, 0);
		m_manualraising = true;
	}
	else
	/// if X lower lift only if not at min position
	if ((m_inputs->xBoxRightY(1 * INP_DUAL) > 0.5) && (m_position > m_liftermin))		/// lower lifter - negative
	{
		if (m_position < m_lifterminspd)
			m_motor->Set(m_lowerspeed * 0.5);
		else
			m_motor->Set(m_lowerspeed);
		m_manualraising = true;
	}
	else
	/// if x and less than or at min position stop lift
	if ((m_inputs->xBoxRightY(1 * INP_DUAL) > 0.5) && (m_position <= m_liftermin))
	{
		m_motor->StopMotor();
		m_motor->SetSelectedSensorPosition(0, 0, 0);
		m_manualraising = true;
	}

	SmartDashboard::PutNumber("LI1_liftermin", m_liftermin);
	SmartDashboard::PutNumber("LI2_liftermax", m_liftermax);
	SmartDashboard::PutNumber("LI3_position", m_position);
}


void Lifter::AutoLoop()
{
	m_position = m_motor->GetSelectedSensorPosition();

	if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
	{
		if (m_highposition < m_position)
		{
			GoToPosition(m_highposition);
			m_autoraising = true;
		}
		else if (m_mediumposition < m_position)
		{
			GoToPosition(m_mediumposition);
			m_autoraising = true;
		}
		else if (m_lowposition < m_position && m_lowposition != 0)
		{
			GoToPosition(m_lowposition);
			m_autoraising = true;
		}
		else
		{
			GoToPosition(m_liftermin);
			m_autoraising = true;
		}
	}
	else if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
	{
		if (m_lowposition > m_position && m_lowposition != 0)
		{
			GoToPosition(m_lowposition);
			m_autoraising = true;
		}
		else if (m_mediumposition < m_position)
		{
			GoToPosition(m_mediumposition);
			m_autoraising = true;
		}
		else if (m_highposition < m_position)
		{
			GoToPosition(m_highposition);
			m_autoraising = true;
		}
		else
		{
			GoToPosition(m_liftermax);
			m_autoraising = true;
		}
	}
	else
	{
		m_motor->StopMotor();
	}
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


void Lifter::SetHatchLevels()
{
	m_lowposition = 0;
	m_mediumposition = LIF_HATCH_MID;
	m_highposition = LIF_HATCH_HIGH;
}


void Lifter::SetCargoLevels()
{
	m_lowposition = LIF_CARGO_LOW;
	m_mediumposition = LIF_CARGO_MID;
	m_highposition = LIF_CARGO_HIGH;
}

bool Lifter::GoToPosition(int designatedposition)		// Magic numbers are for slack
{
	m_position = m_motor->GetSelectedSensorPosition();

	if (m_manualraising)
		return true;

	if (m_position > designatedposition + 20)
	{
		if (m_position < m_lifterminspd)
			m_motor->Set(m_lowerspeed * 0.5);
		else
			m_motor->Set(m_lowerspeed);
		return false;
	}
	else if (m_position < designatedposition - 20)
	{
		if (m_position > m_liftermaxspd)
			m_motor->Set(m_raisespeed * 0.5);
		else
			m_motor->Set(m_raisespeed);
		return false;
	}
	else if (m_position > designatedposition - 20 && m_position < designatedposition + 20)
	{
		m_motor->StopMotor();
		return true;
	}
}