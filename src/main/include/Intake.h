/*
 * Intake.h
 *
 *  Created on: Jan 20, 2018
 *      Author: Jival
 */

#ifndef SRC_Intake_H_
#define SRC_Intake_H_


#include <frc\WPILib.h>
#include <networktables\NetworkTable.h>
#include <networktables\NetworkTableInstance.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"
#include "Lifter.h"
#include "DrivePID.h"
#include <networktables\NetworkTable.h>
#include <networktables\NetworkTableInstance.h>


using namespace frc;
using namespace nt;


class Intake
{
public:
	enum IntakeMode {kModeHatch, kModeCargo, kModeAny};
	enum HatchStage {kHatchIdle, kHatchCapture, kHatchRelease};
	enum CargoStage {kCargoIdle, kCargoIngest, kCargoIngestWait, kCargoBall, kCargoEject};
	enum Vision {kIdle, kVision};

	Intake(DriverStation *ds, OperatorInputs *inputs, Lifter *lifter, DrivePID *drivepid);
	virtual ~Intake();
	void Init();
	void HatchLoop();
	void CargoLoop();
	void TestLoop();
	void Stop();
	void VisionLoop();

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
	Lifter *m_lifter;
	DrivePID *m_drivepid;
	WPI_TalonSRX *m_motor;
	Solenoid *m_solenoidvac1;
	Solenoid *m_solenoidvac2;
    Solenoid *m_solenoidhatch1;
    Solenoid *m_solenoidhatch2;
	Solenoid *m_solenoidarm1;
	Solenoid *m_solenoidarm2;
    Spark *m_spark1;
    Spark *m_spark2;
	IntakeMode m_mode;
	HatchStage m_hatchstage;
	CargoStage m_cargostage;
	Timer m_timer;
	double m_waittime;
    DigitalInput *m_cargosensor;
	bool m_onfloor;
	Vision m_visioning;
	double m_pid[3] = {0.015, 0.0, 0.0};
	shared_ptr<NetworkTable> m_nettable;
	Timer m_visiontimer;
	bool m_visionvalid;
	int m_counter;
};


#endif /* SRC_Intake_H_ */
