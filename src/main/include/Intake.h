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
<<<<<<< HEAD
	DrivePID *m_drivepid;
	WPI_TalonSRX *m_motor;
	Solenoid *m_solenoidvac1;
=======

	Solenoid *m_solenoidvac1;		// Used for vaccuum
>>>>>>> 9094d925fae5f9fc350ed92280ca6c5a2b5cff44
	Solenoid *m_solenoidvac2;
	Solenoid *m_solenoidvac3;
	Solenoid *m_solenoidvac4;
	Spark *m_sparkvac;

    Solenoid *m_solenoidhatch;		// Used for ground hatch intake

	Solenoid *m_solenoidarm1;		// Used for moving entire intake
	Solenoid *m_solenoidarm2;

    Spark *m_sparkcargo;			// Used for intaking ball
	DigitalInput *m_cargosensor;
	
	IntakeMode m_mode;
	HatchStage m_hatchstage;
	CargoStage m_cargostage;
	Timer m_timer;
	double m_waittime;
	double m_vacuumpow;
	bool m_onfloor;
<<<<<<< HEAD
	Vision m_visioning;
	double m_pid[3] = {0.015, 0.0, 0.0};
	shared_ptr<NetworkTable> m_nettable;
	Timer m_visiontimer;
	bool m_visionvalid;
	int m_counter;
=======


>>>>>>> 9094d925fae5f9fc350ed92280ca6c5a2b5cff44
};


#endif /* SRC_Intake_H_ */
