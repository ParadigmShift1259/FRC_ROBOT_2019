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
    if (automode == kAutoPID)
    {
        double P, I, D;
        m_gyrodrive->GetAnglePID(P, I, D);
        SmartDashboard::PutNumber("DB/Slider 0", P);
        SmartDashboard::PutNumber("DB/Slider 1", I);
        SmartDashboard::PutNumber("DB/Slider 2", D);
    }

    m_stage = 0;
    m_startstage = 0;
    m_heading = 0.0;
}


void Autonomous::Loop()
{
    switch (automode)
    {
    case kAutoDefault:
        break;
    
    case kAutoLeft:
        AutoLeft();
        break;
    
    case kAutoLower:
        AutoLower();
        break;
    
    case kAutoRight:
        AutoRight();
        break;
    
    case kAutoPID:
        AutoPID();
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

        m_intake->SetIntakeMode(Intake::kModeHatch);
        m_intake->SetHatchStage(Intake::kHatchCapture);

        m_lifter->MoveBottom();
        m_startstage++;
        break;
    
    case 1:
       	if (Debug) DriverStation::ReportError("AutoStart 1");

        if (m_gyrodrive->DriveStraight(automode == kAutoLower ? -36.0 : -72.0, 0.5, true))
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


void Autonomous::AutoLower()
{
   	if (Debug) DriverStation::ReportError("AutoLower");

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
    double P = SmartDashboard::GetNumber("DB/Slider 0", 0.0);
    double I = SmartDashboard::GetNumber("DB/Slider 1", 0.0);
    double D = SmartDashboard::GetNumber("DB/Slider 2", 0.0);
    m_gyrodrive->SetAnglePID(P, I, D);
    bool change = false;
 
    if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
    {
        m_heading = 0.0;
        change = true;        
    }
    else
    if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
    {
        m_heading = 30.0;
        change = true;
    }
    else
    if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
    {
        m_heading = 60.0;
        change = true;
    }
    else
    if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
    {
        m_heading = 90.0;
        change = true;
    }

    m_gyrodrive->DriveManualAngle(m_heading, change);

}
