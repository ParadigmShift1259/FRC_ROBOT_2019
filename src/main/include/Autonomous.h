/**
 *  GyroDrive.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_Autonomous_H_
#define SRC_Autonomous_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"
#include "GyroDrive.h"
#include "Autonomous.h"


using namespace frc;


class Autonomous
{
public:
	Autonomous(OperatorInputs *inputs, GyroDrive *gyrodrive);
	~Autonomous();
	void Init();
	void Loop();
	void Stop();

protected:
    OperatorInputs *m_inputs;
    GyroDrive *m_gyrodrive;

    int m_stage;
};


#endif /* SRC_Autonomous_H_ */
