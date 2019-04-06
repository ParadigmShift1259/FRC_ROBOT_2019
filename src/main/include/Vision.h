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

protected:
    shared_ptr<NetworkTable> m_nettable;
    Timer m_timer;

    bool m_visionvalid;
    int m_prevcounter;
    int m_counter;

    double m_prevretroangle;
    double m_retroangle;
    double m_retrodistance;
    int m_retroquality;
};


#endif /* SRC_Vision_H_ */
