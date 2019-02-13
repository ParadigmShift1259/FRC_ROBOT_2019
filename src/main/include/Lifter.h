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
	enum Stage {kIdle, kRaise};

	Lifter(DriverStation *ds, OperatorInputs *inputs);
	virtual ~Lifter();
	void Init();
	void ManualLoop();
	void AutoLoop();
	void TestLoop();
	void Stop();
	void ResetPosition();
	void SetHatchLevels();
	void SetCargoLevels();
	bool GoToPosition(int designatedposition);

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
	WPI_TalonSRX *m_motor;
	Stage m_stage;
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
	
	bool m_manualraising;
	bool m_autoraising;
};


#endif /* SRC_LIFTER_H_ */
