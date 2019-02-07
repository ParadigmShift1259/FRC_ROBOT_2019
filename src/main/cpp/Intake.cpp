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
	m_motor = nullptr;
	m_solenoidvac1 = nullptr;
	m_solenoidvac2 = nullptr;
	m_solenoidhatch1 = nullptr;
	m_solenoidhatch2 = nullptr;
	m_solenoidarm1 = nullptr;
	m_solenoidarm2 = nullptr;
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

	if (CAN_INTAKE_MOTOR != -1)
	{
		m_motor = new WPI_TalonSRX(CAN_INTAKE_MOTOR);
		m_motor->Set(ControlMode::PercentOutput, 0);
		m_motor->SetNeutralMode(NeutralMode::Brake);
	}

	if (PCM_INTAKE_SOLENOIDVAC1 != -1)
		m_solenoidvac1 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDVAC1);
	if (PCM_INTAKE_SOLENOIDVAC2 != -1)
		m_solenoidvac2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDVAC2);
	if (PCM_INTAKE_SOLENOIDHATCH1 != -1)
		m_solenoidhatch1 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDHATCH1);
	if (PCM_INTAKE_SOLENOIDHATCH2 != -1)
		m_solenoidhatch2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDHATCH2);
    if (PCM_INTAKE_SOLENOIDARM1 != -1)
		m_solenoidarm1 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDARM1);
	if (PCM_INTAKE_SOLENOIDARM2 != -1)
		m_solenoidarm2 = new Solenoid(PCM_INTAKE_MODULE, PCM_INTAKE_SOLENOIDARM2);
	
	if (SPARK1 != -1)
		m_spark1 = new Spark(SPARK1);
	if (SPARK2 != -1)
    	m_spark2 = new Spark(SPARK2);
	
	m_cargosensor = new DigitalInput(DIO_INTAKE_CARGOSENSOR);
    m_onfloor = false;
	m_mode = kModeAny;
    m_hatchstage = kHatchIdle;
    m_cargostage = kCargoIdle;
}


Intake::~Intake()
{
	if (m_motor != nullptr)
		delete m_motor;
	if (m_solenoidvac1 != nullptr)
        delete m_solenoidvac1;
    if (m_solenoidvac2 != nullptr)
        delete m_solenoidvac2;
	if (m_solenoidhatch1 != nullptr)
        delete m_solenoidhatch1;
    if (m_solenoidhatch2 != nullptr)
        delete m_solenoidhatch2;
	if (m_solenoidarm1 != nullptr)
        delete m_solenoidarm1;
    if (m_solenoidarm2 != nullptr)
        delete m_solenoidarm2;
    if (m_spark1 != nullptr)
        delete m_spark1;
    if (m_spark2 != nullptr)
        delete m_spark2;
}



