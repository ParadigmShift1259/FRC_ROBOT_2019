/*
 * DrivePID.cpp
 *
 *  Created on: Jan 27, 2018
 *      Author: Developer
 */


#include "DrivePID.h"
#include "const.h"


DrivePID::DrivePID(DriveTrain *drivetrain, DualGyro *gyro, OperatorInputs *inputs): PIDSubsystem(0.0, 0.0, 0.0)
{
	m_drivetrain = drivetrain;
	m_gyro = gyro;
	m_inputs = inputs;
	m_p = 0.0;
	m_i = 0.0;
	m_d = 0.0;
	m_y = 0.0;
	m_ramp = false;
	m_feedback = kDisabled;
	m_heading = 0;
	m_ontarget = 0;
}


DrivePID::~DrivePID()
{
}


void DrivePID::Init(double p, double i, double d, Feedback feedback, bool reset)
{
	m_p = p;
	m_i = i;
	m_d = d;
	m_feedback = feedback;
	GetPIDController()->SetPID(m_p, m_i, m_d);
	SetAbsoluteTolerance(2);
	SetOutputRange(-0.7,0.7);
	if (reset)
	{
		SetSetpoint(0);
		m_gyro->ZeroHeading();
	}
	if (feedback != kDisabled)
		EnablePID();
	m_ontarget = 0;
}


void DrivePID::Loop()
{
	double heading;

	if (m_gyro->GetHeading(heading))
		m_heading = heading;
}


void DrivePID::Stop()
{
	DisablePID();
	SetAbsoluteAngle(0);
	m_heading = 0;
	m_gyro->ZeroHeading();
	//m_pigeon->SetYaw(0,0);
}


void DrivePID::Drive(double y, bool ramp)
{
	SetY(y);
	m_ramp = ramp;
}


bool DrivePID::GetEnabled()
{
	return GetPIDController()->IsEnabled();
}


void DrivePID::SetP(double p)
{
	m_p = p;
	GetPIDController()->SetPID(m_p, m_i, m_d);
}


void DrivePID::SetI(double i)
{
	m_i = i;
	GetPIDController()->SetPID(m_p, m_i, m_d);
}


void DrivePID::SetD(double d)
{
	m_d = d;
	GetPIDController()->SetPID(m_p, m_i, m_d);
}


void DrivePID::SetY(double y)
{
	m_y = (y >= -1.0) ? ((y <= 1.0) ? y : 1.0) : -1.0;
}


void DrivePID::SetRelativeAngle(double angle)
{
	SetSetpointRelative(angle);
}


void DrivePID::SetAbsoluteAngle(double angle)
{
	/*
	double heading;
	double direction;
	int rotations;
	double delta;

	// Setting the angle to positive 0 - 360
	if (angle < 0)
		angle += 360;

	// Scaling the angle (adding 360s) until it reaches heading zone
	rotations = trunc(m_gyro->GetHeading(heading) / 360);
	angle += 360 * rotations;

	// Find change in angle
	delta = m_gyro->GetHeading(heading) - angle;

	/* Constain the angle based on value
		1. If greater than 180 on either side, add 360 in the opposite value of sign
		2. If somewhere around 180, find previous direction of travel and go that direction
		3. If less than 180, pass the change in angle through 
	
	if (delta > 180)
		delta -= 360;
	else if (delta < -180)
		delta += 360;

	if (fabs(delta) >= 170 && fabs(delta) <= 180)
	{
		if (m_gyro->GetDirection(direction) != fabs(delta) / delta)
			if (m_gyro->GetDirection(direction) == 1)
				delta += 360;
			else if (m_gyro->GetDirection(direction) == -1)
				delta -= 360;
	}
	this->SetSetpoint(m_gyro->GetHeading(heading) - delta);
	*/
	double heading;
	double heading1;
	double direction;
	int rotations;
	double delta;
	bool success = m_gyro->GetHeading(heading1);

	heading = heading1 - 360 * truncf(heading1 / 360);

	if (angle < 0)
		angle += 360;

	// Find change in angle
	delta = angle - heading;
	
	if (delta > 180)
		delta -= 360;
	else if (delta < -180)
		delta += 360;
	/*
	if (fabs(delta) >= 170 && fabs(delta) <= 180)
	{
		if (m_gyro->GetDirection(direction) != fabs(delta) / delta)
			if (m_gyro->GetDirection(direction) == 1)
				delta += 360;
			else if (m_gyro->GetDirection(direction) == -1)
				delta -= 360;
	}
	*/
	this->SetSetpointRelative(delta);
	

	SmartDashboard::PutNumber("DI1_angle", angle);
	SmartDashboard::PutNumber("DI2_direction", direction);
	SmartDashboard::PutNumber("DI3_delta", delta);
	SmartDashboard::PutNumber("DI4_heading", heading);

}


void DrivePID::ResetGyro()
{
	m_gyro->ZeroHeading();
}


bool DrivePID::IsOnTarget(double count)
{
	if (OnTarget())
	{
		if (count == 0)
		{
			m_ontarget = 0;
			return true;
		}
		else
		{
			m_ontarget++;
			if (m_ontarget > count)
			{
				m_ontarget = 0;
				return true;
			}
			return false;
		}
	}
	m_ontarget = 0;
	return false;
}


void DrivePID::EnablePID()
{
	GetPIDController()->SetPID(m_p, m_i, m_d);
	GetPIDController()->Reset();
	m_ontarget = 0;
	Enable();
}


void DrivePID::DisablePID()
{
	if (GetPIDController()->IsEnabled())
	{
		Disable();
	}
}


double DrivePID::ReturnPIDInput()
{
	if (m_feedback == kEncoder)
	{
		double m_leftpos = m_drivetrain->LeftTalon1()->GetSelectedSensorPosition(0);
		m_leftpos = m_leftpos / CODES_PER_REV;

		double m_rightpos = m_drivetrain->RightTalon1()->GetSelectedSensorPosition(0);
		m_rightpos = m_rightpos / CODES_PER_REV;

		double retval = (360 / (2 * 3.1415926535)) * (m_leftpos + m_rightpos) * WHEEL_DIAMETER * 3.1415926535 / WHEEL_TRACK;

		SmartDashboard::PutNumber("ReturnPosition(Enc)", m_leftpos + m_rightpos);
		SmartDashboard::PutNumber("ReturnCurrentPosition(Enc)", retval);

		return retval;
	}
	else
	if (m_feedback == kGyro)
	{
		double retval;

		if (!m_gyro->GetHeading(retval))
			retval = 0;

		SmartDashboard::PutNumber("ReturnPIDInput(Gyro)", retval);

		return retval;
	}
	return 0;
}


void DrivePID::UsePIDOutput(double output)
{
	m_drivetrain->Drive(output, m_y, m_ramp);
}
