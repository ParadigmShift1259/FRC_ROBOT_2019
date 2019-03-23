/**
 *  Climber.cpp
 *  Date:
 *  Last Edited By:
 */


#include "Climber.h"
#include "Const.h"


using namespace std;


Climber::Climber(OperatorInputs *inputs)
{
    m_inputs = inputs;

    m_solenoid = nullptr;
    if (PCM_CLIMBER_SOLENOID != -1)
        m_solenoid = new Solenoid(PCM_CLIMBER_SOLENOID);
}


Climber::~Climber()
{
    if (m_solenoid != nullptr)
        delete m_solenoid;
}


void Climber::Init()
{
    if (m_solenoid == nullptr)
        return;
    m_solenoid->Set(false);
}


void Climber::Loop()
{
    if (m_solenoid == nullptr)
        return;

    if (m_inputs->xBoxYButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        m_solenoid->Set(true);
    else
    if (m_inputs->xBoxXButton(OperatorInputs::ToggleChoice::kToggle, 0 * INP_DUAL))
        m_solenoid->Set(false);
}


void Climber::Stop()
{
    if (m_solenoid == nullptr)
        return;
    
    m_solenoid->Set(false);
}
