#include "NativeEffortJointHandlerHolder.h"


namespace halodi_controller
{

    NativeEffortJointHandleHolder::NativeEffortJointHandleHolder(double *data_) :
        data(data_)
    {

    }

    NativeEffortJointHandleHolder::~NativeEffortJointHandleHolder()
    {

    }

    void NativeEffortJointHandleHolder::setPosition(double position)
    {
        data[0] = position;
    }

    void NativeEffortJointHandleHolder::setVelocity(double velocity)
    {
        data[1] = velocity;
    }

    void NativeEffortJointHandleHolder::setMeasuredEffort(double effort)
    {
        data[2] = effort;
    }

    double NativeEffortJointHandleHolder::getDesiredEffort()
    {
        return data[3];
    }

    double NativeEffortJointHandleHolder::getDampingScale()
    {
        return data[4];
    }

    double NativeEffortJointHandleHolder::getInitialAngle()
    {
        return data[5];
    }

    double *NativeEffortJointHandleHolder::c_data()
    {
        return data;
    }



}
