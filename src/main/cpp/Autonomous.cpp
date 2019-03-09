/**
 *  Autonomous.cpp
 *  Date:
 *  Last Edited By:
 */


#include "Autonomous.h"
#include "Const.h"


using namespace std;


Autonomous::Autonomous(OperatorInputs *inputs, GyroDrive *gyrodrive, Lifter *lifter, Intake *intake)
{
    m_inputs = inputs;
    m_gyrodrive = gyrodrive;
    m_lifter = lifter;
    m_intake = intake;
    m_stage = 0;
    m_startstage = 0;
    m_heading = 0.0;
}


Autonomous::~Autonomous()
{
}


void Autonomous::Init()
{
/*
	m_gyrodrive->SetStraightPID(0.1, 0.0003, 0.11);    // 2018 Values (0.04, 0.0012, 0.07)
    m_gyrodrive->SetAnglePID(0.1, 0.0003, 0.11);        // Tuned 3/2/19 (0.1, 0.0003, 0.11)
    SmartDashboard::PutNumber("DB/Slider 0", 0.1);
    SmartDashboard::PutNumber("DB/Slider 1", 0.0003);
    SmartDashboard::PutNumber("DB/Slider 2", 0.11);
*/
    m_stage = 0;
    m_startstage = 0;
    m_heading = 0.0;
}


void Autonomous::Loop()
{
    switch (automode)
    {
    case kAutoDefault:
        // AutoPID();
        break;
    
    case kAutoLeft:
        AutoLeft();
        break;
    
    case kAutoCenter:
        AutoCenter();
        break;
    
    case kAutoRight:
        AutoRight();
        break;
    }
}


void Autonomous::Stop()
{
    m_gyrodrive->Stop();
}


bool Autonomous::StartSequence()
{
    bool result = false;

    switch (m_startstage)
    {
    case 0:
       	if (Debug) DriverStation::ReportError("AutoStart 0");

        m_intake->SetHatchVac(Intake::kVacOn);
//        m_intake->SetCargoIntake(Intake::kCargoDown);
        m_lifter->MoveBottom();
        m_startstage++;
        break;
    
    case 1:
       	if (Debug) DriverStation::ReportError("AutoStart 1");

        if (m_gyrodrive->DriveStraight(-72.0, 0.5, false))
            m_startstage++;
        break;

    case 2:
       	if (Debug) DriverStation::ReportError("AutoStart 2");

        if (m_lifter->MoveBottom())
            m_startstage++;
        break;

    case 3:
       	if (Debug) DriverStation::ReportError("AutoStart 3");

        m_lifter->SetLifter(Lifter::kForward);
        result = true;
        break;
    }

    return result;
}


void Autonomous::AutoLeft()
{
   	if (Debug) DriverStation::ReportError("AutoLeft");

    switch (m_stage)
    {
    case 0:
        if (StartSequence())
            m_stage++;
        break;

    case 1:
        break;
    }
}


void Autonomous::AutoCenter()
{
   	if (Debug) DriverStation::ReportError("AutoCenter");

    switch (m_stage)
    {
    case 0:
        if (StartSequence())
            m_stage++;
        break;

    case 1:
        break;
    }
}


void Autonomous::AutoRight()
{
   	if (Debug) DriverStation::ReportError("AutoRight");

    switch (m_stage)
    {
    case 0:
        if (StartSequence())
            m_stage++;
        break;

    case 1:
        break;
    }
}


void Autonomous::AutoPID()
{
/*
    double P = SmartDashboard::GetNumber("DB/Slider 0", 0.0);
    double I = SmartDashboard::GetNumber("DB/Slider 1", 0.0);
    double D = SmartDashboard::GetNumber("DB/Slider 2", 0.0);
    m_gyrodrive->SetAnglePID(P, I, D);
 
    switch (m_stage)
    {
    case 0:
        if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 0.0;
            m_stage++;
        }
        else
        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 30.0;
            m_stage++;
        }
        else
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 60.0;
            m_stage++;
        }
        else
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_heading = 90.0;
            m_stage++;
        }
        break;

    case 1:
        if (m_gyrodrive->DriveHeading(m_heading))
            m_stage = 0;
        break;

    }
*/
/*
    double P = SmartDashboard::GetNumber("DB/Slider 0", 0.0);
    double I = SmartDashboard::GetNumber("DB/Slider 1", 0.0);
    double D = SmartDashboard::GetNumber("DB/Slider 2", 0.0);
    m_gyrodrive->SetStraightPID(P, I, D);

    switch (m_stage)
    {
    case 0:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 1:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 2:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 3:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 4:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 5:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 6:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 7:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 8:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 9:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 10:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 11:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 12:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 13:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 14:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 15:
        if (m_gyrodrive->DriveAngle(45))
            m_stage++;
        break;
    case 16:
        if (m_gyrodrive->DriveStraight(12, 0.5))
            m_stage++;
        break;
    case 17:
        m_gyrodrive->Drive(0, 0);
        break;
    }
*/
}
