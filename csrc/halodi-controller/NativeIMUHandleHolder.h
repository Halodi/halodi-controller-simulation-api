#pragma once

#include "halodi-controller/halodicontroller.h"
#include "NativeUpdateableInterface.h"


namespace halodi_controller
{
    static const unsigned int orientationSize = 4;
    static const unsigned int angularVelocitySize = 3;
    static const unsigned int linearAccelerationSize = 3;

    class NativeIMUHandleHolder : public NativeUpdateableInterface, public IMUHandle
    {
    public:
        NativeIMUHandleHolder();

        virtual ~NativeIMUHandleHolder();

        void readStateIntoBuffer(int& index, double* buffer);
        void writeCommandIntoBuffer(int& index, double* buffer);

        int stateSize();
        int commandSize();


        void setOrientationQuaternion(double wx, double wy, double wz, double ws);
        void setLinearAcceleration(double xdd, double ydd, double zdd);
        void setAngularVelocity(double wx, double wy, double wz);


    private:
        double quaternion[orientationSize];
        double linearAcceleration[angularVelocitySize];
        double angularVelocity[linearAccelerationSize];


    };
}
