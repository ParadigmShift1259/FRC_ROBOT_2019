/**
 *  GryoDrive.cpp
 *  Date:
 *  Last Edited By:
 */


#include "GyroDrive.h"
#include "Const.h"


using namespace std;


GyroDrive::GyroDrive(OperatorInputs *inputs)
{
    m_inputs = inputs;
    m_drivetrain = new DriveTrain(inputs);
    m_gyro = new DualGyro(CAN_GYRO1, CAN_GYRO2);
    m_drivepid = new DrivePID(m_drivetrain, m_gyro, m_inputs);
}


GyroDrive::~GyroDrive()
{
    if (m_drivepid != nullptr)
        delete m_drivepid;
    if (m_gyro != nullptr)
        delete m_gyro;
    if (m_drivetrain != nullptr)
        delete m_drivetrain;
}


void GyroDrive::Init()
{
    m_drivetrain->Init(DriveTrain::DriveMode::kFollower);
    m_gyro->Init();
}


void GyroDrive::Loop()
{
    m_gyro->Loop();
    if (m_drivepid->GetEnabled())
        m_drivepid->Loop();
    else
        m_drivetrain->Loop();
}


void GyroDrive::Stop()
{
    m_drivetrain->Stop();
    m_drivepid->Stop();
    m_gyro->Stop();
}


void GyroDrive::Disabled()
{
    m_gyro->Loop();
}