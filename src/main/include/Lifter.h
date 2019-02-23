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


using namespace frc;


class Lifter
{
public:
	enum LoopMode {kManual, kAutoUp, kAutoDown};
	enum LifterDir {kNone, kUp, kDown};

	Lifter(DriverStation *ds, OperatorInputs *inputs);
	virtual ~Lifter();
	void Init();
	void Loop();
	void TestLoop();
	void Stop();
	void ResetPosition();
	void SetHatchLevels();
	void SetCargoLevels();
	bool MoveSmidgeUp();
	void CargoEjected() { m_cargoejected = true; }
	bool NearBottom();
	bool IsStaging() { return m_staging; }

protected:
	int FindPosition(LifterDir direction);
	void UpdatePosition(LifterDir direction);

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
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

	bool m_smidge;
	bool m_cargoejected;
	bool m_staging;
};


#endif /* SRC_LIFTER_H_ */
