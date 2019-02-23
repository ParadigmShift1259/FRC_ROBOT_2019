/*
 * Intake.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: Jival
 */

#include <Intake.h>
#include "Const.h"


Intake::Intake(DriverStation *ds, OperatorInputs *inputs, Lifter *lifter, DrivePID *drivepid)
{
	m_ds = ds;
	m_inputs = inputs;
	m_lifter = lifter;

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
    if (m_solenoidvac1 == nullptr || m_solenoidvac2 == nullptr || m_solenoidvac3 == nullptr || m_solenoidvac4 == nullptr || m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidcargo == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;
	
	if (Debug) DriverStation::ReportError("Intake Init");

    m_inited = true;

	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
	m_solenoidvac3->Set(false);
	m_solenoidvac4->Set(false);
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

    if (m_inputs->xBoxLeftTrigger(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_mode = kModeHatch;
        m_hatchstage = kHatchIdle;
        m_lifter->SetHatchLevels();
    }
    else
    if (m_inputs->xBoxRightTrigger(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_mode = kModeCargo;
        m_cargostage = kCargoIdle;
        m_lifter->SetCargoLevels();
    }
    
    if (m_lifter->NearBottom())             // if lifter is near bottom, lower the cargo intake
        SetCargoIntake(kCargoDown);
    else
    if (m_lifter->IsStaging())              // if lifter is staging and above bottom, raise the cargo intake
        SetCargoIntake(kCargoUp);

    Cargo();
    Hatch();
    Flush();
    
    SmartDashboard::PutNumber("IN0_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN1_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN2_solenoidvac3", m_solenoidvac3->Get());
	SmartDashboard::PutNumber("IN3_solenoidvac4", m_solenoidvac4->Get());
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
	m_solenoidvac2->Set(false);
	m_solenoidvac3->Set(false);
	m_solenoidvac4->Set(false);
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

        SetHatchVac(kPoofOff);

        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOn);
            m_timer.Reset();
            m_hatchstage = kHatchRelease;
        }
        else
        {
            SetHatchVac(kVacOn);
        }
        break;

    case kHatchRelease:
    	if (Debug) DriverStation::ReportError("kHatchRelease");

        SetHatchVac(kVacOff);
        SetHatchVac(kPoofOn);

        if (m_timer.Get() > m_waittime)
        {
            SetHatchVac(kVacOff);
            SetHatchVac(kPoofOff);

            m_hatchstage = kHatchIdle;
                                                // back up while poofing?
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

		SetCargoIntake(kCargoIn);

        if ((m_cargosensor && m_cargosensor->Get()) || m_inputs->xBoxBackButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
			m_timer.Reset();            
            m_cargostage = kCargoIngestWait;
        }
        break;

    case kCargoIngestWait:
    	if (Debug) DriverStation::ReportError("kCargoIngestWait");

        if (m_timer.Get() > INT_CARGO_INGEST_WAIT)
        {
            SetCargoIntake(kCargoOff);
            m_lifter->MoveSmidgeUp();
            m_cargostage = kCargoBallSmidge;
        }
        else
        {
            SetCargoIntake(kCargoIn);
        }
        break;
    
    case kCargoBallSmidge:
        if (m_lifter->MoveSmidgeUp())
        {
            SetCargoIntake(kCargoUp);
            m_cargostage = kCargoBall;
        }
        break;

    case kCargoBall:
    	if (Debug) DriverStation::ReportError("kCargoBall");

        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoIn);
        }
        else
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            SetCargoIntake(kCargoOut);
            m_lifter->CargoEjected();
            m_timer.Reset();
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

    if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL) && !m_lifter->NearBottom())
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
        m_sparkcargo->Set(0);
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
            m_sparkcargo->Set(INT_CARGO_EJECT_SPEED);

            m_timer.Reset();
            m_flushstage = kFlushEject;
        }
        break;

    case kFlushEject:
        if (m_timer.Get() > INT_CARGO_EJECT_WAIT)
        {
            m_sparkcargo->Set(0);
            m_flushstage = kFlushNone;
        }
        break;
    }
}


void Intake::SetCargoIntake(CargoDir cargodir)
{
    if (cargodir == kCargoUp)
        m_solenoidcargo->Set(false);
    else
    if (cargodir == kCargoDown)
        m_solenoidcargo->Set(true);
    else
    if (cargodir == kCargoOff)
        m_sparkcargo->Set(0);
    else
    if (cargodir == kCargoIn)
        m_sparkcargo->Set(INT_CARGO_INGEST_SPEED);
    else
    if (cargodir == kCargoOut)
        m_sparkcargo->Set(INT_CARGO_EJECT_SPEED);
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
        m_solenoidvac2->Set(true);
        m_solenoidvac3->Set(true);
        m_solenoidvac4->Set(true);
    }
    else
    if (hatchvac == kPoofOff)
    {
        m_solenoidvac1->Set(false);
        m_solenoidvac2->Set(false);
        m_solenoidvac3->Set(false);
        m_solenoidvac4->Set(false);
    }
}