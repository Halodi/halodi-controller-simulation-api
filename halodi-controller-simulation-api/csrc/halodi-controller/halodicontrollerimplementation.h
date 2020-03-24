#pragma once

#include <string>
#include <memory>
#include <vector>
#include "halodi-controller/halodicontroller.h"
#include "jvmLauncher/javavirtualmachine.h"

namespace  halodi_controller {

class HalodiControllerImplementation : public HalodiController
{
public:
    HalodiControllerImplementation(ControllerConfiguration &configuration);

    std::shared_ptr<JointHandle> addJoint(std::string name);
    std::shared_ptr<IMUHandle> addIMU(std::string parentLink, std::string name);
    std::shared_ptr<ForceTorqueSensorHandle> addForceTorqueSensor(std::string parentLink, std::string name);

    bool initialize();
    void update(long long timeInNanoseconds, long long duration);
    void reset();

    void attachCurrentThread();
    void deattachCurrentThread();


    virtual ~HalodiControllerImplementation();

private:
    std::shared_ptr<JavaVirtualMachine> vm;

    std::shared_ptr<JavaObject> bridge;

    std::shared_ptr<JavaMethod> jAddJoint;
    std::shared_ptr<JavaMethod> jAddIMU;
    std::shared_ptr<JavaMethod> jAddForceTorqueSensor;
    std::shared_ptr<JavaMethod> jInitialize;
    std::shared_ptr<JavaMethod> jUpdate;
    std::shared_ptr<JavaMethod> jReset;

};


}
