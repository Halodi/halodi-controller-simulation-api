#pragma once

#include "halodi-controller/halodicontroller.h"

namespace halodi_controller
{
    class NativeEffortJointHandleHolder : public JointHandle
    {
    public:
        static const unsigned int size = 5;

        NativeEffortJointHandleHolder(double* data_, double initialAngle_);

        virtual ~NativeEffortJointHandleHolder();



       void setPosition(double position);

       void setVelocity(double velocity);

       void setMeasuredEffort(double effort);

       double getDesiredEffort();

       double getDampingScale();

       double getInitialAngle();

    private:

        double* data;

        double initialAngle;
    };
}
