#include "halodicontrollerimplementation.h"
#include <iostream>

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"

namespace  halodi_controller
{

HalodiControllerImplementation::HalodiControllerImplementation(std::string classpath) :
    vm(JavaVirtualMachine::startVM(".", "-Djava.class.path=" + classpath))
{

    std::string mainClass = "com.halodi.controllerAPI.HalodiControllerJavaBridge";

    std::shared_ptr<JavaMethod> ctor = vm->getJavaMethod(mainClass, "<init>", "()V");
    bridge = ctor->createObject(jargument);


    jAddJoint = vm->getJavaMethod(mainClass, "createEffortJointHandle", "(Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    if(jAddJoint == nullptr)
    {
        return;
    }
    jAddIMU = vm->getJavaMethod(mainClass, "createIMUHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    if(jAddIMU == nullptr)
    {
        return;
    }
    jAddForceTorqueSensor = vm->getJavaMethod(mainClass, "createForceTorqueSensorHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    if(jAddForceTorqueSensor == nullptr)
    {
        return;
    }

    jInitialize = vm->getJavaMethod(mainClass, "initFromNative", "()V");
    if(jInitialize == nullptr)
    {
        std::cerr << "Cannot find initFromNative() in " << mainClass << std::endl;
    }
    jUpdate = vm->getJavaMethod(mainClass, "updateFromNative", "(J)V");
    if(jUpdate == nullptr)
    {
        std::cerr << "Cannot find updateFromNative(time) in " << mainClass << std::endl;
    }

    jReset = vm->getJavaMethod(mainClass, "resetFromNative", "()V");
    if(jReset == nullptr)
    {
        std::cerr << "Cannot find resetFromNative() in " << mainClass << std::endl;
    }
}

std::shared_ptr<JointHandle> HalodiControllerImplementation::addJoint(std::string name)
{
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddJoint->callBytebufferMethod(bridge, NativeEffortJointHandleHolder::size, jName->native());

    if(!buffer)
    {
        std::cerr << "Cannot create joint handle for " << name << std::endl;
        return nullptr;
    }

    return std::make_shared<NativeEffortJointHandleHolder>(buffer);
}

std::shared_ptr<IMUHandle> HalodiControllerImplementation::addIMU(std::string parentLink, std::string name)
{
    std::shared_ptr<JavaString> jParent = vm->createJavaString(parentLink);
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddIMU->callBytebufferMethod(bridge, NativeIMUHandleHolder::size, jParent->native(), jName->native());

    if(!buffer)
    {
        std::cerr << "Cannot create joint handle for " << name << std::endl;
        return nullptr;
    }

    return std::make_shared<NativeIMUHandleHolder>(buffer);
}

std::shared_ptr<ForceTorqueSensorHandle> HalodiControllerImplementation::addForceTorqueSensor(std::string parentLink, std::string name)
{
    std::shared_ptr<JavaString> jParent = vm->createJavaString(parentLink);
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    double* buffer = (double*) jAddForceTorqueSensor->callBytebufferMethod(bridge, NativeForceTorqueSensorHandleHolder::size, jParent->native(), jName->native());

    if(!buffer)
    {
        std::cerr << "Cannot create joint handle for " << name << std::endl;
        return nullptr;
    }

    return std::make_shared<NativeForceTorqueSensorHandleHolder>(buffer);
}

void HalodiControllerImplementation::initialize()
{


}

void HalodiControllerImplementation::update(long long timeInNanoseconds)
{
    //launcher->call(updateMethod, controllerObject, time.toNSec(), period.toNSec());

}

void HalodiControllerImplementation::reset()
{

}

}
