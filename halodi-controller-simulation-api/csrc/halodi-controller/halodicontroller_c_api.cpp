#include "halodicontroller_c_api.h"
#include "halodi-controller/halodicontroller.h"

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"

#include <iostream>

using namespace halodi_controller;


std::shared_ptr<HalodiController> controller;

char* to_c_str(std::string str)
{
    // Make a copy on the heap to avoid invalidation as soon as the
    char* buffer = new char[str.length() + 1];
    str.copy(buffer, str.length());
    buffer[str.length()] = '\0';

    return buffer;
}


void halodi_controller_redirect_output(char *stdoutFilename, char *stderrFilename)
{

    if(freopen(stdoutFilename, "a", stdout) == 0);
    if(freopen(stderrFilename, "a", stderr) == 0);

}


bool halodi_controller_create(char* controllerName_, char *workingDirectory_)
{

    if(controller)
    {
        return false;
    }

    try
    {

        std::string workingDirectory;
        if(workingDirectory_ != nullptr)
        {
            workingDirectory = std::string(workingDirectory_);
        }
        else
        {
            workingDirectory = ".";
        }

        if(controllerName_ == nullptr)
        {
            std::cout << "Controller name not set." << std::endl;
            return false;
        }

        std::string controllerName = std::string(controllerName_);

        controller = HalodiController::create(controllerName, workingDirectory);

        return true;
    }
    catch(std::runtime_error re)
    {
        std::cerr << "Cannot start controller with runtime exception " <<  re.what() << std::endl;
        return false;
    }
    catch(std::exception e)
    {
        std::cerr << "Cannot start controller with exception " <<  e.what() << std::endl;
        return false;
    }
    catch(...)
    {
        return false;
    }
}


bool halodi_controller_created()
{
    if(controller)
    {
        return true;
    }
    else
    {
        return false;
    }
}

double *halodi_controller_add_joint(char *name)
{
    std::shared_ptr<JointHandle> joint = controller->addJoint(std::string(name));
    return std::dynamic_pointer_cast<NativeEffortJointHandleHolder>(joint)->c_data();
}

double *halodi_controller_add_imu(char *parentLink, char *name)
{
    std::shared_ptr<IMUHandle> imu = controller->addIMU(std::string(parentLink), std::string(name));
    return std::dynamic_pointer_cast<NativeIMUHandleHolder>(imu)->c_data();
}

double *halodi_controller_add_force_torque_sensor(char *parentLink, char *name)
{
    std::shared_ptr<ForceTorqueSensorHandle> forceTorqueSensor = controller->addForceTorqueSensor(std::string(parentLink), std::string(name));
    return std::dynamic_pointer_cast<NativeForceTorqueSensorHandleHolder>(forceTorqueSensor)->c_data();
}


char *halodi_controller_create_shared_buffer(char *name, int32_t size)
{
    std::shared_ptr<SharedBuffer> sharedBuffer = controller->createSharedBuffer(std::string(name), size);
    return sharedBuffer->data();
}

bool halodi_controller_initialize(char *arguments)
{
    return controller->initialize(std::string(arguments));
}


bool halodi_controller_start()
{
    return controller->start();

}

bool halodi_controller_update(long long timeInNanoseconds, long long duration)
{
    return controller->update(timeInNanoseconds, duration);
}

bool halodi_controller_stop()
{
    return controller->stop();
}

char *halodi_controller_get_controller_description()
{
    std::string desc = controller->getControllerDescription();
    return to_c_str(desc);
}

char *halodi_controller_get_virtual_machine_configuration()
{
    std::string desc = controller->getVirtualMachineConfiguration();
    return to_c_str(desc);
}


char *halodi_controller_call_controller(char *request, char *arguments)
{
    std::string reply = controller->callController(std::string(request), std::string(arguments));
    return to_c_str(reply);
}


void halodi_controller_attach_current_thread()
{
    controller->attachCurrentThread();
}

void halodi_controller_deattach_current_thread()
{
    controller->deattachCurrentThread();
}


void halodi_controller_destroy()
{
    controller.reset();
}

void halodi_controller_free_string(char *str)
{
    delete str;
}

