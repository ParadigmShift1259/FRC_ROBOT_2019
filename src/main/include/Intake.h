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
	enum IntakeMode {kModeNone, kModeHatch, kModeCargo};
	enum HatchStage {kHatchIdle, kHatchCapture, kHatchRelease};
	enum CargoStage {kCargoIdle, kCargoIngest, kCargoIngestWait, kCargoBall, kCargoEject};
	enum FlushStage {kFlushIdle, kFlushPoof, kFlushEject};
	enum Vision {kIdle, kVision};

	Intake(DriverStation *ds, OperatorInputs *inputs, Lifter *lifter, DrivePID *drivepid);
	virtual ~Intake();
	void Init();
	void Loop();
	void TestLoop();
	//void VisionLoop();
	void Stop();

protected:	
	void Hatch();
	void Cargo();
	void Flush();

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;
	Lifter *m_lifter;

	Solenoid *m_solenoidvac1;		// Used for vaccuum 
	Solenoid *m_solenoidvac2;
	Solenoid *m_solenoidvac3;
	Solenoid *m_solenoidvac4;
	Spark *m_sparkvac;

    Solenoid *m_solenoidhatch;		// Used for ground hatch intake

	Solenoid *m_solenoidarm;		// Used for moving entire intake

    Spark *m_sparkcargo;			// Used for intaking ball
	DigitalInput *m_cargosensor;
	
	IntakeMode m_mode;
	HatchStage m_hatchstage;
	CargoStage m_cargostage;
	FlushStage m_flushstage;

	Timer m_timer;
	double m_waittime;
	double m_vacuumpow;
	bool m_onfloor;
	bool m_inited;
};


#endif /* SRC_Intake_H_ */
