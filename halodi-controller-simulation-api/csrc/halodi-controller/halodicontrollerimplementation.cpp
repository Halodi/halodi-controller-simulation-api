#include "halodicontrollerimplementation.h"
#include <iostream>

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"

#include <stdexcept>

namespace  halodi_controller
{

HalodiControllerImplementation::HalodiControllerImplementation(ControllerConfiguration &configuration) :
    vmConfig(configuration)

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
    std::string json = "{";

    json += "\"workingDirectory\":\"" + vmConfig.workingDirectory + "\",";
    json += "\"classPath\":\"" + vmConfig.classPath + "\",";
    json += "\"mainClass\":\"" + vmConfig.mainClass + "\",";
    json += "\"vmOptions\":\"" + vmConfig.vmOptions + "\"";

    json += "}";
    return json;

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
