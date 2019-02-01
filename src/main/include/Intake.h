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
	enum Stage {kClose, kCapture, kOpen};

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
    Solenoid *m_solenoid3;
    Solenoid *m_solenoid4;
    Spark *m_spark1;
    Spark *m_spark2;
	Stage m_stage;
	Timer m_timer;
	double m_waittime;
    DigitalInput *m_cargosensor;
};


#endif /* SRC_Intake_H_ */
