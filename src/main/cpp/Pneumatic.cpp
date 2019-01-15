/**
 *  Pneumatic.cpp
 *  Date:
 *  Last Edited By:
 */


#include <Pneumatic.h>
#include "Const.h"
#include <cmath>


using namespace std;

 Pneumatic::Pneumatic(DriverStation *ds, OperatorInputs *inputs)
 {
     m_ds = ds;
     m_inputs = inputs;
     m_waittime = PCM_WAITTIME;

     m_solenoid1 = nullptr;
     m_solenoid2 = nullptr;
     m_stage = kOpen;

     if (PCM_COMPRESSOR_SOLENOID != -1)
     {
        m_compressor = new Compressor(PCM_COMPRESSOR_SOLENOID);
        m_solenoid1 = new Solenoid(PCM_COMPRESSOR_SOLENOID, 0); //ask about solenoid as well as module numbers
        m_solenoid2 = new Solenoid(PCM_COMPRESSOR_SOLENOID, 1);
    }

 }

Pneumatic::~Pneumatic()
{
    if (m_solenoid1 != nullptr)
        delete m_solenoid1; //ask about delete compressor
    if (m_solenoid2 != nullptr)
        delete m_solenoid2;
    if (m_compressor != nullptr)
        delete m_compressor;
}

void Pneumatic::Init()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr))
        return;

    m_stage = kOpen;
    m_solenoid1 -> Set(false);
    m_solenoid2 -> Set(false);

    if (m_compressor != nullptr)
        m_compressor -> Start();
}

void Pneumatic::Loop()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr))
        return;


    switch (m_stage)
    {
        case kOpen:
            m_solenoid1 -> Set(false);
            m_solenoid2 -> Set(false);
            if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0))
            {
                m_solenoid1 -> Set(true);
                m_solenoid2 -> Set(true);
                m_stage = kClose;
            }
            if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0))
            {
                m_waittime += 50;
            }
            if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0) && m_waittime != 50)
            {
                m_waittime -= 50;
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
    
    m_solenoid1 -> Set(false);
    m_solenoid2 -> Set(false);
}
