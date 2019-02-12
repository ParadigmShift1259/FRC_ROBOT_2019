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

	m_solenoidarm1 = nullptr;
	m_solenoidarm2 = nullptr;
<<<<<<< HEAD
    m_spark1 = nullptr;
    m_spark2 = nullptr;
	m_ds = ds;
	m_inputs = inputs;
	m_lifter = lifter;
	m_waittime = PCM_PNEUMATIC_WAIT;
    m_drivepid = drivepid;
    m_counter = 0;
    m_visionvalid = false;
    m_nettable = NetworkTableInstance::GetDefault().GetTable("OpenCV");
=======
>>>>>>> 9094d925fae5f9fc350ed92280ca6c5a2b5cff44

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
    if (SPARK_VAC != -1)
        m_sparkvac = new Spark(SPARK_VAC);
    
	if (PCM_INTAKE_SOLENOIDHATCH != -1)
		m_solenoidhatch = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDHATCH);

    if (PCM_INTAKE_SOLENOIDARM1 != -1)
		m_solenoidarm1 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDARM1);
	if (PCM_INTAKE_SOLENOIDARM2 != -1)
		m_solenoidarm2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDARM2);
	
	if (SPARK_CARGO != -1)
		m_sparkcargo = new Spark(SPARK_CARGO);
	if (DIO_INTAKE_CARGOSENSOR != -1)
	    m_cargosensor = new DigitalInput(DIO_INTAKE_CARGOSENSOR);
    
	m_mode = kModeAny;
    m_hatchstage = kHatchIdle;
    m_cargostage = kCargoIdle;
	m_waittime = PCM_PNEUMATIC_WAIT;
    m_vacuumpow = PCM_PNEUMATIC_POW;
    m_onfloor = false;
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

	if (m_solenoidarm1 != nullptr)
        delete m_solenoidarm1;
    if (m_solenoidarm2 != nullptr)
        delete m_solenoidarm2;

    if (m_sparkcargo != nullptr)
        delete m_sparkcargo;
}



