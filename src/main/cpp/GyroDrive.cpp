/**
 *  GryoDrive.cpp
 *  Date:
 *  Last Edited By:
 */


#include "GyroDrive.h"
#include "Const.h"
#include <cmath>


using namespace std;


GyroDrive::GyroDrive(OperatorInputs *inputs)
{
    m_inputs = inputs;
    m_drivetrain = new DriveTrain(inputs);
    m_gyro = new DualGyro(CAN_GYRO1, CAN_GYRO2);
    m_drivepid = new DrivePID(m_drivetrain, m_gyro, m_inputs);
	m_vision = new Vision();

	m_drivemode = kManual;
	m_stage = 0;
    m_drivestate = kInit;
    m_pidstraight[0] = 0.0;
    m_pidstraight[1] = 0.0;
    m_pidstraight[2] = 0.0;
    m_pidangle[0] = 0.0;
    m_pidangle[1] = 0.0;
    m_pidangle[2] = 0.0;
    m_distance = 0;
}


GyroDrive::~GyroDrive()
{
    if (m_drivepid != nullptr)
        delete m_drivepid;
    if (m_gyro != nullptr)
        delete m_gyro;
    if (m_drivetrain != nullptr)
        delete m_drivetrain;
}


void GyroDrive::Init()
{
    m_drivetrain->Init(DriveTrain::DriveMode::kFollower);
	// disable change direction in drivetrain
	m_drivetrain->SetChangeDirButton(-1);		
	// if single controller use L3 for shifter else left trigger
	if (!INP_DUAL)
		m_drivetrain->SetShifterButton(L3_BUTTON);
    m_gyro->Init();
    m_drivestate = kInit;
    m_timer.Reset();
    m_timer.Start();
	m_drivemode = kManual;
	m_stage = 0;
    m_drivestate = kInit;

	SetStraightPID(AUT_P, AUT_I, AUT_D);    // 2018 Values (0.04, 0.0012, 0.07)
    SetAnglePID(AUT_P, AUT_I, AUT_D);        // Tuned 3/2/19 (0.1, 0.0003, 0.11)
    m_distance = 0;
}


void GyroDrive::Loop()
{
    m_gyro->Loop();
	m_vision->Loop();
    if (m_drivepid->GetEnabled())
        m_drivepid->Loop();

	switch (m_drivemode)
	{
	case kManual:
		if (!m_drivepid->GetEnabled())
		{
			m_drivetrain->Loop();
			RetroVision();
		}
		break;
	
	case kRetroVision:
		RetroVision();
		break;
	}
	
}


void GyroDrive::Stop()
{
    m_drivetrain->Stop();
    m_drivepid->Stop();
    m_gyro->Stop();
}


void GyroDrive::Disabled()
{
    m_gyro->Loop();
}


void GyroDrive::Drive(double x, double y, bool ramp)
{
    m_drivetrain->Drive(x, y, ramp);
}


void GyroDrive::SetStraightPID(double P, double I, double D)
{
    if (P != -1)
        m_pidstraight[0] = P;
    if (I != -1)
        m_pidstraight[1] = I;
    if (D != -1)
        m_pidstraight[2] = D;
}


void GyroDrive::SetAnglePID(double P, double I, double D)
{
    if (P != -1)
        m_pidangle[0] = P;
    if (I != -1)
        m_pidangle[1] = I;
    if (D != -1)
        m_pidangle[2] = D;
}


void GyroDrive::GetAnglePID(double &P, double &I, double &D)
{
	P = m_pidangle[0];
	I = m_pidangle[1];
	D = m_pidangle[2];
}


bool GyroDrive::DriveStraight(double targetdistance, double autopower, bool reset)
{
	double modifier;

	switch (m_drivestate)
	{
	case kInit:
		// accelerates during this case for a duration specified by ACCEL_TIME, feeds into kMaintain
		m_drivetrain->ResetDeltaDistance();
		m_distance = m_drivetrain->GetMaxDeltaDistance();
		SmartDashboard::PutNumber("DriveStraight", m_distance);
		m_drivepid->Init(m_pidstraight[0], m_pidstraight[1], m_pidstraight[2], DrivePID::Feedback::kGyro, reset);
		m_drivepid->EnablePID();
		m_timer.Reset();
		m_drivestate = kDrive;
		break;

	case kDrive:
		m_distance = 0;
		if (m_timer.Get() > 0.1)
			m_distance = m_drivetrain->GetMaxDeltaDistance();
		SmartDashboard::PutNumber("DriveStraight", m_distance);
		modifier = (autopower > 0) ? 1 : -1;
		m_distance *= modifier;

		if (m_distance > fabs(targetdistance))
		{
			m_drivepid->Drive(0);
			m_drivepid->DisablePID();
			m_drivetrain->Drive(0, 0, false);
			m_drivestate = kInit;
			return true;
		}
		else
		{
			m_drivepid->Drive(-1 * autopower, true);
		}
		break;
	}
	return false;
}


