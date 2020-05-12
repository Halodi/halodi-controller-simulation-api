#include "halodicontrollerimplementation.h"
#include <iostream>

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"

#include <stdexcept>

namespace  halodi_controller
{

HalodiControllerImplementation::HalodiControllerImplementation(std::string controllerName_, std::string working_directory_) :
    configurationLoader(controllerName_)
{

    vm = JavaVirtualMachine::startVM(configurationLoader.javaHome, working_directory_, configurationLoader.classPath, configurationLoader.vmArgs);


    std::string mainClass = configurationLoader.mainClass;
    std::shared_ptr<JavaMethod> ctor = vm->getJavaMethod(mainClass, "<init>", "()V");
    bridge = ctor->createObject(jargument);


    jAddJoint = vm->getJavaMethod(mainClass, "createEffortJointHandle", "(Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jAddIMU = vm->getJavaMethod(mainClass, "createIMUHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jAddForceTorqueSensor = vm->getJavaMethod(mainClass, "createForceTorqueSensorHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jInitialize = vm->getJavaMethod(mainClass, "initFromNative", "(Ljava/lang/String;)Z");
    jStart = vm->getJavaMethod(mainClass, "startFromNative", "()Z");
    jUpdate = vm->getJavaMethod(mainClass, "updateFromNative", "(JJ)Z");
    jStop = vm->getJavaMethod(mainClass, "stopFromNative", "()Z");
    jShutdown = vm->getJavaMethod(mainClass, "shutdownFromNative", "()V");
    jGetControllerDescription = vm->getJavaMethod(mainClass, "getControllerDescriptionFromNative", "()Ljava/lang/String;");
}

std::shared_ptr<JointHandle> HalodiControllerImplementation::addJoint(std::string name)
{
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddJoint->callBytebufferMethod(bridge, NativeEffortJointHandleHolder::size * sizeof(double), jName->native());

    return std::make_shared<NativeEffortJointHandleHolder>(buffer);
}

std::shared_ptr<IMUHandle> HalodiControllerImplementation::addIMU(std::string parentLink, std::string name)
{
    std::shared_ptr<JavaString> jParent = vm->createJavaString(parentLink);
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddIMU->callBytebufferMethod(bridge, NativeIMUHandleHolder::size * sizeof(double), jParent->native(), jName->native());


    return std::make_shared<NativeIMUHandleHolder>(buffer);
}

std::shared_ptr<ForceTorqueSensorHandle> HalodiControllerImplementation::addForceTorqueSensor(std::string parentLink, std::string name)
{
    std::shared_ptr<JavaString> jParent = vm->createJavaString(parentLink);
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddForceTorqueSensor->callBytebufferMethod(bridge, NativeForceTorqueSensorHandleHolder::size * sizeof(double), jParent->native(), jName->native());

    return std::make_shared<NativeForceTorqueSensorHandleHolder>(buffer);
}

bool HalodiControllerImplementation::initialize(std::string arguments)
{
    std::shared_ptr<JavaString> jArguments = vm->createJavaString(arguments);
    return jInitialize->callBooleanMethod(bridge, jArguments->native());
}

bool HalodiControllerImplementation::start()
{
    return jStart->callBooleanMethod(bridge);
}

bool HalodiControllerImplementation::update(long long timeInNanoseconds, long long duration)
{
    return jUpdate->callBooleanMethod(bridge, timeInNanoseconds, duration);
}

bool HalodiControllerImplementation::stop()
{
    return jStop->callBooleanMethod(bridge);
}

std::string HalodiControllerImplementation::getControllerDescription()
{
    return jGetControllerDescription->callStringMethod(bridge);
}

std::string HalodiControllerImplementation::getControllerConfiguration()
{
    return configurationLoader.jsonConfiguration;
}

void HalodiControllerImplementation::attachCurrentThread()
{
    vm->attachCurrentThread();
}

void HalodiControllerImplementation::deattachCurrentThread()
{
    vm->detachCurrentThread();
}

HalodiControllerImplementation::~HalodiControllerImplementation()
{
    vm->attachCurrentThread();
    jShutdown->callVoidMethod(bridge);
}

}