void Intake::Init()
{
    //DriverStation::ReportError("Intake Init");
    if (m_solenoidvac1 == nullptr || m_solenoidvac2 == nullptr || m_solenoidvac3 == nullptr || m_solenoidvac4 == nullptr || m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidarm1 == nullptr || m_solenoidarm2 == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;
	
	DriverStation::ReportError("IntakeInit");

	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
<<<<<<< HEAD
	m_solenoidhatch1->Set(false);
	m_solenoidhatch2->Set(false);
	m_solenoidarm1->Set(false);
	m_solenoidarm2->Set(false);
    m_spark1->Set(0);
    m_spark2->Set(0);
    m_visiontimer.Reset();
    m_visiontimer.Start();
    m_visionvalid = false;
=======
	m_solenoidvac3->Set(false);
	m_solenoidvac4->Set(false);
    m_sparkvac->Set(0);

	m_solenoidhatch->Set(false);

	//m_solenoidarm1->Set(false);
	//m_solenoidarm2->Set(false);

	m_sparkcargo->Set(0);

    m_mode = kModeAny;
    m_cargostage = kCargoIdle;
    m_hatchstage = kHatchIdle;
    m_timer.Reset();
    m_timer.Start();
>>>>>>> 9094d925fae5f9fc350ed92280ca6c5a2b5cff44
}


void Intake::HatchLoop()
{
    if (m_solenoidvac1 == nullptr || m_solenoidvac2 == nullptr || m_solenoidvac3 == nullptr || m_solenoidvac4 == nullptr || m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidarm1 == nullptr || m_solenoidarm2 == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;
    
    if (m_mode == kModeCargo)
    {
        m_sparkvac->Set(0);
        return;
    }

    switch (m_hatchstage)
    {
    case kHatchIdle:
    	DriverStation::ReportError("kHatchIdle");

        m_solenoidvac1->Set(false);
        m_solenoidvac2->Set(false);
        m_solenoidvac3->Set(false);
        m_solenoidvac4->Set(false);
        m_sparkvac->Set(0);

        m_solenoidhatch->Set(false);

        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            m_hatchstage = kHatchCapture;
            m_mode = kModeHatch;
        }
        break;

    case kHatchCapture:
    	DriverStation::ReportError("kHatchCapture");

        m_solenoidvac1->Set(false);
        m_solenoidvac2->Set(false);
        m_solenoidvac3->Set(false);
        m_solenoidvac4->Set(false);
        m_sparkvac->Set(0.550);

        if (m_inputs->xBoxBackButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
            m_onfloor = !m_onfloor;

        if (m_onfloor)
        {
            m_solenoidhatch->Set(true);
        }
        else
        {
            m_solenoidhatch->Set(false);
        }

        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            m_solenoidvac1->Set(true);
            m_solenoidvac2->Set(true);
            m_solenoidvac3->Set(true);
            m_solenoidvac4->Set(true);
            m_timer.Reset();
            m_hatchstage = kHatchRelease;
        }
        break;

    case kHatchRelease:
    	DriverStation::ReportError("kHatchRelease");

        m_solenoidhatch->Set(false);
        m_sparkvac->Set(0);

        if (m_timer.Get() > m_waittime)
        {
            m_solenoidvac1->Set(false);
            m_solenoidvac2->Set(false);
            m_solenoidvac3->Set(false);
            m_solenoidvac4->Set(false);
            m_sparkvac->Set(0);

            m_solenoidhatch->Set(false);

            m_onfloor = false;
            m_hatchstage = kHatchIdle;
            m_mode = kModeAny;
        }
        break;
    }

    if (m_inputs->xBoxDPadRight(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
    {
        m_waittime += 0.05;
    }
    else
    if (m_inputs->xBoxDPadLeft(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL) && (m_waittime > 0.05))
    {
        m_waittime -= 0.05;
    }

    if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
    {
        m_vacuumpow += 0.025;
    }
    else
    if (m_inputs->xBoxDPadDown(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL) && (m_vacuumpow > 0.025))
    {
        m_vacuumpow -= 0.025;
    }

    if (m_inputs->xBoxStartButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_solenoidvac1->Set(false);
        m_solenoidvac2->Set(false);
        m_solenoidvac3->Set(false);
        m_solenoidvac4->Set(false);
        m_sparkvac->Set(0);

        m_solenoidhatch->Set(false);
        
        m_hatchstage = kHatchIdle;
        m_mode = kModeAny;
    }

	SmartDashboard::PutNumber("IN0_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN1_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN2_solenoidvac3", m_solenoidvac3->Get());
	SmartDashboard::PutNumber("IN3_solenoidvac4", m_solenoidvac4->Get());
    SmartDashboard::PutNumber("IN4_sparkvac", m_sparkvac->Get());

	SmartDashboard::PutNumber("IN5_solenoidhatch", m_solenoidhatch->Get());

	//SmartDashboard::PutNumber("IN6_solenoidarm1", m_solenoidarm1->Get());
	//SmartDashboard::PutNumber("IN7_solenoidarm2", m_solenoidarm2->Get());

	SmartDashboard::PutNumber("IN8_mode", m_mode);
    SmartDashboard::PutNumber("IN9_hatchmode", m_hatchstage);
    SmartDashboard::PutNumber("IN10_cargomode", m_cargostage);
	SmartDashboard::PutNumber("IN11_waittime", m_waittime);
	SmartDashboard::PutNumber("IN12_cargosensor", m_cargosensor->Get());
}


void Intake::CargoLoop()
{
    if (m_solenoidvac1 == nullptr || m_solenoidvac2 == nullptr || m_solenoidvac3 == nullptr || m_solenoidvac4 == nullptr || m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidarm1 == nullptr || m_solenoidarm2 == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;

    if (m_mode == kModeHatch)
    {
        m_sparkcargo->Set(0);
        return;
    }

    switch (m_cargostage)
    {
    case kCargoIdle:
    	DriverStation::ReportError("kCargoIdle");

        m_sparkcargo->Set(0);

        //m_solenoidarm1->Set(false);
        //m_solenoidarm2->Set(false);

        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            m_cargostage = kCargoIngest;
        }
        break;

    case kCargoIngest:
    	DriverStation::ReportError("kCargoIngest");
        m_mode = kModeCargo;

		m_sparkcargo->Set(0.7);

        //m_solenoidarm1->Set(false);
        //m_solenoidarm2->Set(false);

        if (/*m_cargosensor->Get() or manual check?*/ m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
			m_timer.Reset();            
            m_cargostage = kCargoBall;
        }
        break;

    case kCargoIngestWait:
    	DriverStation::ReportError("kCargoIngestWait");

        if (m_timer.Get() > 1.0)        // Magic # used for ingest
        {
            m_sparkcargo->Set(0);
            m_cargostage = kCargoBall;
            m_timer.Reset();
        }
        else
        {
            m_sparkcargo->Set(0.7);
        }
        break;
    
    case kCargoBall:
    	DriverStation::ReportError("kCargoBall");

        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kHold, 1 * INP_DUAL))
        {
            m_sparkcargo->Set(0.7);
        }
        else
        {
            m_sparkcargo->Set(0);
        }

        if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            m_sparkcargo->Set(-1.0);
            m_timer.Reset();
            m_cargostage = kCargoEject;
        }
        if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
        {
            //m_solenoidarm1->Set(true);
            //m_solenoidarm2->Set(true);
        }
        break;
    
    case kCargoEject:
        if (m_timer.Get() > 1.5)    // Magic # for eject
        {
            m_sparkcargo->Set(0);
            //m_solenoidarm1->Set(false);
            //m_solenoidarm2->Set(false);
            m_cargostage = kCargoIdle;
            m_mode = kModeAny;
        }
        else
        {
            m_sparkcargo->Set(-1.0);
        }
        break;

    };

    if (m_inputs->xBoxStartButton(OperatorInputs::ToggleChoice::kToggle, 1 * INP_DUAL))
    {
        m_sparkcargo->Set(0);
        //m_solenoidarm1->Set(false);
        //m_solenoidarm2->Set(false);
        m_cargostage = kCargoIdle;
        m_mode = kModeAny;
    }

	SmartDashboard::PutNumber("IN0_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN1_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN2_solenoidvac3", m_solenoidvac3->Get());
	SmartDashboard::PutNumber("IN3_solenoidvac4", m_solenoidvac4->Get());
    SmartDashboard::PutNumber("IN4_sparkvac", m_sparkvac->Get());

	SmartDashboard::PutNumber("IN5_solenoidhatch", m_solenoidhatch->Get());

	//SmartDashboard::PutNumber("IN6_solenoidarm1", m_solenoidarm1->Get());
	//SmartDashboard::PutNumber("IN7_solenoidarm2", m_solenoidarm2->Get());

	SmartDashboard::PutNumber("IN8_mode", m_mode);
    SmartDashboard::PutNumber("IN9_hatchmode", m_hatchstage);
    SmartDashboard::PutNumber("IN10_cargomode", m_cargostage);
	SmartDashboard::PutNumber("IN11_waittime", m_waittime);
	SmartDashboard::PutNumber("IN12_cargosensor", m_cargosensor->Get());
}


