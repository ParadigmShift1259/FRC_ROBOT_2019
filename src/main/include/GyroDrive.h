/**
 *  GyroDrive.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_GyroDrive_H_
#define SRC_GyroDrive_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"
#include "Drivetrain.h"
#include "DrivePID.h"
#include "Gyro.h"


using namespace frc;


class GyroDrive
{
public:
	GyroDrive(OperatorInputs *inputs);
	~GyroDrive();
	void Init();
	void Loop();
	void Stop();
    void Disabled();

protected:
    OperatorInputs *m_inputs;
    DriveTrain *m_drivetrain;
    DrivePID *m_drivepid;
    DualGyro *m_gyro;
};


#endif /* SRC_GyroDrive_H_ */
