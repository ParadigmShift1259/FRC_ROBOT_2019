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

    m_visionvalid = false;
    m_prevcounter = 0;
    m_counter = 0;

    m_prevretroangle = 0.0;
    m_retroangle = 0.0;
    m_retrodistance = 0.0;
    m_retroquality = 0;
}


Vision::~Vision()
{

}


void Vision::Init()
{
    m_timer.Reset();
    m_timer.Start();

    m_visionvalid = false;
    m_prevcounter = 0;
    m_counter = 0;

    m_prevretroangle = 0.0;
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
        m_prevretroangle = m_retroangle;
        m_retroangle = m_nettable->GetNumber("RetroAngle", 0.0) * -1.0;
        m_retrodistance = m_nettable->GetNumber("RetroDistance", 0.0);
        m_retroquality = m_nettable->GetNumber("RetroQuality", 0.0);
    }

    SmartDashboard::PutNumber("VI_01angle", m_retroangle);
    SmartDashboard::PutNumber("VI_02distance", m_retrodistance);
    SmartDashboard::PutNumber("VI_03quality", m_retroquality);
}


void Vision::Stop()
{
}


bool Vision::GetRetro(double &angle, double &distance)
{
    if (m_visionvalid &&                    // if vision is valid
        (m_counter > m_prevcounter) &&      // if counter incrementing
        (m_retroquality > 1))               // if retro quality is green
    {
        // difference between previous and current angle has to be reasonable
        if (fabs(fabs(m_retroangle) - fabs(m_prevretroangle)) < 25)
        {
            angle = m_retroangle;
            distance = m_retrodistance;
            m_prevcounter = m_counter;
            return true;
        }
    }
    return false;
}


/*
        //double newangle = 22.19332 - 0.450045 * distance + 0.002948177 * distance * distance;
        //angle -= newangle;

        // modify angle due to camera being off center
        //angle += distance * 0.3939052 - 19.79458;

        if (distance > 40) angle -=4;
        if ((distance > 30) && (distance <= 40)) angle -=8;
        if ((distance > 20) && (distance <= 30)) angle -=12;
        if ((distance > 12) && (distance <= 20)) angle -=15;
*/