void Intake::TestLoop()
{
    if (m_solenoidvac1 == nullptr || m_solenoidvac2 == nullptr || m_solenoidvac3 == nullptr || m_solenoidvac4 == nullptr || m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidarm1 == nullptr || m_solenoidarm2 == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;
	
	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
	m_solenoidvac3->Set(false);
	m_solenoidvac4->Set(false);
    m_sparkvac->Set(0);

	m_solenoidhatch->Set(false);

	//m_solenoidarm1->Set(false);
	//m_solenoidarm2->Set(false);

	m_sparkcargo->Set(0);
	
	SmartDashboard::PutNumber("IN0_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN1_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN2_solenoidvac3", m_solenoidvac3->Get());
	SmartDashboard::PutNumber("IN3_solenoidvac4", m_solenoidvac4->Get());
    SmartDashboard::PutNumber("IN4_sparkvac", m_sparkvac->Get());

	SmartDashboard::PutNumber("IN5_solenoidhatch", m_solenoidhatch->Get());

	//SmartDashboard::PutNumber("IN6_solenoidarm1", m_solenoidarm1->Get());
	//SmartDashboard::PutNumber("IN7_solenoidarm2", m_solenoidarm2->Get());

	SmartDashboard::PutNumber("IN8_mode", m_mode);
    SmartDashboard::PutNumber("IN9_hatchmode", m_hatchstage);
    SmartDashboard::PutNumber("IN_10, cargomode", m_cargostage);
	SmartDashboard::PutNumber("IN8_waittime", m_waittime);
	SmartDashboard::PutNumber("IN9_cargosensor", m_cargosensor->Get());
}


void Intake::Stop()
{
    if (m_solenoidvac1 == nullptr || m_solenoidvac2 == nullptr || m_solenoidvac3 == nullptr || m_solenoidvac4 == nullptr || m_sparkvac == nullptr)
        return;
    if (m_solenoidhatch == nullptr)
        return;
    if (m_solenoidarm1 == nullptr || m_solenoidarm2 == nullptr)
        return;
    if (m_sparkcargo == nullptr)
        return;

	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
	m_solenoidvac3->Set(false);
	m_solenoidvac4->Set(false);
    m_sparkvac->Set(0);

	m_solenoidhatch->Set(false);

	//m_solenoidarm1->Set(false);
	//m_solenoidarm2->Set(false);

	m_sparkcargo->Set(0);

	m_timer.Stop();
}


void Intake::VisionLoop()
{
	int counter = m_nettable->GetNumber("visioncounter", 0);
	double angle = m_nettable->GetNumber("XOffAngle", 0) * -1;
	double distance = 0.01;//m_nettable->GetNumber("Forward_Distance_Inch", 0);

	double scale = distance / (96 * 2) + 0.25;
	if (counter > m_counter)
	{
		m_counter = counter;
		if (distance > 0.0)
		{
			m_visiontimer.Reset();
			m_visionvalid = true;
		}
	}
	else
	if (m_visiontimer.Get() > 0.5)
	{
		m_visionvalid = false;
	}

	switch (m_visioning)
	{
	case kIdle:
		if (m_visionvalid && m_inputs->xBoxStartButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
		{
			m_drivepid->Init(m_pid[0], m_pid[1], m_pid[2], DrivePID::Feedback::kGyro);
			m_drivepid->EnablePID();
			m_visioning = kVision;
		}
		else
			m_drivepid->DisablePID();
		m_counter = 0;
		break;

	case kVision:
		if (!m_visionvalid || m_inputs->xBoxBackButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
		{
			m_drivepid->DisablePID();
			m_visioning = kIdle;
		}
		else
		{
			//double x = m_inputs->xBoxLeftX(0 * INP_DUAL) * 90;
			//m_drivepid->SetAbsoluteAngle(x);

			double y = /*m_inputs->xBoxLeftY(0 * INP_DUAL) * */-1*(scale > 1 ? 1 : scale);

			m_drivepid->Drive(y, true);
			m_drivepid->ResetGyro(); // Watch out! Conflicts with drive heading
			m_drivepid->SetAbsoluteAngle(angle);
		}
		break;
	}
	SmartDashboard::PutNumber("IN999_scale", scale);
	SmartDashboard::PutNumber("IN6_visioncounter", counter);
	SmartDashboard::PutNumber("IN7_visionangle", angle);
	SmartDashboard::PutNumber("IN8_distance", distance);
}