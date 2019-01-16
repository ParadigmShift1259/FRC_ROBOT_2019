/**
 *  Pneumatic.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_Pneumatic_H_
#define SRC_Pneumatic_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"


using namespace frc;


class Pneumatic
{
public:
	enum Stage {kClose, kOpen};
    
    Pneumatic(DriverStation *ds, OperatorInputs *inputs);
    ~Pneumatic();
    void Init();
    void Loop();
    void Stop();

protected:
    DriverStation *m_ds;
    OperatorInputs *m_inputs;
    Compressor *m_compressor;
    Solenoid *m_solenoid1;
    Solenoid *m_solenoid2;
    Stage m_stage;
    double m_waittime;
    Timer m_timer;
};


#endif /* SRC_Pneumatic_H_ */
