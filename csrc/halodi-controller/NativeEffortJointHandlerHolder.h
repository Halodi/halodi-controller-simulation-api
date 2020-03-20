#pragma once

#include "halodi-controller/halodicontroller.h"
#include "NativeUpdateableInterface.h"


namespace halodi_controller
{
    class NativeEffortJointHandleHolder : public NativeUpdateableInterface, JointHandle
    {
    public:
        NativeEffortJointHandleHolder();

        virtual ~NativeEffortJointHandleHolder();

        void readStateIntoBuffer(int& index, double* buffer);
        virtual void writeCommandIntoBuffer(int& index, double* buffer);

        int stateSize();
        virtual int commandSize();




       void setPosition(double position);

       void setVelocity(double velocity);

       void setMeasuredEffort(double effort);

       double getDesiredEffort();

       double getDampingScale();

    private:
        double position, velocity, measuredEffort;
        double desiredEffort, dampingScale;
    };
}
