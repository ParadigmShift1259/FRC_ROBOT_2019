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
    m_solenoid3 = nullptr;
    m_solenoid4 = nullptr;
    m_spark1 = nullptr;
    m_spark2 = nullptr;
    m_spark3 = nullptr;
    m_spark4 = nullptr;
    m_stage = kOpen;
    m_waittime = PCM_PNEUMATIC_WAIT;

    if ((PCM_MODULE != -1) && (PCM_PNEUMATIC_SOLENOID1 != -1) && (PCM_PNEUMATIC_SOLENOID2 != -1))
    {
        m_compressor = new Compressor(PCM_MODULE);
        m_solenoid1 = new Solenoid(PCM_MODULE, PCM_PNEUMATIC_SOLENOID1);
        m_solenoid2 = new Solenoid(PCM_MODULE, PCM_PNEUMATIC_SOLENOID2);
        m_solenoid3 = new Solenoid(PCM_MODULE, PCM_PNEUMATIC_SOLENOID3);
        m_solenoid4 = new Solenoid(PCM_MODULE, PCM_PNEUMATIC_SOLENOID4);
    }

    m_spark1 = new Spark(0);
    m_spark2 = new Spark(1);
    m_spark3 = new Spark(2);
    m_spark4 = new Spark(3);
}


Pneumatic::~Pneumatic()
{
    if (m_solenoid1 != nullptr)
        delete m_solenoid1;
    if (m_solenoid2 != nullptr)
        delete m_solenoid2;
    if (m_solenoid3 != nullptr)
        delete m_solenoid3;
    if (m_solenoid4 != nullptr)
        delete m_solenoid4;
    if (m_compressor != nullptr)
        delete m_compressor;
    if (m_spark1 != nullptr)
        delete m_spark1;
    if (m_spark2 != nullptr)
        delete m_spark2;
    if (m_spark3 != nullptr)
        delete m_spark3;
    if (m_spark4 != nullptr)
        delete m_spark4;
      
}


void Pneumatic::Init()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr || m_spark3 == nullptr || m_spark4 == nullptr)
        return;

    if (m_compressor != nullptr)
        m_compressor -> Start();

    m_solenoid1 -> Set(false);
    m_solenoid2 -> Set(false);
    m_solenoid3 -> Set(false);
    m_solenoid4 -> Set(false);
    m_spark1->Set(0);
    m_spark2->Set(0);
    m_spark3->Set(0);
    m_spark4->Set(0);
    m_stage = kOpen;
    m_waittime = PCM_PNEUMATIC_WAIT;
    m_timer.Start();
    m_timer.Reset();
}


void Pneumatic::Loop()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr || m_spark3 == nullptr || m_spark4 == nullptr)
        return;

    switch (m_stage)
    {
    case kOpen:
    	DriverStation::ReportError("kOpen");

        m_solenoid1->Set(false);
        m_solenoid2->Set(false);
        m_solenoid3->Set(false);
        m_solenoid4->Set(false);
        m_spark1->Set(0);
        m_spark2->Set(0);
        m_spark3->Set(0);
        m_spark4->Set(0);
        if (m_inputs->xBoxAButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_stage = kCapture;
        }
        break;

    case kCapture:
    	DriverStation::ReportError("kCapture");

        m_solenoid1->Set(false);
        m_solenoid2->Set(false);
        m_solenoid3->Set(false);
        m_solenoid4->Set(false);
        m_spark1->Set(0.3);
        m_spark2->Set(0.3);
        m_spark3->Set(0.3);
        m_spark4->Set(0.3);
        if (m_inputs->xBoxBButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        {
            m_solenoid1 -> Set(true);
            m_solenoid2 -> Set(true);
            m_solenoid3 -> Set(true);
            m_solenoid4 -> Set(true);
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
    	DriverStation::ReportError("kClose");

        m_spark1->Set(0);
        m_spark2->Set(0);
        m_spark3->Set(0);
        m_spark4->Set(0);
        if (m_timer.Get() > m_waittime)
        {
            m_solenoid1 -> Set(false);
            m_solenoid2 -> Set(false);
            m_solenoid3 -> Set(false);
            m_solenoid4 -> Set(false);
            m_stage = kOpen;
        }
        break;
    }

	SmartDashboard::PutNumber("PN_Stage", m_stage);
	SmartDashboard::PutNumber("PN_Waittime", m_waittime);
}


void Pneumatic::Stop()
{
    if ((m_solenoid1 == nullptr) || (m_solenoid2 == nullptr) || (m_solenoid3 == nullptr) || (m_solenoid4 == nullptr))
        return;
    if (m_spark1 == nullptr || m_spark2 == nullptr || m_spark3 == nullptr || m_spark4 == nullptr)
        return;
    
    m_solenoid1->Set(false);
    m_solenoid2->Set(false);
    m_solenoid3->Set(false);
    m_solenoid4->Set(false);
    m_spark1->Set(0);
    m_spark2->Set(0);
    m_spark3->Set(0);
    m_spark4->Set(0);
}
