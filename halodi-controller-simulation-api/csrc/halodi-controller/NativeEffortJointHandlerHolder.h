#pragma once

#include "halodi-controller/halodicontroller.h"

namespace halodi_controller
{
    class NativeEffortJointHandleHolder : public JointHandle
    {
    public:
        static const unsigned int size = 6;

        NativeEffortJointHandleHolder(double* data_);

        virtual ~NativeEffortJointHandleHolder();



       void setPosition(double position);

       void setVelocity(double velocity);

       void setMeasuredEffort(double effort);

       double getDesiredEffort();

       double getDampingScale();

       double getInitialAngle();

       double* c_data();

    private:

        double* data;

        double initialAngle;
    };
}
