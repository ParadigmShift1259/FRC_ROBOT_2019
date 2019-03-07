/**
 *  GyroDrive.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_Autonomous_H_
#define SRC_Autonomous_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include <networktables\NetworkTable.h>
#include <networktables\NetworkTableInstance.h>
#include "OperatorInputs.h"
#include "GyroDrive.h"
#include "Lifter.h"
#include "Intake.h"


using namespace frc;


class Autonomous
{
public:
	enum Vision {kIdle, kVision};

	Autonomous(OperatorInputs *inputs, GyroDrive *gyrodrive, Lifter *lifter, Intake *intake);
	~Autonomous();
	void Init();
	void Loop();
	void Stop();

protected:
	bool StartSequence();
	void AutoLeft();
	void AutoCenter();
	void AutoRight();
	void AutoPID();
	void AutoVision();

protected:
    OperatorInputs *m_inputs;
    GyroDrive *m_gyrodrive;
	Lifter *m_lifter;
	Intake *m_intake;
	Vision m_visioning;
	Timer m_visiontimer;

	shared_ptr<NetworkTable> m_nettable;
	int m_counter;
    int m_stage;
	bool m_visionvalid;
	int m_startstage;
	double m_heading;
};


#endif /* SRC_Autonomous_H_ */
