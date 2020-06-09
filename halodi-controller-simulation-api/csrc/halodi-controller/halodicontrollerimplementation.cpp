#include "halodicontrollerimplementation.h"
#include <iostream>

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"
#include "sharedbufferimplementation.h"

#include <stdexcept>

namespace  halodi_controller
{

void HalodiControllerImplementation::standardOuputCallback(JNIEnv *env, jobject caller, jlong ptr, jboolean standardError, jstring message)
{
    HalodiControllerImplementation* impl = (HalodiControllerImplementation*) ptr;
    impl->standardOutputHandler(false, JavaVirtualMachine::toCppString(env, message));
}

HalodiControllerImplementation::HalodiControllerImplementation(std::string controllerName_, std::string working_directory_) :
    configurationLoader(controllerName_)
{

    vm = JavaVirtualMachine::startVM(configurationLoader.javaHome, working_directory_, configurationLoader.classPath, configurationLoader.vmArgs);


    std::string mainClass = configurationLoader.mainClass;


    vm->registerNativeMethod("com.halodi.controllerAPI.NativeHalodiControllerJavaBridge", "outputHandler", "(JZLjava/lang/String;)V", reinterpret_cast<void*>(&HalodiControllerImplementation::standardOuputCallback));

    std::shared_ptr<JavaMethod> ctor = vm->getJavaMethod(mainClass, "<init>", "()V");
    bridge = ctor->createObject(jargument);


    std::shared_ptr<JavaMethod> jRedirectOutput = vm->getJavaMethod(mainClass, "redirectOutput", "(J)V");
    jRedirectOutput->callVoidMethod(bridge, (jlong) this);




    jAddJoint = vm->getJavaMethod(mainClass, "createEffortJointHandle", "(Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jAddIMU = vm->getJavaMethod(mainClass, "createIMUHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jAddForceTorqueSensor = vm->getJavaMethod(mainClass, "createForceTorqueSensorHandle", "(Ljava/lang/String;Ljava/lang/String;)Ljava/nio/ByteBuffer;");
    jInitialize = vm->getJavaMethod(mainClass, "initFromNative", "(Ljava/lang/String;)Z");
    jStart = vm->getJavaMethod(mainClass, "startFromNative", "()Z");
    jUpdate = vm->getJavaMethod(mainClass, "updateFromNative", "(JJ)Z");
    jStop = vm->getJavaMethod(mainClass, "stopFromNative", "()Z");
    jShutdown = vm->getJavaMethod(mainClass, "shutdownFromNative", "()V");
    jGetControllerDescription = vm->getJavaMethod(mainClass, "getControllerDescriptionFromNative", "()Ljava/lang/String;");

    jCallController = vm->getJavaMethod(mainClass, "callControllerFromNative", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    jCreateSharedBuffer = vm->getJavaMethod(mainClass, "createSharedBuffer", "(Ljava/lang/String;I)Ljava/nio/ByteBuffer;");



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

std::string HalodiControllerImplementation::getVirtualMachineConfiguration()
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

std::string HalodiControllerImplementation::callController(std::string request, std::string arguments)
{
    std::shared_ptr<JavaString> jRequest = vm->createJavaString(request);
    std::shared_ptr<JavaString> jArguments = vm->createJavaString(arguments);

    return jCallController->callStringMethod(bridge, jRequest->native(), jArguments->native());
}

std::shared_ptr<SharedBuffer> HalodiControllerImplementation::createSharedBuffer(std::string name, int32_t size)
{
    std::shared_ptr<JavaString> jName = vm->createJavaString(name);
    char* const buffer= (char*) jCreateSharedBuffer->callBytebufferMethod(bridge, size, jName->native(), size);
    return std::make_shared<SharedBufferImplementation>(size, buffer);
}

void HalodiControllerImplementation::setOutputHandler(std::function<void (bool, std::string)> handler)
{
    this->outputHandler = handler;
}

HalodiControllerImplementation::~HalodiControllerImplementation()
{
    vm->attachCurrentThread();
    jShutdown->callVoidMethod(bridge);
}

void HalodiControllerImplementation::standardOutputHandler(bool stdErr, std::string message)
{
    if(outputHandler)
    {
        outputHandler(stdErr, message);
    }
    else
    {
        if(stdErr)
        {
            std::cerr << "[Controller] " << message << std::endl;
        }
        else
        {
            std::cout << "[Controller] " << message << std::endl;
        }
    }
}

}