void Intake::Init()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoidvac1 == nullptr) || (m_solenoidvac2 == nullptr))
        return;
    if ((m_solenoidhatch1 == nullptr) || (m_solenoidhatch2 == nullptr))
        return;
    if ((m_solenoidarm1 == nullptr) || (m_solenoidarm2 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;
	
	DriverStation::ReportError("IntakeInit");

	m_motor->StopMotor();
	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
	m_solenoidhatch1->Set(false);
	m_solenoidhatch2->Set(false);
	m_solenoidarm1->Set(false);
	m_solenoidarm2->Set(false);
    m_spark1->Set(0);
    m_spark2->Set(0);
    m_visiontimer.Reset();
    m_visiontimer.Start();
    m_visionvalid = false;
}


void Intake::HatchLoop()
{
    if ((m_solenoidvac1 == nullptr) || (m_solenoidvac2 == nullptr))
        return;
    if ((m_solenoidhatch1 == nullptr) || (m_solenoidhatch2 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;
    
    if (m_mode == kModeCargo)
    {
        m_spark1->Set(0);
        m_spark2->Set(0);
        return;
    }

    switch (m_hatchstage)
    {
    case kHatchIdle:
    	DriverStation::ReportError("kHatchIdle");

		m_motor->StopMotor();
        m_solenoidvac1->Set(false);
        m_solenoidvac2->Set(false);
        m_solenoidhatch1->Set(false);
        m_solenoidhatch2->Set(false);
        m_spark1->Set(0);
        m_spark2->Set(0);
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_hatchstage = kHatchCapture;
        }
        break;

    case kHatchCapture:
    	DriverStation::ReportError("kHatchCapture");
        m_mode = kModeCargo;

		m_motor->StopMotor();
        m_solenoidvac1->Set(false);
        m_solenoidvac2->Set(false);
        m_spark1->Set(0.3);
        m_spark2->Set(0.3);

        if (m_inputs->xBoxBackButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_onfloor = true;

        if (m_onfloor)
        {
            m_solenoidhatch1->Set(true);
            m_solenoidhatch2->Set(true);
        }
        else
        {
            m_solenoidhatch1->Set(false);
            m_solenoidhatch2->Set(false);
        }

        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_solenoidvac1->Set(true);
            m_solenoidvac2->Set(true);
            m_timer.Reset();
            m_hatchstage = kHatchRelease;
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

    case kHatchRelease:
    	DriverStation::ReportError("kHatchRelease");

        m_spark1->Set(0);
        m_spark2->Set(0);
        m_solenoidhatch1->Set(false);
        m_solenoidhatch2->Set(false);

        if (m_timer.Get() > m_waittime)
        {
			m_motor->StopMotor();
            m_solenoidvac1->Set(false);
            m_solenoidvac2->Set(false);
            m_solenoidhatch1->Set(false);
            m_solenoidhatch2->Set(false);
            m_onfloor = false;
            m_hatchstage = kHatchIdle;
            m_mode = kModeAny;
        }
        break;
    }

	SmartDashboard::PutNumber("IN1_motor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN2_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN3_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN4_solenoidhatch1", m_solenoidhatch1->Get());
	SmartDashboard::PutNumber("IN5_solenoidhatch2", m_solenoidhatch2->Get());
	SmartDashboard::PutNumber("IN6_solenoidarm1", m_solenoidarm1->Get());
	SmartDashboard::PutNumber("IN7_solenoidarm2", m_solenoidarm2->Get());
	SmartDashboard::PutNumber("IN8_mode", m_mode);
	SmartDashboard::PutNumber("IN9_waittime", m_waittime);
	SmartDashboard::PutNumber("IN10_cargosensor", m_cargosensor->Get());
}


void Intake::CargoLoop()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoidarm1 == nullptr) || (m_solenoidarm2 == nullptr))
        return;

    if (m_mode == kModeHatch)
    {
        m_motor->Set(0);
        return;
    }

    switch (m_cargostage)
    {
    case kCargoIdle:
    	DriverStation::ReportError("kCargoIdle");

		m_motor->StopMotor();
        m_solenoidarm1->Set(false);
        m_solenoidarm2->Set(false);
        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_cargostage = kCargoIngest;
        }
        break;

    case kCargoIngest:
    	DriverStation::ReportError("kCargoIngest");
        m_mode = kModeCargo;

		m_motor->Set(1.0);
        m_solenoidarm1->Set(true);
        m_solenoidarm2->Set(true);

        if (m_cargosensor->Get() /*or manual check?*/)
        {
			m_timer.Reset();
            m_cargostage = kCargoBall;
        }
        break;

    case kCargoIngestWait:
    	DriverStation::ReportError("kCargoIngestWait");

        if (m_timer.Get() > 1.0)        // Magic # used for ingest
        {
            m_motor->StopMotor();
            m_cargostage = kCargoBall;
        }
        else
        {
            m_motor->Set(1.0);
        }
        break;
    
    case kCargoBall:
    	DriverStation::ReportError("kCargoBall");

		m_motor->StopMotor();
        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_motor->Set(1.0);
        }
        if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_motor->Set(-1.0);
            m_timer.Reset();
            m_cargostage = kCargoEject;
        }
        break;
    
    case kCargoEject:
        if (m_timer.Get() > 1.5)    // Magic # for eject
        {
            m_motor->StopMotor();
            m_cargostage = kCargoIdle;
            m_mode = kModeAny;
        }
        else
        {
            m_motor->Set(-1.0);
        }
        break;

    };

	SmartDashboard::PutNumber("IN1_motor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN3_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN4_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN3_solenoidhatch1", m_solenoidhatch1->Get());
	SmartDashboard::PutNumber("IN4_solenoidhatch2", m_solenoidhatch2->Get());
	SmartDashboard::PutNumber("IN3_solenoidarm1", m_solenoidarm1->Get());
	SmartDashboard::PutNumber("IN4_solenoidarm2", m_solenoidarm2->Get());
	SmartDashboard::PutNumber("IN7_mode", m_mode);
	SmartDashboard::PutNumber("IN8_waittime", m_waittime);
	SmartDashboard::PutNumber("IN9_cargosensor", m_cargosensor->Get());
}


