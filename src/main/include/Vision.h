/**
 *  GyroDrive.h
 *  Date:
 *  Last Edited By:
 */


#ifndef SRC_VISION_H_
#define SRC_VISION_H_


#include <frc\WPILib.h>
#include <ctre\Phoenix.h>


using namespace frc;
using namespace std;


class Vision
{
public:
	Vision();
	~Vision();
	void Init();
	void Loop();
	void Stop();

    bool GetRetro(double &angle, double &distance);
    void Pause(bool pause) { m_pause = pause; }

protected:
    shared_ptr<NetworkTable> m_nettable;
    Timer m_timer;

    int m_counter;
    bool m_visionvalid;
    int m_getretrocounter;

    double m_retroangle;
    double m_retrodistance;
    int m_retroquality;

    bool m_pause;
};


#endif /* SRC_Vision_H_ */
