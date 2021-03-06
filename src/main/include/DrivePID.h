/*
 * DrivePID.h
 *
 *  Created on: Jan 27, 2018
 *      Author: Developer
 */


#ifndef SRC_DRIVEPID_H_
#define SRC_DRIVEPID_H_


#include <Drivetrain.h>
#include <frc\WPILib.h>
#include "OperatorInputs.h"
#include "Gyro.h"


using namespace frc;


class DrivePID: public PIDSubsystem
{
public:
	enum Feedback { kDisabled, kEncoder, kGyro };

	DrivePID(DriveTrain *drivetrain, DualGyro *gyro, OperatorInputs *inputs);
	~DrivePID();
	void Init(double p = 0, double i = 0, double d = 0, Feedback feedback = kDisabled, bool reset = true);
	void Loop();
	void Drive(double y, bool ramp = false);
	void Stop();

	bool GetEnabled();

	void SetP(double p);
	void SetI(double i);
	void SetD(double d);
	void SetY(double y);
	void SetRelativeAngle(double angle);
	void SetAbsoluteAngle(double angle);
	void ResetGyro();
	bool IsOnTarget(double count = 0);

	void EnablePID();
	void DisablePID();
	double ReturnPIDInput();
	void UsePIDOutput(double output);

protected:
	DriveTrain *m_drivetrain;
	DualGyro *m_gyro;
	OperatorInputs *m_inputs;
	Feedback m_feedback;
	double m_heading;
	double m_p;
	double m_i;
	double m_d;
	double m_y;
	double m_ramp;
	int m_ontarget;
};


#endif /* SRC_DRIVEPID_H_ */
