/*
 * Intake.h
 *
 *  Created on: Jan 20, 2018
 *      Author: Jival
 */

#ifndef SRC_Intake_H_
#define SRC_Intake_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"


using namespace frc;
using namespace nt;


class Intake
{
public:
	enum IntakeMode {kModeNone, kModeHatch, kModeCargo};
	enum HatchStage {kHatchIdle, kHatchCapture, kHatchRelease};
	enum HatchDir {kHatchUp, kHatchDown};
	enum HatchVac {kVacOn, kVacOff, kPoofOn, kPoofOff};
	enum CargoStage {kCargoIdle, kCargoIngest, kCargoIngestWait, kCargoBall, kCargoEject};
	enum CargoDir {kCargoUp, kCargoDown, kCargoOff, kCargoIn, kCargoOut};
	enum FlushStage {kFlushNone, kFlushStart, kFlushPoof, kFlushEject};

	Intake(DriverStation *ds, OperatorInputs *inputs);
	virtual ~Intake();
	void Init();
	void Loop();
	void Stop();

	void SetIntakeMode(IntakeMode intakemode);
	IntakeMode GetIntakeMode() { return m_mode; }
	void SetCargoIntake(CargoDir cargodir);
	CargoDir GetCargoIntake();
	void SetHatchIntake(HatchDir hatchdir);
	HatchDir GetHatchIntake();
	void SetHatchVac(HatchVac hatchvac);

	bool HasCargoHatch() { return m_hascargohatch; }
	void SetAtBottom(bool atbottom) { m_atbottom = atbottom; }

	void SetAutoIngest(bool ingest) { m_autoingest = ingest; }
	void SetFinishIngest(bool ingest) { m_finishingest = ingest; }
	void SetAutoEject(bool eject) { m_autoeject = eject; }

protected:	
	void Hatch();
	void Cargo();
	void Flush();

protected:
	DriverStation *m_ds;
	OperatorInputs *m_inputs;

	Solenoid *m_solenoidvac1;		// Used for vaccuum 
	Solenoid *m_solenoidvac2;
	Solenoid *m_solenoidvac3;
	Solenoid *m_solenoidvac4;
	Spark *m_sparkvac;

    Solenoid *m_solenoidhatch;		// Used for ground hatch intake
	Solenoid *m_solenoidcargo;		// Used for moving entire intake

    Spark *m_sparkcargo;			// Used for intaking ball
	DigitalInput *m_cargosensor;
	
	IntakeMode m_mode;
	HatchStage m_hatchstage;
	CargoStage m_cargostage;
	FlushStage m_flushstage;

	Timer m_timer;
	double m_waittime;
	double m_vacuumpow;
	bool m_inited;
	bool m_hascargohatch;
	bool m_atbottom;

	bool m_autoingest;
	bool m_finishingest;
	bool m_autoeject;
};


#endif /* SRC_Intake_H_ */
