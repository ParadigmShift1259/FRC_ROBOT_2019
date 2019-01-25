/*
 * Intake.h
 *
 *  Created on: Jan 20, 2018
 *      Author: Jival
 */

#ifndef SRC_Intake_H_
#define SRC_Intake_H_


#include <frc\WPILib.h>
#include <networktables\NetworkTable.h>
#include <networktables\NetworkTableInstance.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"
#include "Lifter.h"


using namespace frc;
using namespace nt;


class Intake
{
public:
	enum Stage {kTemporary};

	Intake(DriverStation *ds, OperatorInputs *inputs, Lifter *lifter);
	virtual ~Intake();
	void Init();
	void Loop();
	void TestLoop();
	void Stop();

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
	Lifter *m_lifter;
	WPI_TalonSRX *m_motor;
	Solenoid *m_solenoid1;
	Solenoid *m_solenoid2;
	Stage m_stage;
	Timer m_timer;

};


#endif /* SRC_Intake_H_ */
