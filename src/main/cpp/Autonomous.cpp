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
    m_visioning = kIdle;
    m_visiontype = kVisionStation;

    m_nettable = NetworkTableInstance::GetDefault().GetTable("OpenCV");
    m_counter = 0;
    m_visionvalid = false;
    m_hashatch = false;
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

    m_counter = 0;
    m_visionvalid = false;
    m_hashatch = false;
    m_visioning = kIdle;
    m_visiontype = kVisionStation;
    m_visiontimer.Reset();
    m_visiontimer.Start();
}


void Autonomous::Loop()
{
    AutoVision();

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


void Autonomous::AutoVision()
{
    int counter = m_nettable->GetNumber("visioncounter", 0);

    /*
    if (m_intake->GetIntakeMode() == Intake::kModeCargo && !m_intake->HasCargoHatch())
        m_visiontype = kVisionCargo;
    else if (m_intake->GetIntakeMode() == Intake::kModeHatch && !m_intake->HasCargoHatch())
        m_visiontype = kVisionHatch;
    else
    */
    m_visiontype = kVisionStation;
    
    double angle = 0;       // wanted angle from the network table
    double distance = 0;    // wanted disatnce from the network table
    double quality = 0;     // quality of the network table - 0 is bad, 1 is sketchy, 2 is good

    switch (m_visiontype)   // setting angle, distance, and quality based on bot conditions
    {
    case kVisionCargo:
        angle = m_nettable->GetNumber("CargoAngle", 0) * -1;
        distance = m_nettable->GetNumber("CargoDistance", 0);
        quality = m_nettable->GetNumber("CargoQuality", 0);
        break;
    
    case kVisionHatch:
        angle = m_nettable->GetNumber("HatchAngle", 0) * -1;
        distance = m_nettable->GetNumber("HatchDistance", 0);
        quality = m_nettable->GetNumber("HatchQuality", 0);
        break;
    
    case kVisionStation:
        angle = m_nettable->GetNumber("RetroAngle", 0) * -1;
        distance = m_nettable->GetNumber("RetroDistance", 0);
        quality = m_nettable->GetNumber("RetroQuality", 0);
        break;
    }

    double scale = distance / (96 * 2) + 0.25;  // scale used for converting distance to motor speed

    if (counter > m_counter)                    // if rio is receiving counter, then vision is valid
    {
        m_counter = counter;
        if (distance > 0.0)
        {
            m_visiontimer.Reset();
            m_visionvalid = true;
        }
    }
    else
    if (m_visiontimer.Get() > 0.5)              // if pi does not count, vision is not valid
    {
        m_visionvalid = false;
    }

    switch (m_visioning)
    {
    case kIdle:
        if (m_visionvalid && (quality > 1) && m_inputs->xBoxR3(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))      // starts the vision sequence
        {
            m_gyrodrive->PIDInit();                                                            // enables PID
            m_gyrodrive->EnablePID();

            if (m_intake->GetIntakeMode() == Intake::kModeHatch)                            // starts or maintains sucking of hatch
                m_intake->SetHatchStage(Intake::kHatchCapture);
            else
            if (m_intake->GetIntakeMode() == Intake::kModeCargo && m_intake->HasCargoHatch())   // maintains hold of ball if there is ball
                m_intake->SetCargoStage(Intake::kCargoBall);
            else
            if (m_intake->GetIntakeMode() == Intake::kModeCargo)                            // starts ingest if there is no ball
                m_intake->SetCargoStage(Intake::kCargoIngest);
            else
            if (m_intake->GetIntakeMode() == Intake::kModeNone)                             // prevents vision if no mode is selected
                m_visionvalid = false;

            m_visioning = kVision;
        }
        else
            m_gyrodrive->DisablePID();
        
        m_counter = 0;
        break;
    
    case kVision:
        if (!m_visionvalid || m_inputs->xBoxR3(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))                      // stops the vision sequence
        {
            m_gyrodrive->DisablePID();

        if (m_intake->GetIntakeMode() == Intake::kModeHatch && m_hashatch)                  // if there is a hatch, then eject
        {
            m_intake->SetHatchStage(Intake::kHatchRelease);
            m_hashatch = false;
        }

        if (m_intake->GetHatchIntake() == Intake::kHatchCapture && !m_hashatch)             // if there isn't a hatch, say that we have a hatch
            m_hashatch = true;
    
            m_visioning = kIdle;
        }
        else
        if (m_lifter->IsBottom())                                                           // if lifter is at bottom, start visioning
        {
            double y = -1 * (scale > 1 ? 1 : scale);

            m_gyrodrive->Drive(y, true);
            m_gyrodrive->ResetGyro();
            m_gyrodrive->SetAbsoluteAngle(angle);
        }
        else                                                                                // else, wait for lifter to move to the bottom
        {
            m_lifter->MoveBottom();
        }
        break;
    }

}
