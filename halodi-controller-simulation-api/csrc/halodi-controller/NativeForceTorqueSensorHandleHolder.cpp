#include "NativeForceTorqueSensorHandleHolder.h"


namespace halodi_controller
{
    NativeForceTorqueSensorHandleHolder::NativeForceTorqueSensorHandleHolder(double *data_) :
        data(data_)
    {
    }

    NativeForceTorqueSensorHandleHolder::~NativeForceTorqueSensorHandleHolder()
    {

    }

    void NativeForceTorqueSensorHandleHolder::setForce(double fx, double fy, double fz)
    {
        data[0] = fx;
        data[1] = fy;
        data[2] = fz;
    }

    void NativeForceTorqueSensorHandleHolder::setTorque(double tx, double ty, double tz)
    {
        data[3] = tx;
        data[4] = ty;
        data[5] = tz;
    }


}
