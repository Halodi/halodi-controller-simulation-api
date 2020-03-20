#include "NativeEffortJointHandlerHolder.h"


namespace halodi_controller
{
    NativeEffortJointHandleHolder::NativeEffortJointHandleHolder()
    {
    }

    NativeEffortJointHandleHolder::~NativeEffortJointHandleHolder()
    {

    }

    void NativeEffortJointHandleHolder::readStateIntoBuffer(int& index, double* buffer)
    {
        buffer[index++] = measuredEffort;
        buffer[index++] = position;
        buffer[index++] = velocity;
    }

    int NativeEffortJointHandleHolder::stateSize()
    {
        return 3;
    }

    int NativeEffortJointHandleHolder::commandSize()
    {
        return 1;
    }

    void NativeEffortJointHandleHolder::setPosition(double position)
    {
        this->position = position;
    }

    void NativeEffortJointHandleHolder::setVelocity(double velocity)
    {
        this->velocity = velocity;
    }

    void NativeEffortJointHandleHolder::setMeasuredEffort(double effort)
    {
        this->measuredEffort = effort;
    }

    double NativeEffortJointHandleHolder::getDesiredEffort()
    {
        return this->desiredEffort;
    }

    double NativeEffortJointHandleHolder::getDampingScale()
    {
        return this->dampingScale;
    }

    void NativeEffortJointHandleHolder::writeCommandIntoBuffer(int &index, double *buffer)
    {
        desiredEffort = buffer[index++];
        dampingScale = buffer[index++];
    }
}
