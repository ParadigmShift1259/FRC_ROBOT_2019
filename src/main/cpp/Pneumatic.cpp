/**
 *  Pneumatic.cpp
 *  Date:
 *  Last Edited By:
 */


#include "Pneumatic.h"
#include "Const.h"
#include <cmath>


using namespace std;


Pneumatic::Pneumatic(DriverStation *ds, OperatorInputs *inputs)
{
     m_ds = ds;
     m_inputs = inputs;

     m_solenoid1 = nullptr;
     m_solenoid2 = nullptr;
     m_stage = kOpen;
     m_waittime = PCM_PNEUMATIC_WAIT;

     if ((PCM_MODULE != -1) && (PCM_PNEUMATIC_SOLENOID1 != -1) && (PCM_PNEUMATIC_SOLENOID2 != -1))
     {
        m_compressor = new Compressor(PCM_MODULE);
        m_solenoid1 = new Solenoid(PCM_MODULE, PCM_PNEUMATIC_SOLENOID1);
        m_solenoid2 = new Solenoid(PCM_MODULE, PCM_PNEUMATIC_SOLENOID2);
    }
}


Pneumatic::~Pneumatic()
{
    if (m_solenoid1 != nullptr)
        delete m_solenoid1;
    if (m_solenoid2 != nullptr)
        delete m_solenoid2;
    if (m_compressor != nullptr)
        delete m_compressor;
}


void Pneumatic::Init()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr))
        return;

    if (m_compressor != nullptr)
        m_compressor -> Start();

    m_solenoid1 -> Set(false);
    m_solenoid2 -> Set(false);
    m_stage = kOpen;
    m_waittime = PCM_PNEUMATIC_WAIT;
    m_timer.Start();
    m_timer.Reset();
}


void Pneumatic::Loop()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr))
        return;

    switch (m_stage)
    {
    case kOpen:
        m_solenoid1->Set(false);
        m_solenoid2->Set(false);
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_solenoid1 -> Set(true);
            m_solenoid2 -> Set(true);
            m_timer.Reset();
            m_stage = kClose;
        }
        else
        if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_waittime += .05;
        }
        else
        if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL) && (m_waittime > PCM_PNEUMATIC_WAIT))
        {
            m_waittime -= .05;
        }
        break;

    case kClose:
        if (m_timer.Get() > m_waittime)
        {
            m_solenoid1 -> Set(false);
            m_solenoid2 -> Set(false);
            m_stage = kOpen;
        }
        break;
    }

	SmartDashboard::PutNumber("PN_Stage", m_stage);
	SmartDashboard::PutNumber("PN_Waittime", m_waittime);
}


void Pneumatic::Stop()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr))
        return;
    
    m_solenoid1->Set(false);
    m_solenoid2->Set(false);
}
