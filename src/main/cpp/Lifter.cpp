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
	m_loopmode = kManual;

	m_lowposition = 0;
	m_mediumposition = 0;
	m_highposition = 0;
	m_selectedposition = 0;

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

	if (Debug) DriverStation::ReportError("LifterInit");

	// do initialization for auto mode
	if (m_ds->IsAutonomous())
	{
		m_motor->SetSelectedSensorPosition(LIF_LIFTERSTART, 0, 0);
	}

	// do initialization for any mode
	m_position = 0;
	m_lowposition = 0;
	m_mediumposition = 0;
	m_highposition = 0;
	m_selectedposition = 0;

	m_motor->StopMotor();
	m_loopmode = kManual;
}


void Lifter::Loop()
{
	if (m_motor == nullptr)
		return;

	m_position = m_motor->GetSelectedSensorPosition(0);
	
	switch (m_loopmode)
	{
	case kManual:
		/// if left bumper and Y override position sensor and raise lift
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) < -0.5) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) &&
			!m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
		{
			m_motor->Set(m_raisespeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
		}
		else
		/// if Y raise lift only if not at max position
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) < -0.5) && (m_position < m_liftermax) && 
			!m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))		/// raise lifter - positive
		{
			if (m_position > m_liftermaxspd)
				m_motor->Set(m_raisespeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			else
				m_motor->Set(m_raisespeed);
		}
		else
		/// if left bumper and X override position sensor and lower lift
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) > 0.5) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
		{
			m_motor->Set(m_lowerspeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			m_motor->SetSelectedSensorPosition(0, 0, 0);
		}
		else
		/// if X lower lift only if not at min position
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) > 0.5) && (m_position > m_liftermin))		/// lower lifter - negative
		{
			if (m_position < m_lifterminspd)
				m_motor->Set(m_lowerspeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			else
				m_motor->Set(m_lowerspeed * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
		}
		else
		/// if x and less than or at min position stop lift
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) > 0.5) && (m_position <= m_liftermin))
		{
			m_motor->StopMotor();
			m_motor->SetSelectedSensorPosition(0, 0, 0);
		}
		else
		{
			m_motor->Set(LIF_LIFTERHOLD);
		}

		/// if Y is pressed, initiate up auto sequence
		if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL) && m_highposition != 0)
		{
			m_selectedposition = FindPosition(kUp);
			m_loopmode = kAutoUp;
		}
		else
		/// if X is pressed, initiate down auto sequence
		if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL) && m_highposition != 0)
		{
			m_selectedposition = FindPosition(kDown);
			m_loopmode = kAutoDown;
		}
		break;

	case kAutoUp:
		/// if position is not at goal, keep raising
		if (m_position < m_selectedposition - LIF_SLACK)
		{
			if (m_position > m_liftermaxspd)
				m_motor->Set(m_raisespeed * 0.5);
			else
				m_motor->Set(m_raisespeed);
		}
		else 
		/// if position is greater than goal, stop and return to manual control
		if (m_position > m_selectedposition - LIF_SLACK)
		{
			m_motor->StopMotor();
			m_loopmode = kManual;
		}

		/// if manual control is sensed, return to manual control
		if (fabs(m_inputs->xBoxRightY(1 * INP_DUAL)) > 0.50)
		{
			m_loopmode = kManual;
		}
		break;

	case kAutoDown:
		/// if position is not at goal, keep lowering
		if (m_position > m_selectedposition + LIF_SLACK)
		{
			if (m_position < m_lifterminspd)
				m_motor->Set(m_lowerspeed * 0.5);
			else
				m_motor->Set(m_lowerspeed);
		}
		else 
		/// if position is less than goal, stop and return to manual control
		if (m_position < m_selectedposition + LIF_SLACK)
		{
			m_motor->StopMotor();
			m_loopmode = kManual;
		}

		/// if manual control is sensed, return to manual control
		if (fabs(m_inputs->xBoxRightY(1 * INP_DUAL)) > 0.50)
		{
			m_loopmode = kManual;
		}
		break;
	}

	SmartDashboard::PutNumber("LI1_liftermin", m_liftermin);
	SmartDashboard::PutNumber("LI2_liftermax", m_liftermax);
	SmartDashboard::PutNumber("LI3_position", m_position);
	SmartDashboard::PutNumber("LI4_mode", m_loopmode);
	SmartDashboard::PutNumber("LI5_selectedposition", m_selectedposition);
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
	m_lowposition = -1;
	m_mediumposition = LIF_HATCH_MID;
	m_highposition = LIF_HATCH_HIGH;
}


void Lifter::SetCargoLevels()
{
	m_lowposition = LIF_CARGO_LOW;
	m_mediumposition = LIF_CARGO_MID;
	m_highposition = LIF_CARGO_HIGH;
}


int Lifter::FindPosition(LifterDir direction)
{
	m_position = m_motor->GetSelectedSensorPosition();

	if (direction == kUp)
	{
		if ((m_position < (m_lowposition - LIF_SLACK)) && (m_lowposition != -1))
		{
			if (Debug) DriverStation::ReportError("Up Low");
			return m_lowposition;
		}
		else 
		if (m_position < (m_mediumposition - LIF_SLACK))
		{
			if (Debug) DriverStation::ReportError("Up Medium");
			return m_mediumposition;
		}
		else 
		if (m_position < (m_highposition - LIF_SLACK))
		{
			if (Debug) DriverStation::ReportError("Up High");
			return m_highposition;
		}
		else
		{
			if (Debug) DriverStation::ReportError("Up Max");
			return m_liftermax;
		}
	}
	else
	if (direction == kDown) 
	{
		if (m_position > (m_highposition + LIF_SLACK))
		{
			if (Debug) DriverStation::ReportError("Down High");
			return m_highposition;
		}
		else 
		if (m_position > (m_mediumposition + LIF_SLACK))
		{
			if (Debug) DriverStation::ReportError("Down Medium");
			return m_mediumposition;
		}
		else 
		if ((m_position > (m_lowposition + LIF_SLACK)) && (m_lowposition != -1))
		{
			if (Debug) DriverStation::ReportError("Down Low");
			return m_lowposition;
		}
		else
		{
			if (Debug) DriverStation::ReportError("Down Min");
			return m_liftermin;
		}
	}
}