void Intake::TestLoop()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoidvac1 == nullptr) || (m_solenoidvac2 == nullptr))
        return;
    if ((m_solenoidhatch1 == nullptr) || (m_solenoidhatch2 == nullptr))
        return;
    if ((m_solenoidarm1 == nullptr) || (m_solenoidarm2 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;
	
	m_motor->StopMotor();
	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
	m_solenoidhatch1->Set(false);
	m_solenoidhatch2->Set(false);
	m_solenoidarm1->Set(false);
	m_solenoidarm2->Set(false);
	m_spark1->Set(0);
	m_spark2->Set(0);
	
	SmartDashboard::PutNumber("IN1_motor", m_motor->GetSelectedSensorVelocity(0));
	SmartDashboard::PutNumber("IN2_solenoidvac1", m_solenoidvac1->Get());
	SmartDashboard::PutNumber("IN3_solenoidvac2", m_solenoidvac2->Get());
	SmartDashboard::PutNumber("IN4_solenoidhatch1", m_solenoidhatch1->Get());
	SmartDashboard::PutNumber("IN5_solenoidhatch2", m_solenoidhatch2->Get());
	SmartDashboard::PutNumber("IN6_solenoidarm1", m_solenoidarm1->Get());
	SmartDashboard::PutNumber("IN7_solenoidarm2", m_solenoidarm2->Get());
	SmartDashboard::PutNumber("IN8_mode", m_mode);
	SmartDashboard::PutNumber("IN9_waittime", m_waittime);
	SmartDashboard::PutNumber("IN10_cargosensor", m_cargosensor->Get());
}


void Intake::Stop()
{
	if (m_motor == nullptr)
		return;
    if ((m_solenoidvac1 == nullptr) || (m_solenoidvac2 == nullptr))
        return;
    if ((m_solenoidhatch1 == nullptr) || (m_solenoidhatch2 == nullptr))
        return;
    if ((m_solenoidarm1 == nullptr) || (m_solenoidarm2 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr)
        return;

	m_motor->StopMotor();
	m_solenoidvac1->Set(false);
	m_solenoidvac2->Set(false);
	m_solenoidhatch1->Set(false);
	m_solenoidhatch2->Set(false);
	m_solenoidarm1->Set(false);
	m_solenoidarm2->Set(false);
	m_spark1->Set(0);
	m_spark2->Set(0);
	m_timer.Stop();
}


void Intake::VisionLoop()
{
	int counter = m_nettable->GetNumber("visioncounter", 0);
	double angle = m_nettable->GetNumber("XOffAngle", 0) * -1;
	double distance = m_nettable->GetNumber("Forward_Distance_Inch", 0);

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