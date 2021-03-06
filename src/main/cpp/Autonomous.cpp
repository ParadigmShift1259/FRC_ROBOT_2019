/**
 *  Autonomous.cpp
 *  Date:
 *  Last Edited By:
 */


#include "Autonomous.h"
#include "Const.h"


using namespace std;


Autonomous::Autonomous(OperatorInputs *inputs, GyroDrive *gyrodrive)
{
    m_inputs = inputs;
    m_gyrodrive = gyrodrive;
    m_stage = 0;
    m_heading = 0.0;
}


Autonomous::~Autonomous()
{
}


void Autonomous::Init()
{
	m_gyrodrive->SetStraightPID(0.04, 0.0012, 0.07);
    m_gyrodrive->SetAnglePID(0.013, 0.0002, 0.045);
    m_stage = 0;
    m_heading = 0.0;
}


void Autonomous::Loop()
{
    switch (m_stage)
    {
    case 0:
        if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 0.0;
            m_stage++;
        }
        else
        if (m_inputs->xBoxDPadLeft(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 90.0;
            m_stage++;
        }
        else
        if (m_inputs->xBoxDPadDown(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 180.0;
            m_stage++;
        }
        else
        if (m_inputs->xBoxDPadRight(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 270.0;
            m_stage++;
        }
        break;

    case 1:
        if (m_gyrodrive->DriveHeading(m_heading))
            m_stage = 0;
        break;

    }

    /*
    switch (m_stage)
    {
    case 0:
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_stage++;
        break;
    case 1:
        if (m_gyrodrive->DriveAngle(90, false))
            m_stage = 0;
        break;
    }
    */
    /*
    switch (m_stage)
    {
    case 0:
        if (m_gyrodrive->DriveStraight(48, 0.5))
            m_stage++;
        break;
    case 1:
        if (m_gyrodrive->DriveAngle(180))
            m_stage++;
        break;
    case 2:
        if (m_gyrodrive->DriveStraight(48, 0.5))
            m_stage++;
        break;
    case 3:
        m_gyrodrive->Drive(0, 0);
        break;
    }
    */
}


void Autonomous::Stop()
{
}