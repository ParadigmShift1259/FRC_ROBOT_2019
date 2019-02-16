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
	enum LifterDir {kUp, kDown};

	Lifter(DriverStation *ds, OperatorInputs *inputs);
	virtual ~Lifter();
	void Init();
	void Loop();
	void TestLoop();
	void Stop();
	void ResetPosition();
	void SetHatchLevels();
	void SetCargoLevels();
	int FindPosition(LifterDir direction);

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
	WPI_TalonSRX *m_motor;
	LoopMode m_loopmode;

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
};


#endif /* SRC_LIFTER_H_ */
