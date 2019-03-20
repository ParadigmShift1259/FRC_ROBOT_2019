/**
 *  Climber.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_Climber_H_
#define SRC_Climber_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"


using namespace frc;


class Climber
{
public:
	Climber(OperatorInputs *inputs);
	~Climber();
	void Init();
	void Loop();
	void Stop();

protected:
    OperatorInputs *m_inputs;
    Solenoid *m_solenoid;
};


#endif /* SRC_Climber_H_ */
