#pragma once

#include "halodi-controller/halodicontroller.h"


namespace halodi_controller
{
    class NativeIMUHandleHolder : public IMUHandle
    {
    public:

        static const unsigned int orientationSize = 4;
        static const unsigned int angularVelocitySize = 3;
        static const unsigned int linearAccelerationSize = 3;


        static const unsigned int size = orientationSize + angularVelocitySize + linearAccelerationSize;

        NativeIMUHandleHolder(double* data_);

        virtual ~NativeIMUHandleHolder();

        void setOrientationQuaternion(double wx, double wy, double wz, double ws);
        void setLinearAcceleration(double xdd, double ydd, double zdd);
        void setAngularVelocity(double wx, double wy, double wz);

        double* c_data();

    private:

        double* data;


    };
}
