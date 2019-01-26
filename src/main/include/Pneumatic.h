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
	enum Stage {kClose, kCapture, kOpen};
    
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
    Solenoid *m_solenoid3;
    Solenoid *m_solenoid4;
    Spark *m_spark1;
    Spark *m_spark2;
    Spark *m_spark3;
    Spark *m_spark4;
    Stage m_stage;
    double m_waittime;
    double m_vacuumpow;
    Timer m_timer;
};


#endif /* SRC_Pneumatic_H_ */
