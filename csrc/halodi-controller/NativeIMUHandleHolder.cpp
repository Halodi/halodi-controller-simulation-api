#include "NativeIMUHandleHolder.h"


namespace halodi_controller
{
    NativeIMUHandleHolder::NativeIMUHandleHolder(double *data_) :
        data(data_)
    {
    }


    NativeIMUHandleHolder::~NativeIMUHandleHolder()
    {

    }

    void NativeIMUHandleHolder::setOrientationQuaternion(double wx, double wy, double wz, double ws)
    {
        data[0] = wx;
        data[1] = wy;
        data[2] = wz;
        data[3] = ws;
    }

    void NativeIMUHandleHolder::setLinearAcceleration(double xdd, double ydd, double zdd)
    {
        data[4] = xdd;
        data[5] = ydd;
        data[6] = zdd;
    }

    void NativeIMUHandleHolder::setAngularVelocity(double wx, double wy, double wz)
    {
        data[7] = wx;
        data[8] = wy;
        data[9] = wz;
    }
}
