/**
 *  Vision.cpp
 *  Date:
 *  Last Edited By:
 */


#include "Vision.h"
#include "Const.h"
//#include <networktables\NetworkTable.h>
#include <networktables\NetworkTableInstance.h>


using namespace std;
using namespace nt;


Vision::Vision()
{
    m_nettable = NetworkTableInstance::GetDefault().GetTable("OpenCV");

    m_counter = 0;
    m_visionvalid = false;

    m_retroangle = 0.0;
    m_retrodistance = 0.0;
    m_retroquality = 0;
}


Vision::~Vision()
{

}


void Vision::Init()
{
    m_counter = 0;
    m_visionvalid = false;
    m_timer.Reset();
    m_timer.Start();

    m_retroangle = 0.0;
    m_retrodistance = 0.0;
    m_retroquality = 0;
}


void Vision::Loop()
{
    int counter = m_nettable->GetNumber("visioncounter", 0);

    if (counter > m_counter)
    {
        m_counter = counter;
        m_timer.Reset();
        m_visionvalid = true;
    }
    else
    if (m_timer.Get() > 0.5)
        m_visionvalid = false;

    if (m_visionvalid)
    {
        m_retroangle = m_nettable->GetNumber("RetroAngle", 0.0) * -1.0;
        m_retrodistance = m_nettable->GetNumber("RetroDisatnce", 0.0);
        m_retroquality = m_nettable->GetNumber("RetroQuality", 0.0);
    }
}


void Vision::Stop()
{
}


bool Vision::GetRetro(double &angle, double &distance)
{
    if (m_visionvalid && m_retroquality > 1)      // if vision is valid and retro quality is green, give values
    {
        angle = m_retroangle;
        distance = m_retrodistance;
        return true;
    }
    return false;
}