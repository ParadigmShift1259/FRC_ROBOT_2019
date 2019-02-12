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
    enum DriveState { kInit, kDrive };

	GyroDrive(OperatorInputs *inputs);
	~GyroDrive();
	void Init();
	void Loop();
	void Stop();
    void Disabled();
	void Drive(double x, double y, bool ramp = false);
    void SetStraightPID(double P = -1, double I = -1, double D = -1);
    void SetAnglePID(double P = -1, double I = -1, double D = -1);
    void AdjAnglePID(double P, double I, double D);
    bool DriveStraight(double targetdistance, double autopower, bool reset = true);
    bool DriveAngle(double angle, bool reset = true);
    bool DriveHeading(double heading);

protected:
    OperatorInputs *m_inputs;
    DriveTrain *m_drivetrain;
    DrivePID *m_drivepid;
    DualGyro *m_gyro;

    Timer m_timer;
    DriveState m_drivestate;
    double m_pidstraight[3];
    double m_pidangle[3];
    double m_distance;
};


#endif /* SRC_GyroDrive_H_ */
