/*
 * Lifter.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: Yreffoeg
 */


#include "Lifter.h"
#include "Intake.h"
#include "Const.h"


Lifter::Lifter(DriverStation *ds, OperatorInputs *inputs, Intake *intake)
{
	m_ds = ds;
	m_inputs = inputs;
	m_intake = intake;

	m_motor = nullptr;
	m_motorslave = nullptr;
	m_solenoid = nullptr;

	m_position = 0;
	m_raisespeed = LIF_RAISESPEED;
	m_lowerspeed = LIF_LOWERSPEED;
	m_liftermin = LIF_LIFTERMIN;
	m_liftermax = LIF_LIFTERMAX;
	m_lifterminspd = LIF_LIFTERMINSPD;
	m_liftermaxspd = LIF_LIFTERMAXSPD;
	m_loopmode = kManual;
	m_movingdir = kNone;

	m_lowposition = 0;
	m_mediumposition = 0;
	m_highposition = 0;
	m_selectedposition = 0;

	m_prevhascargo = false;
	m_movebottom = false;
	m_movesmidge = false;
	m_staging = false;

	if (CAN_LIFTER_MOTOR1 != -1)
	{
		m_motor = new WPI_TalonSRX(CAN_LIFTER_MOTOR1);
		m_motor->Set(ControlMode::PercentOutput, 0);
		m_motor->SetNeutralMode(NeutralMode::Brake);
		m_motor->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);

		if (CAN_LIFTER_MOTOR2 != -1)
		{
			m_motorslave = new WPI_TalonSRX(CAN_LIFTER_MOTOR2);
			m_motorslave->Set(ControlMode::Follower, CAN_LIFTER_MOTOR1);
			m_motorslave->SetNeutralMode(NeutralMode::Brake);
			m_motorslave->SetInverted(InvertType::OpposeMaster);
		}
	}

	if (PCM_LIFTER_SOLENOID != -1)
		m_solenoid = new Solenoid(PCM_LIFTER_SOLENOID);
}


Lifter::~Lifter()
{
	if (m_motor != nullptr)
		delete m_motor;
	if (m_motorslave != nullptr)
		delete m_motorslave;
	if (m_solenoid != nullptr)
		delete m_solenoid;
}


void Lifter::Init()
{
	if ((m_motor == nullptr) || (m_solenoid == nullptr))
		return;

	if (Debug) DriverStation::ReportError("LifterInit");

	// do initialization for auto mode
	if (m_ds->IsAutonomous())
	{
		m_motor->SetSelectedSensorPosition(LIF_LIFTERSTART, 0, 0);
		m_solenoid->Set(false);
	}
	else
		m_solenoid->Set(true);

	// do initialization for any mode
	m_position = 0;
	m_lowposition = 0;
	m_mediumposition = 0;
	m_highposition = 0;
	m_selectedposition = 0;

	m_prevhascargo = false;
	m_movebottom = false;
	m_movesmidge = false;
	m_staging = false;

	m_motor->StopMotor();
	m_loopmode = kManual;
	m_movingdir = kNone;
}


