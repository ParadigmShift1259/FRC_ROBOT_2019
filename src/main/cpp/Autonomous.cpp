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
    m_side = 1;
}


Autonomous::~Autonomous()
{
}


void Autonomous::Init()
{
	m_gyrodrive->SetStraightPID(0.04, 0.0012, 0.07);
    m_gyrodrive->SetAnglePID(0.0028, 0.0001, 0.06);//(0.01, 0.002, 0.045);
    m_stage = 0;
    m_heading = 0.0;
    m_side = 1;
}


void Autonomous::Loop()
{
    bool reached_heading;

    switch (m_stage)
    {
    case 0:
        if (m_inputs->xBoxDPadUp(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 0.0;
//            m_stage++;
        }
        else
        if (m_inputs->xBoxDPadLeft(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 90.0;
//            m_stage++;
        }
        else
        if (m_inputs->xBoxDPadDown(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 180.0;
//            m_stage++;
        }
        else
        if (m_inputs->xBoxDPadRight(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 270.0;            
 //           m_stage++;
        }
        else if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_gyrodrive->AdjAnglePID(.0001, 0, 0);
        else if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_gyrodrive->AdjAnglePID(-.0001, 0, 0);
        else if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_gyrodrive->AdjAnglePID(0, .00001, 0);
        else if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_gyrodrive->AdjAnglePID(0, -.00001, 0);
        else if (m_inputs->xBoxRightBumper(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_gyrodrive->AdjAnglePID(0, 0, .001);
        else if (m_inputs->xBoxRightTrigger(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
            m_gyrodrive->AdjAnglePID(0, 0, -.001);
            
        reached_heading = m_gyrodrive->DriveHeading(m_heading);
        break;

    case 1:
        if (m_gyrodrive->DriveHeading(m_heading))
            m_stage = 0;
        break;

 
    }
   SmartDashboard::PutNumber("Auto01_heading", m_heading);
    SmartDashboard::PutNumber("Auto02_reachedheading", reached_heading);
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

    if (side == kLeft)
        m_side = -1;
    if (side == kRight)
        m_side = 1;

   switch (direction)
   {
    case kNothing:
        break;
    case kCargoLoading:
        if (m_gyrodrive->DriveHeading(180))
            direction = kNothing;
        break;
    case kCargoShip:
        if (m_gyrodrive->DriveHeading(90 * m_side))
            direction = kNothing;
        break;
    case kRocketClose:
        if (m_gyrodrive->DriveHeading(-30 * m_side))
            direction = kNothing;
        break;
    case kRocketMedium:
        if (m_gyrodrive->DriveHeading(-90 * m_side))
            direction = kNothing;
        break;
    case kRocketFar:
        if (m_gyrodrive->DriveHeading(-150 * m_side))
            direction = kNothing;
        break;
   }
}


void Autonomous::Stop()
{
}