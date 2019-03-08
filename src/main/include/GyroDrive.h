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
#include "Vision.h"


using namespace frc;


class GyroDrive
{
public:
    enum DriveState { kInit, kDrive };
    enum DriveMode { kManual, kQuickLeft, kQuickRight, kRetroVision };

	GyroDrive(OperatorInputs *inputs);
	~GyroDrive();
	void Init();
	void Loop();
	void Stop();
    void Disabled();
	void Drive(double x, double y, bool ramp = false);
    void SetStraightPID(double P = -1, double I = -1, double D = -1);
    void SetAnglePID(double P = -1, double I = -1, double D = -1);
    bool DriveStraight(double targetdistance, double autopower, bool reset = true);
    bool DriveAngle(double angle, bool reset = true);
    bool DriveHeading(double heading);
    bool DriveManualAngle(double angle);
    void SetLowSpeedMode(bool mode) {m_drivetrain->SetLowSpeedMode(mode);}
    void QuickLeft();
    void QuickRight();
    void RetroVision();

protected:
    OperatorInputs *m_inputs;
    DriveTrain *m_drivetrain;
    DualGyro *m_gyro;
    DrivePID *m_drivepid;
    Vision *m_vision;

    Timer m_timer;
    DriveMode m_drivemode;
    int m_stage;
    DriveState m_drivestate;
    double m_pidstraight[3];
    double m_pidangle[3];
    double m_distance;
};


#endif /* SRC_GyroDrive_H_ */