void Lifter::Loop()
{
	if ((m_motor == nullptr) || (m_solenoid == nullptr))
		return;

	m_position = m_motor->GetSelectedSensorPosition(0);

    if (m_inputs->xBoxLeftTrigger(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_intake->SetIntakeMode(Intake::kModeHatch);
        SetHatchLevels();
    }
    else
    if (m_inputs->xBoxRightTrigger(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_intake->SetIntakeMode(Intake::kModeCargo);
        SetCargoLevels();
    }

    // if lifter is near bottom, force the cargo intake down
    if (NearBottom())
        m_intake->SetCargoIntake(Intake::kCargoDown);
	else
    // if lifter is staging and above bottom, force the cargo intake up
    if (m_staging)
        m_intake->SetCargoIntake(Intake::kCargoUp);

	// if cargo just ejected, enable move all the way down
	CheckCargoEjected();

	// if cargo just ingested, move lifter up a smidge and raise cargo intake
	CheckMoveSmidge();

	switch (m_loopmode)
	{
	case kManual:
		m_raisespeed = LIF_RAISESPEED;
		m_lowerspeed = LIF_LOWERSPEED;
		m_staging = false;

		/// check for staging of lifter (pre game)
		if (m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) &&
			m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) &&
			m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
		{
			m_selectedposition = LIF_LIFTERSTART;
			m_raisespeed = LIF_RAISESPEED * 0.5;
			m_loopmode = kAutoUp;
			m_staging = true;
		}
		else
		/// if Y is pressed, initiate up auto sequence
		if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL) && m_highposition)
		{
			m_selectedposition = FindPosition(kUp);
			m_loopmode = kAutoUp;
		}
		else
		/// if X is pressed, initiate down auto sequence
		if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL) && m_highposition)
		{
			m_selectedposition = FindPosition(kDown);
			m_loopmode = kAutoDown;
		}
		else
		/// if left bumper and up override position sensor and raise lift
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) < -LIF_DEADZONE_Y) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
		{
			m_motor->Set(m_raisespeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			m_movingdir = kUp;
		}
		else
		/// if up raise lift only if not at max position
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) < -LIF_DEADZONE_Y) && (m_position < m_liftermax))
		{
			if (m_position > m_liftermaxspd)
				m_motor->Set(m_raisespeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			else
				m_motor->Set(m_raisespeed * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			m_movingdir = kUp;
		}
		else
		/// if left bumper and down override position sensor and lower lift
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) > LIF_DEADZONE_Y) && m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
		{
			m_motor->Set(m_lowerspeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			ResetPosition();
			m_movingdir = kDown;
		}
		else
		/// if down lower lift only if not at min position
		if ((m_inputs->xBoxRightY(1 * INP_DUAL) > LIF_DEADZONE_Y) && (m_position > m_liftermin))
		{
			if (m_position < m_lifterminspd)
				m_motor->Set(m_lowerspeed * 0.5 * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			else
				m_motor->Set(m_lowerspeed * fabs(m_inputs->xBoxRightY(1 * INP_DUAL)));
			m_movingdir = kDown;
		}
		else
		/// no lifter movement hold lifter hight if not at bottom
		{
			if (m_position > m_liftermin)
				m_motor->Set(LIF_HOLDSPEED);
			else
				m_motor->StopMotor();
			m_movingdir = kNone;
		}
		break;

	case kAutoUp:
		/// if position is not at goal, keep raising
		if (m_position < m_selectedposition)
		{
			if (m_position > m_liftermaxspd)
				m_motor->Set(m_raisespeed * 0.5);
			else
				m_motor->Set(m_raisespeed);
			m_movingdir = kUp;
		}
		else
		/// if position is greater than goal, stop and return to manual control
		if (m_position > (m_selectedposition - (m_selectedposition == m_highposition ? 0 : LIF_SLACK)))
		{
			m_motor->StopMotor();
			m_loopmode = kManual;
			m_movingdir = kNone;
		}

		/// if manual control is sensed, return to manual control
		if (fabs(m_inputs->xBoxRightY(1 * INP_DUAL)) > LIF_DEADZONE_Y)
		{
			m_loopmode = kManual;
		}
		else
		/// if Y is pressed, increase target position
		if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
		{
			UpdatePosition(kUp);
		}
		else
		/// if X is pressed, decrease target position
		if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
		{
			UpdatePosition(kDown);
		}
		break;

	case kAutoDown:
		/// if position is not at goal, keep lowering
		if (m_position > m_selectedposition)
		{
			if (m_position < m_lifterminspd)
				m_motor->Set(m_lowerspeed * 0.5);
			else
				m_motor->Set(m_lowerspeed);
			m_movingdir = kDown;
		}
		else 
		/// if position is less than goal, stop and return to manual control
		if (m_position < (m_selectedposition + (m_selectedposition == m_liftermin ? 0 : LIF_SLACK)))
		{
			m_motor->StopMotor();
			m_loopmode = kManual;
			m_movingdir = kNone;
		}

		/// if manual control is sensed, return to manual control
		if (fabs(m_inputs->xBoxRightY(1 * INP_DUAL)) > LIF_DEADZONE_Y)
		{
			m_loopmode = kManual;
		}
		else
		/// if Y is pressed, increase target position
		if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
		{
			UpdatePosition(kUp);
		}
		else
		/// if X is pressed, decrease target position
		if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
		{
			UpdatePosition(kDown);
		}
		break;
	}

	// straighten lifter forward - deploy - true
	if (m_inputs->xBoxDPadRight(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
		m_solenoid->Set(true);
	else
	// angle lifter back - retract - false (default)
	if (m_inputs->xBoxDPadLeft(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
		m_solenoid->Set(false);

	SmartDashboard::PutNumber("LI1_liftermin", m_liftermin);
	SmartDashboard::PutNumber("LI2_liftermax", m_liftermax);
	SmartDashboard::PutNumber("LI3_position", m_position);
	SmartDashboard::PutNumber("LI4_mode", m_loopmode);
	SmartDashboard::PutNumber("LI5_selectedposition", m_selectedposition);
}


void Lifter::Stop()
{
	if ((m_motor == nullptr) || (m_solenoid == nullptr))
		return;

	m_motor->StopMotor();
	m_solenoid->Set(false);
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


int Lifter::FindPosition(LifterDir direction)
{
	if (m_motor == nullptr)
		return m_position;

	m_position = m_motor->GetSelectedSensorPosition();

	if (direction == kUp)
	{
		if ((m_position < (m_lowposition - LIF_SLACK)) && (m_lowposition != 0))
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
	}
	else
	if (direction == kDown) 
	{
		if (m_movebottom)
		{
			if (Debug) DriverStation::ReportError("Down Bottom");
			m_movebottom = false;
			return m_liftermin;
		}
		else
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
		if ((m_position > (m_lowposition + LIF_SLACK)) && (m_lowposition != 0))
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


void Lifter::UpdatePosition(LifterDir direction)
{
	if (direction == kUp)
	{
		if (m_selectedposition == m_liftermin)
			m_selectedposition = m_lowposition;
		else
		if (m_selectedposition == m_lowposition)
			m_selectedposition = m_mediumposition;
		else
		if (m_selectedposition == m_mediumposition)
			m_selectedposition = m_highposition;
	}
	else
	if (direction == kDown)
	{
		if (m_selectedposition == m_highposition)
			m_selectedposition = m_mediumposition;
		else
		if (m_selectedposition == m_mediumposition)
			m_selectedposition = m_lowposition;
		else
		if (m_selectedposition == m_lowposition)
			m_selectedposition = m_liftermin;
	}

	if (m_selectedposition == m_highposition)
		m_loopmode = kAutoUp;
	else
	if (m_selectedposition == m_liftermin)
		m_loopmode = kAutoDown;
	else
	if (m_position > (m_selectedposition + LIF_SLACK))
		m_loopmode = kAutoDown;
	else
	if (m_position < (m_selectedposition - LIF_SLACK))
		m_loopmode = kAutoUp;
	else
		m_loopmode = kManual;
}


// controls the ability to move all the way down based on whether ball has just been intaked/ejected
void Lifter::CheckCargoEjected()
{
	bool newhascargo = m_intake->HasCargo();

	if (m_prevhascargo && !newhascargo)		// if just ejected, allow move all the way down
	{
		m_movebottom = true;
	}
	else
	if (!m_prevhascargo && newhascargo)		// if just intaked, prevent move all the way down
	{
		m_movebottom = false;
		m_movesmidge = true;
	}
	m_prevhascargo = newhascargo;
}


void Lifter::CheckMoveSmidge()
{
	if (m_movesmidge)
	{
		m_selectedposition = LIF_LIFTERSMIDGELOW + LIF_SLACK;
		m_loopmode = kAutoUp;
	}
	else
	if (m_loopmode == kManual)
	{
		m_intake->SetCargoIntake(Intake::kCargoUp);
		m_movesmidge = false;
	}
}


bool Lifter::NearBottom()
{
	if (m_movingdir == kNone)
	{
		return (m_position < LIF_LIFTERSMIDGELOW);
	}
	else
	if (m_movingdir == kUp)
	{
		return (m_position < LIF_LIFTERSMIDGELOW);
	}
	else
	if (m_movingdir == kDown)
	{
		return (m_position < LIF_LIFTERSMIDGEHIGH);
	}
}