bool GyroDrive::DriveAngle(double angle, bool reset)
{
	switch (m_drivestate)
	{
	case kInit:
		m_drivepid->Init(m_pidangle[0], m_pidangle[1], m_pidangle[2], DrivePID::Feedback::kGyro, reset);
		m_drivepid->EnablePID();
		m_drivepid->SetRelativeAngle(angle);
		m_drivestate = kDrive;
		break;

	case kDrive:
		m_drivepid->Drive(0, false);
		if (m_drivepid->IsOnTarget(3))
		{
			m_drivepid->DisablePID();
			m_drivestate = kInit;
			return true;
		}
		break;
	}
	return false;
}


bool GyroDrive::DriveHeading(double heading)
{
	double gyroheading;

	switch (m_drivestate)
	{
	case kInit:
		if (m_gyro->GetHeading(gyroheading))
		{
			if (fabs(gyroheading) >= 360.0)
				heading += 360.0 * trunc(gyroheading / 360.0);
			SmartDashboard::PutNumber("DriveHeading", heading);
			m_drivepid->Init(m_pidangle[0], m_pidangle[1], m_pidangle[2], DrivePID::Feedback::kGyro, false);
			m_drivepid->EnablePID();
			m_drivepid->SetAbsoluteAngle(heading);
			m_drivestate = kDrive;
		}
		break;

	case kDrive:
		m_drivepid->Drive(0, false);
		if (m_drivepid->IsOnTarget(3))
		{
			m_drivepid->DisablePID();
			m_drivestate = kInit;
			return true;
		}
		break;
	}
	return false;
}


bool GyroDrive::DriveManualAngle(double angle, bool update)
{
	switch (m_drivestate)
	{
	case kInit:
		m_drivepid->Init(m_pidangle[0], m_pidangle[1], m_pidangle[2], DrivePID::Feedback::kGyro, false);
		m_drivepid->EnablePID();
		m_drivestate = kDrive;
		break;

	case kDrive:
		double y = m_inputs->xBoxLeftY(0 * INP_DUAL);
		if (m_drivetrain->GetLowSpeedMode())
			y = y * LOWSPEED_MODIFIER_Y;

		m_drivepid->Drive(y, true);
		if (update)
		{
			m_drivepid->SetAbsoluteAngle(m_drivepid->GetPosition());
			m_drivepid->SetRelativeAngle(angle);
		}
		if (m_drivepid->IsOnTarget(3))
			return true;
		break;
	}
	return false;
}

/*
bool GyroDrive::DriveAutoAngle(double angle, double distance, bool update)
{
	switch (m_drivestate)
	{
	case kInit:
		m_drivepid->Init(m_pidangle[0], m_pidangle[1], m_pidangle[2], DrivePID::Feedback::kGyro, false);
		m_drivepid->EnablePID();
		m_drivestate = kDrive;
		break;

	case kDrive:
		double distMultiplier = SmartDashboard::GetNumber("DB/Slider 0", 0.0);
		double powerConst = SmartDashboard::GetNumber("DB/Slider 1", 0.0);
		// distMultiplier = 100, powerConst = 0.18?
		double y = distance / (distMultiplier) + powerConst;		// distance / (96 * 2) + 0.25
		if (m_drivetrain->GetLowSpeedMode())
			y = y * LOWSPEED_MODIFIER_Y;

		m_drivepid->Drive(y, true);
		if (update)
		{
			m_drivepid->SetAbsoluteAngle(m_drivepid->GetPosition());
			m_drivepid->SetRelativeAngle(angle);
		}
		if (m_drivepid->IsOnTarget(3))
			return true;
		break;
	}
	return false;
}
*/

void GyroDrive::RetroVision()
{
	double angle, distance;
	bool result;

	if (m_drivemode == kManual)
	{
		if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
		{
			if (m_vision->GetRetro(angle, distance) && (distance > 12.0))
			{
				DriveManualAngle(angle * 0.35, true);
				m_drivemode = kRetroVision;
			}
		}
	}
	else
	if (m_drivemode == kRetroVision)
	{
		if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
		{
			m_drivepid->DisablePID();
			m_stage = 0;
			m_drivestate = kInit;
			m_drivemode = kManual;
		}
		else
		{
			if (m_vision->GetRetro(angle, distance) && distance > 12.0)
				DriveManualAngle(angle * 0.35, true);		//0.35
			else
				DriveManualAngle();
		}
	}
}
