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

    m_drivestate = kInit;
    m_pidstraight[0] = 0.0;
    m_pidstraight[1] = 0.0;
    m_pidstraight[2] = 0.0;
    m_pidangle[0] = 0.0; // P
    m_pidangle[1] = 0.0; // I
    m_pidangle[2] = 0.0; // D
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
    m_gyro->Init();
    m_drivestate = kInit;
    m_timer.Reset();
    m_timer.Start();
    m_drivestate = kInit;

    m_pidstraight[0] = 0.0;
    m_pidstraight[1] = 0.0;
    m_pidstraight[2] = 0.0;
    m_pidangle[0] = 0.0;  //P
    m_pidangle[1] = 0.0;  //I
    m_pidangle[2] = 0.0;  //D
    m_distance = 0;
}


void GyroDrive::Loop()
{
    m_gyro->Loop();
    if (m_drivepid->GetEnabled())
        m_drivepid->Loop();
    else
        m_drivetrain->Loop();
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
	SmartDashboard::PutNumber("GD01_P", m_pidangle[0]);
	SmartDashboard::PutNumber("GD02_I", m_pidangle[1]);
	SmartDashboard::PutNumber("GD03_D", m_pidangle[2]);
}

void  GyroDrive::AdjAnglePID(double P, double I, double D)
    {    
	m_pidangle[0] += P;
	m_pidangle[1] += I;
    m_pidangle[2] += D;
	m_drivepid->Init(m_pidangle[0], m_pidangle[1], m_pidangle[2], DrivePID::Feedback::kGyro, false);
    SmartDashboard::PutNumber("GD01_P", m_pidangle[0]);
	SmartDashboard::PutNumber("GD02_I", m_pidangle[1]);
	SmartDashboard::PutNumber("GD03_D", m_pidangle[2]);
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
		//if (reset)		redundant remove after testing
		//	m_drivepid->SetAbsoluteAngle(0);
		m_timer.Reset();
		m_drivestate = kDrive;
		break;

	case kDrive:
		m_distance = 0;
		if (m_timer.Get() > 0.1)
			m_distance = m_drivetrain->GetMaxDeltaDistance();
		SmartDashboard::PutNumber("DriveStraight", m_distance);
		modifier = (targetdistance > 0) ? 1 : -1;
		m_distance *= modifier;
		targetdistance *= modifier;

		if (m_distance > targetdistance)
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
	bool on_target;

	switch (m_drivestate)
	{
	case kInit:
		if (m_gyro->GetHeading(gyroheading))
		{
			if (fabs(gyroheading) >= 360.0)
				heading += 360.0 * trunc(gyroheading / 360.0);

			m_drivepid->Init(m_pidangle[0], m_pidangle[1], m_pidangle[2], DrivePID::Feedback::kGyro, false);
			m_drivepid->EnablePID();
			m_drivepid->SetAbsoluteAngle(heading);
			m_drivestate = kDrive;
		}
		break;

	case kDrive:
		m_drivepid->Drive(0, false);
		on_target = m_drivepid->IsOnTarget(3);
		if (on_target)
		{
			m_drivepid->DisablePID();
			m_drivestate = kInit;
			return true;
		}
		break;
	}
	
	SmartDashboard::PutNumber("m_drivestate", m_drivestate);
	SmartDashboard::PutNumber("DriveHeading", heading);
	SmartDashboard::PutNumber("on_target", on_target);
	return false;
}
