#include "halodicontrollerimplementation.h"
#include <iostream>

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"

#include <stdexcept>

namespace  halodi_controller
{

HalodiControllerImplementation::HalodiControllerImplementation(ControllerConfiguration &configuration)

{
    if(configuration.mainClass.empty())
    {
        throw std::runtime_error("Main class not set.");
    }

    if(configuration.classPath.empty())
    {
        throw std::runtime_error("Classpath not set.");
    }

    if(configuration.workingDirectory.empty())
    {
        throw std::runtime_error("Working directory not set.");
    }



    vm = JavaVirtualMachine::startVM(configuration.workingDirectory, configuration.classPath, configuration.vmOptions);


    std::shared_ptr<JavaMethod> ctor = vm->getJavaMethod(configuration.mainClass, "<init>", "()V");
    bridge = ctor->createObject(jargument);


    std::string mainClass = configuration.mainClass;
    jAddJoint = vm->getJavaMethod(mainClass, "createEffortJointHandle", "(Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jGetInitialAngle = vm->getJavaMethod(mainClass, "getInitialJointAngleFromNative", "(Ljava/lang/String;)D");
    jAddIMU = vm->getJavaMethod(mainClass, "createIMUHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jAddForceTorqueSensor = vm->getJavaMethod(mainClass, "createForceTorqueSensorHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jInitialize = vm->getJavaMethod(mainClass, "initFromNative", "()Z");
    jUpdate = vm->getJavaMethod(mainClass, "updateFromNative", "(JJ)V");
    jReset = vm->getJavaMethod(mainClass, "resetFromNative", "()V");
    jShutdown = vm->getJavaMethod(mainClass, "shutdownFromNative", "()V");
}

std::shared_ptr<JointHandle> HalodiControllerImplementation::addJoint(std::string name)
{
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddJoint->callBytebufferMethod(bridge, NativeEffortJointHandleHolder::size * sizeof(double), jName->native());

    double initialAngle = jGetInitialAngle->callDoubleMethod(bridge, jName->native());
    return std::make_shared<NativeEffortJointHandleHolder>(buffer, initialAngle);
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

bool HalodiControllerImplementation::initialize()
{
    jInitialize->callBooleanMethod(bridge);
}

void HalodiControllerImplementation::update(long long timeInNanoseconds, long long duration)
{
    jUpdate->callVoidMethod(bridge, timeInNanoseconds, duration);
}

void HalodiControllerImplementation::reset()
{
    jReset->callVoidMethod(bridge);
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
