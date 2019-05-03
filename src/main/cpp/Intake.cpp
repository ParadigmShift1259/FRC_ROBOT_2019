/*
 * Intake.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: Jival
 */

#include <Intake.h>
#include "Const.h"


Intake::Intake(DriverStation *ds, OperatorInputs *inputs)
{
	m_ds = ds;
	m_inputs = inputs;

	m_solenoidvac1 = nullptr;
	m_solenoidvac2 = nullptr;
	m_solenoidvac3 = nullptr;
	m_solenoidvac4 = nullptr;
    m_sparkvac = nullptr;

	m_solenoidhatch = nullptr;

	m_solenoidcargo = nullptr;

    m_sparkcargo = nullptr;
    m_cargosensor = nullptr;

	if (PCM_INTAKE_SOLENOIDVAC1 != -1)
		m_solenoidvac1 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDVAC1);
	if (PCM_INTAKE_SOLENOIDVAC2 != -1)
		m_solenoidvac2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDVAC2);
	if (PCM_INTAKE_SOLENOIDVAC3 != -1)
		m_solenoidvac3 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDVAC3);
	if (PCM_INTAKE_SOLENOIDVAC4 != -1)
		m_solenoidvac4 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDVAC4);
    if (PWM_INTAKE_SPARKVAC != -1)
        m_sparkvac = new Spark(PWM_INTAKE_SPARKVAC);
    
	if (PCM_INTAKE_SOLENOIDHATCH != -1)
		m_solenoidhatch = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDHATCH);

    if (PCM_INTAKE_SOLENOIDCARGO != -1)
		m_solenoidcargo = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDCARGO);
	
	if (PWM_INTAKE_SPARKCARGO != -1)
		m_sparkcargo = new Spark(PWM_INTAKE_SPARKCARGO);
	if (DIO_INTAKE_CARGOSENSOR != -1)
	    m_cargosensor = new DigitalInput(DIO_INTAKE_CARGOSENSOR);
    
	m_mode = kModeNone;
    m_hatchstage = kHatchIdle;
    m_cargostage = kCargoIdle;
    m_flushstage - kFlushNone;
	m_waittime = INT_VACUUM_WAIT;
    m_vacuumpow = INT_VACUUM_POW;
    m_inited = false;
    m_hascargohatch = false;
    m_atbottom = false;
}


Intake::~Intake()
{
	if (m_solenoidvac1 != nullptr)
        delete m_solenoidvac1;
    if (m_solenoidvac2 != nullptr)
        delete m_solenoidvac2;
	if (m_solenoidvac3 != nullptr)
        delete m_solenoidvac3;
    if (m_solenoidvac4 != nullptr)
        delete m_solenoidvac4;
    if (m_sparkvac != nullptr)
        delete m_sparkvac;

	if (m_solenoidhatch != nullptr)
        delete m_solenoidhatch;

	if (m_solenoidcargo != nullptr)
        delete m_solenoidcargo;

    if (m_sparkcargo != nullptr)
        delete m_sparkcargo;
}


