#pragma once

#include <string>
#include <memory>
#include <vector>
#include "halodi-controller/halodicontroller.h"
#include "jvmLauncher/javavirtualmachine.h"
#include "halodicontrollerconfigurationloader.h"


namespace  halodi_controller {

class HalodiControllerImplementation : public HalodiController
{
public:
    HalodiControllerImplementation(std::string controllerName_, std::string working_directory_);

    static void standardOuputCallback(JNIEnv* env, jobject caller, jlong ptr, jboolean stderr, jstring data);

    std::shared_ptr<JointHandle> addJoint(std::string name);
    std::shared_ptr<IMUHandle> addIMU(std::string parentLink, std::string name);
    std::shared_ptr<ForceTorqueSensorHandle> addForceTorqueSensor(std::string parentLink, std::string name);

    bool initialize(std::string arguments);

    bool start();
    bool update(long long timeInNanoseconds, long long duration);
    bool stop();

    std::string getControllerDescription();
    std::string getVirtualMachineConfiguration();


    void attachCurrentThread();
    void deattachCurrentThread();

    std::string callController(std::string request, std::string arguments);
    std::shared_ptr<SharedBuffer> createSharedBuffer(std::string name, int32_t size);

    void setOutputHandler(std::function<void(bool, std::string)> handler);

    virtual ~HalodiControllerImplementation();

private:
    HalodiControllerConfigurationLoader configurationLoader;

    std::shared_ptr<JavaVirtualMachine> vm;

    std::shared_ptr<JavaObject> bridge;

    std::shared_ptr<JavaMethod> jAddJoint;
    std::shared_ptr<JavaMethod> jAddIMU;
    std::shared_ptr<JavaMethod> jAddForceTorqueSensor;
    std::shared_ptr<JavaMethod> jInitialize;
    std::shared_ptr<JavaMethod> jStart;
    std::shared_ptr<JavaMethod> jUpdate;
    std::shared_ptr<JavaMethod> jStop;
    std::shared_ptr<JavaMethod> jShutdown;

    std::shared_ptr<JavaMethod> jGetControllerDescription;
    std::shared_ptr<JavaMethod> jCallController;
    std::shared_ptr<JavaMethod> jCreateSharedBuffer;


    std::function<void(bool, std::string)> outputHandler;

    void standardOutputHandler(bool stdErr, std::string message);
};


}
