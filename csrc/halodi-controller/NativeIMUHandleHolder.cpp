#include "NativeIMUHandleHolder.h"


namespace halodi_controller
{
    NativeIMUHandleHolder::NativeIMUHandleHolder()
    {
    }


    NativeIMUHandleHolder::~NativeIMUHandleHolder()
    {

    }

    void NativeIMUHandleHolder::readStateIntoBuffer(int& index, double* buffer)
    {
        for(int i = 0; i < orientationSize; i++)
        {
            buffer[index++] = quaternion[i];
        }

        for(int i = 0; i < angularVelocitySize; i++)
        {
            buffer[index++] = angularVelocity[i];
        }

        for(int i = 0; i < linearAccelerationSize; i++)
        {
            buffer[index++] = linearAcceleration[i];
        }

    }

    void NativeIMUHandleHolder::writeCommandIntoBuffer(int& index, double* buffer)
    {
        // Nothing to do here
    }

    int NativeIMUHandleHolder::stateSize()
    {
        return orientationSize + angularVelocitySize + linearAccelerationSize;
    }

    int NativeIMUHandleHolder::commandSize()
    {
        return 0;
    }

    void NativeIMUHandleHolder::setOrientationQuaternion(double wx, double wy, double wz, double ws)
    {
        quaternion[0] = wx;
        quaternion[1] = wy;
        quaternion[2] = wz;
        quaternion[3] = ws;
    }

    void NativeIMUHandleHolder::setLinearAcceleration(double xdd, double ydd, double zdd)
    {
        linearAcceleration[0] = xdd;
        linearAcceleration[1] = ydd;
        linearAcceleration[2] = zdd;
    }

    void NativeIMUHandleHolder::setAngularVelocity(double wx, double wy, double wz)
    {
        angularVelocity[0] = wx;
        angularVelocity[1] = wy;
        angularVelocity[2] = wz;
    }
}