void Intake::Init()
{
    if (m_solenoidvac1 == nullptr)
        return;
    if (m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidcargo == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;
	
	if (Debug) DriverStation::ReportError("Intake Init");

    m_inited = true;
    m_hascargohatch = false;
    m_atbottom = false;

	m_solenoidvac1->Set(false);
	if (m_solenoidvac2) m_solenoidvac2->Set(false);
	if (m_solenoidvac3) m_solenoidvac3->Set(false);
	if (m_solenoidvac4) m_solenoidvac4->Set(false);
    m_sparkvac->Set(0);

	m_solenoidhatch->Set(false);

	m_solenoidcargo->Set(false);

	m_sparkcargo->Set(0);

    m_mode = kModeNone;
    m_cargostage = kCargoIdle;
    m_hatchstage = kHatchIdle;
    m_flushstage = kFlushNone;
    m_timer.Reset();
    m_timer.Start();
}


void Intake::Loop()
{
    if (!m_inited)
        return;

    Cargo();
    Hatch();
    Flush();
    
    SmartDashboard::PutNumber("IN0_solenoidvac1", m_solenoidvac1->Get());
	if (m_solenoidvac2) SmartDashboard::PutNumber("IN1_solenoidvac2", m_solenoidvac2->Get());
	if (m_solenoidvac3) SmartDashboard::PutNumber("IN2_solenoidvac3", m_solenoidvac3->Get());
	if (m_solenoidvac4) SmartDashboard::PutNumber("IN3_solenoidvac4", m_solenoidvac4->Get());
    SmartDashboard::PutNumber("IN4_sparkvac", m_sparkvac->Get());

	SmartDashboard::PutNumber("IN5_solenoidhatch", m_solenoidhatch->Get());

	SmartDashboard::PutNumber("IN6_solenoidarm", m_solenoidcargo->Get());

	SmartDashboard::PutNumber("IN7_mode", m_mode);
    SmartDashboard::PutNumber("IN8_hatchmode", m_hatchstage);
    SmartDashboard::PutNumber("IN9_cargomode", m_cargostage);
	SmartDashboard::PutNumber("IN10_waittime", m_waittime);

	if (m_cargosensor)
        SmartDashboard::PutNumber("IN11_cargosensor", m_cargosensor->Get());
}


void Intake::Stop()
{
    if (!m_inited)
        return;

	m_solenoidvac1->Set(false);
	if (m_solenoidvac2) m_solenoidvac2->Set(false);
	if (m_solenoidvac3) m_solenoidvac3->Set(false);
	if (m_solenoidvac4) m_solenoidvac4->Set(false);
    m_sparkvac->Set(0);

	m_solenoidhatch->Set(false);

	m_solenoidcargo->Set(false);

	m_sparkcargo->Set(0);

	m_timer.Stop();
}


void Intake::Hatch()
{
    if (!m_inited)
        return;

    switch (m_mode)
    {
    case kModeNone:                     // no mode selected or flush mode
        return;
        break;

    case kModeHatch:                    // hatch mode
        break;

    case kModeCargo:                    // cargo mode - turn off hatch mechanisms
        SetHatchVac(kVacOff);
        SetHatchVac(kPoofOff);
        SetHatchIntake(kHatchUp);
        return;
        break;
    }

    switch (m_hatchstage)
    {
    case kHatchIdle:
    	if (Debug) DriverStation::ReportError("kHatchIdle");

        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetHatchVac(kVacOn);
            SetHatchVac(kPoofOff);
            m_hascargohatch = true;
            m_hatchstage = kHatchCapture;
        }
        else
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOn);
        }
        else
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOff);
        }
        break;

    case kHatchCapture:
    	if (Debug) DriverStation::ReportError("kHatchCapture");

        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOn);
            m_timer.Reset();
            m_hascargohatch = false;
            m_hatchstage = kHatchRelease;
        }
        else
        {
            SetHatchVac(kVacOn);
            SetHatchVac(kPoofOff);
        }
        break;

    case kHatchRelease:
    	if (Debug) DriverStation::ReportError("kHatchRelease");

        if (m_timer.Get() > m_waittime)
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOff);
            m_hatchstage = kHatchIdle;
                                                // back up while poofing?
        }
        else
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOn);
        }
        break;
    }

    if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        SetHatchIntake(kHatchUp);
    }
    else
    if (m_inputs->xBoxDPadDown(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        SetHatchIntake(kHatchDown);
    }
    else
    if (m_inputs->xBoxStartButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_hatchstage = kHatchIdle;
        m_mode = kModeNone;
        m_flushstage = kFlushStart;
    }
}


void Intake::Cargo()
{
    if (!m_inited)
        return;

    switch (m_mode)
    {
    case kModeNone:                     // no mode selected or flush mode
        return;
        break;

    case kModeHatch:                    // hatch mode - turn off cargo mechanisms
        SetCargoIntake(kCargoOff);
        SetCargoIntake(kCargoDown);
        return;
        break;

    case kModeCargo:                    // cargo mode
        break;
    }

    switch (m_cargostage)
    {
    case kCargoIdle:
    	if (Debug) DriverStation::ReportError("kCargoIdle");

        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoIn);
            m_cargostage = kCargoIngest;
        }
        else
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoOut);
            m_timer.Reset();
            m_cargostage = kCargoEject;
        }
        else
        {
            SetCargoIntake(kCargoOff);
        }
        break;

    case kCargoIngest:
    	if (Debug) DriverStation::ReportError("kCargoIngest");

        if (((m_cargosensor && m_cargosensor->Get()) || m_inputs->xBoxBackButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        && !m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoIn);
			m_timer.Reset();            
            m_cargostage = kCargoIngestWait;
        }
        else
        {
		    SetCargoIntake(kCargoIn);
        }
        break;

    case kCargoIngestWait:
    	if (Debug) DriverStation::ReportError("kCargoIngestWait");

        if (m_timer.Get() > INT_CARGO_INGEST_WAIT)
        {
            SetCargoIntake(kCargoOff);
            m_hascargohatch = true;
            m_cargostage = kCargoBall;
        }
        else
        {
            SetCargoIntake(kCargoIn);
        }
        break;

    case kCargoBall:
    	if (Debug) DriverStation::ReportError("kCargoBall");

        if (m_inputs->xBoxLeftBumper(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL) && m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoSlowOut);
        }
        else
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoIn);
        }
        else
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoOut);
            m_timer.Reset();
            m_hascargohatch = false;
            m_cargostage = kCargoEject;
        }
        else
        {
            SetCargoIntake(kCargoOff);
        }
        break;
    
    case kCargoEject:
        if (m_timer.Get() > INT_CARGO_EJECT_WAIT)
        {
            SetCargoIntake(kCargoOff);
            m_cargostage = kCargoIdle;
        }
        else
        {
            SetCargoIntake(kCargoOut);
        }
        break;
    }

    if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        SetCargoIntake(kCargoUp);
    }
    else
    if (m_inputs->xBoxDPadDown(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        SetCargoIntake(kCargoDown);
    }
    else
    if (m_inputs->xBoxStartButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_cargostage = kCargoIdle;
        m_mode = kModeNone;
        m_flushstage = kFlushStart;
    }
}


