/**
 *  Drivetrain.h
 *  Date: 6/25/19
 *  Last Edited By: Geoffrey Xue
 */


#ifndef SRC_Drivetrain_H_
#define SRC_Drivetrain_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>
#include "OperatorInputs.h"
#include "rev/CANSparkMax.h"

using namespace frc;
using namespace rev;


class Drivetrain
{
public:
	Drivetrain(OperatorInputs *inputs, 
			   CANSparkMax *leftmain = nullptr, CANSparkMax *left2 = nullptr, CANSparkMax *left3 = nullptr,
			   CANSparkMax *rightmain = nullptr, CANSparkMax *right2 = nullptr, CANSparkMax *right3 = nullptr);
	~Drivetrain();
	void Init();
	void Loop();
	void Stop();

protected:
	void ExperimentalData();

private:
	OperatorInputs *m_inputs;

	CANSparkMax *m_leftmain;
	CANSparkMax *m_left2;
	CANSparkMax *m_left3;
	CANSparkMax *m_rightmain;
	CANSparkMax *m_right2;
	CANSparkMax *m_right3;

	CANEncoder *m_leftenc;
	CANEncoder *m_rightenc;

	DifferentialDrive *m_drive;

	bool m_run;
};


#endif /* SRC_Drivetrain_H_ */
