/*
 * Lifter.h
 *
 *  Created on: Jan 20, 2018
 *      Author: Yreffoeg
 */

#ifndef SRC_LIFTER_H_
#define SRC_LIFTER_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"
#include "Intake.h"


using namespace frc;


class Lifter
{
public:
	enum LoopMode {kManual, kAutoUp, kAutoDown};
	enum LifterDir {kNone, kUp, kDown, kBack, kForward};

	Lifter(DriverStation *ds, OperatorInputs *inputs, Intake *intake);
	virtual ~Lifter();
	void Init();
	void Loop();
	void Stop();
	void SetLifter(LifterDir direction);
	bool MoveBottom();

protected:
	void ResetPosition();
	void SetHatchLevels();
	void SetCargoLevels();
	void CheckMoveSmidge();
	bool NearBottom();

	int FindPosition(LifterDir direction);
	void UpdatePosition(LifterDir direction);
	void CheckCargoHatchEjected();

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
	Intake *m_intake;
	WPI_TalonSRX *m_motor;
	WPI_TalonSRX *m_motorslave;
	Solenoid *m_solenoid;

	LoopMode m_loopmode;
	LifterDir m_movingdir;

	int m_position;
	double m_raisespeed;
	double m_lowerspeed;
	double m_liftermin;
	double m_liftermax;
	double m_lifterminspd;
	double m_liftermaxspd;

	int m_lowposition;
	int m_mediumposition;
	int m_highposition;
	int m_selectedposition;

	bool m_prevhascargohatch;
	bool m_movebottom;
	bool m_movesmidge;
	bool m_staging;
};


#endif /* SRC_LIFTER_H_ */
