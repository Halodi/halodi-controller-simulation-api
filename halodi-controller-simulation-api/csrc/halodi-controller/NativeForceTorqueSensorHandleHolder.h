#pragma once

#include "halodi-controller/halodicontroller.h"

namespace halodi_controller
{


    class NativeForceTorqueSensorHandleHolder : public ForceTorqueSensorHandle
    {
    public:
        static const unsigned int forceLength = 3;
        static const unsigned int torqueLength = 3;
        static const unsigned int size = forceLength + torqueLength;

        NativeForceTorqueSensorHandleHolder(double* data_);

        virtual ~NativeForceTorqueSensorHandleHolder();


        void setForce(double fx, double fy, double fz);
        void setTorque(double tx, double ty, double tz);

        double* c_data();

    private:
        double* data;
    };
}