void Intake::Flush()
{
    if (!m_inited)
        return;

    if (m_mode != kModeNone)
        return;

    switch (m_flushstage)
    {
    case kFlushNone:
        if (m_inputs->xBoxStartButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
            m_flushstage = kFlushStart;
        break;
    
    case kFlushStart:
        SetCargoIntake(kCargoOff);
        SetCargoIntake(kCargoDown);
        SetHatchIntake(kHatchUp);
        SetHatchVac(kVacOff);
        SetHatchVac(kPoofOn);

        m_timer.Reset();
        m_flushstage = kFlushPoof;
        break;
    
    case kFlushPoof:

        if (m_timer.Get() > m_waittime)
        {
            SetHatchVac(kPoofOff);
            SetCargoIntake(kCargoOut);

            m_timer.Reset();
            m_hascargohatch = false;
            m_flushstage = kFlushEject;
        }
        break;

    case kFlushEject:
        if (m_timer.Get() > INT_CARGO_EJECT_WAIT)
        {
            SetCargoIntake(kCargoOff);
            m_hascargohatch = false;
            m_flushstage = kFlushNone;
        }
        break;
    }
}


void Intake::SetIntakeMode(IntakeMode intakemode)
{
    if (intakemode == kModeHatch)
    {
        m_mode = kModeHatch;
        m_hatchstage = kHatchIdle;
    }
    else
    if (intakemode == kModeCargo)
    {
        m_mode = kModeCargo;
        m_cargostage = kCargoIdle;
    }
}


void Intake::SetCargoIntake(CargoDir cargodir)
{
    if (cargodir == kCargoUp && !m_atbottom)
    {
        // if (Debug) DriverStation::ReportError("kCargoUp");
        m_solenoidcargo->Set(false);
    }
    else
    if (cargodir == kCargoDown)
    {
        // if (Debug) DriverStation::ReportError("kCargoDown");
        m_solenoidcargo->Set(true);
    }
    else
    if (cargodir == kCargoOff)
    {
        // if (Debug) DriverStation::ReportError("kCargoOff");
        m_sparkcargo->Set(0);
    }
    else
    if (cargodir == kCargoIn)
    {
        // if (Debug) DriverStation::ReportError("kCargoIn");
        m_sparkcargo->Set(INT_CARGO_INGEST_SPEED);
    }
    else
    if (cargodir == kCargoOut)
    {
        // if (Debug) DriverStation::ReportError("kCargoOut");
        m_sparkcargo->Set(INT_CARGO_EJECT_SPEED);
    }
    else
    if (cargodir == kCargoSlowOut)
    {
        m_sparkcargo->Set(INT_CARGO_SLOW_EJECT_SPEED);
    }
}


Intake::CargoDir Intake::GetCargoIntake()
{
    if (m_solenoidcargo->Get())
        return kCargoDown;
    else
        return kCargoUp;
}


void Intake::SetHatchIntake(HatchDir hatchdir)
{
    if (hatchdir == kHatchUp)
        m_solenoidhatch->Set(false);
    else
    if (hatchdir == kHatchDown)
        m_solenoidhatch->Set(true);
}


Intake::HatchDir Intake::GetHatchIntake()
{
    if (m_solenoidhatch->Get())
        return kHatchDown;
    else
        return kHatchUp;
}


void Intake::SetHatchVac(HatchVac hatchvac)
{
    if (hatchvac == kVacOn)
    {
        m_sparkvac->Set(m_vacuumpow);
    }
    else
    if (hatchvac == kVacOff)
    {
        m_sparkvac->Set(0);
    }
    else
    if (hatchvac == kPoofOn)
    {
        m_solenoidvac1->Set(true);
        if (m_solenoidvac2) m_solenoidvac2->Set(true);
        if (m_solenoidvac3) m_solenoidvac3->Set(true);
        if (m_solenoidvac4) m_solenoidvac4->Set(true);
    }
    else
    if (hatchvac == kPoofOff)
    {
        m_solenoidvac1->Set(false);
        if (m_solenoidvac2) m_solenoidvac2->Set(false);
        if (m_solenoidvac3) m_solenoidvac3->Set(false);
        if (m_solenoidvac4) m_solenoidvac4->Set(false);
    }
}
