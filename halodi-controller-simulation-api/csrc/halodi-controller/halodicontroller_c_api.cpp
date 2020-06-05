#include "halodicontroller_c_api.h"
#include "halodi-controller/halodicontroller.h"

#include "NativeEffortJointHandlerHolder.h"
#include "NativeForceTorqueSensorHandleHolder.h"
#include "NativeIMUHandleHolder.h"

#include <iostream>

using namespace halodi_controller;


std::shared_ptr<HalodiController> halodi_controller_ptr;
std::string halodi_controller_last_error;

halodi_controller_c_output_handler_delegate halodi_controller_c_output_handler = nullptr;

char* halodi_controller_to_c_str(std::string str)
{
    // Make a copy on the heap to avoid invalidation as soon as the function returns
    char* buffer = new char[str.length() + 1];
    str.copy(buffer, str.length());
    buffer[str.length()] = '\0';

    return buffer;
}


void halodi_controller_c_output_handler_callback(bool standardError, std::string message)
{
    if(halodi_controller_c_output_handler != nullptr)
    {
        halodi_controller_c_output_handler(standardError, halodi_controller_to_c_str(message));
    }
    else
    {
        if(standardError)
        {
            std::cerr << "[Controller (C)] " << message << std::endl;
        }
        else
        {
            std::cout << "[Controller (C)] " << message << std::endl;
        }
    }
}

bool halodi_controller_create(char* controllerName_, char *workingDirectory_)
{

    if(halodi_controller_ptr)
    {
        halodi_controller_last_error = "Controller already created";
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
            halodi_controller_last_error = "Controller name not set.";
            return false;
        }

        std::string controllerName = std::string(controllerName_);

        halodi_controller_ptr = HalodiController::create(controllerName, workingDirectory);

        halodi_controller_ptr->setOutputHandler(halodi_controller_c_output_handler_callback);

        return true;
    }
    catch(std::runtime_error re)
    {
        halodi_controller_last_error = re.what();
        return false;
    }
    catch(std::exception e)
    {
        halodi_controller_last_error = e.what();
        return false;
    }
    catch(...)
    {
        halodi_controller_last_error = "Undefined error.";
        return false;
    }
}


bool halodi_controller_created()
{
    if(halodi_controller_ptr)
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
    std::shared_ptr<JointHandle> joint = halodi_controller_ptr->addJoint(std::string(name));
    return std::dynamic_pointer_cast<NativeEffortJointHandleHolder>(joint)->c_data();
}

double *halodi_controller_add_imu(char *parentLink, char *name)
{
    std::shared_ptr<IMUHandle> imu = halodi_controller_ptr->addIMU(std::string(parentLink), std::string(name));
    return std::dynamic_pointer_cast<NativeIMUHandleHolder>(imu)->c_data();
}

double *halodi_controller_add_force_torque_sensor(char *parentLink, char *name)
{
    std::shared_ptr<ForceTorqueSensorHandle> forceTorqueSensor = halodi_controller_ptr->addForceTorqueSensor(std::string(parentLink), std::string(name));
    return std::dynamic_pointer_cast<NativeForceTorqueSensorHandleHolder>(forceTorqueSensor)->c_data();
}


char *halodi_controller_create_shared_buffer(char *name, int32_t size)
{
    std::shared_ptr<SharedBuffer> sharedBuffer = halodi_controller_ptr->createSharedBuffer(std::string(name), size);
    return sharedBuffer->data();
}

bool halodi_controller_initialize(char *arguments)
{
    return halodi_controller_ptr->initialize(std::string(arguments));
}


bool halodi_controller_start()
{
    return halodi_controller_ptr->start();

}

bool halodi_controller_update(long long timeInNanoseconds, long long duration)
{
    return halodi_controller_ptr->update(timeInNanoseconds, duration);
}

bool halodi_controller_stop()
{
    return halodi_controller_ptr->stop();
}

char *halodi_controller_get_controller_description()
{
    std::string desc = halodi_controller_ptr->getControllerDescription();
    return halodi_controller_to_c_str(desc);
}

char *halodi_controller_get_virtual_machine_configuration()
{
    std::string desc = halodi_controller_ptr->getVirtualMachineConfiguration();
    return halodi_controller_to_c_str(desc);
}


char *halodi_controller_call_controller(char *request, char *arguments)
{
    std::string reply = halodi_controller_ptr->callController(std::string(request), std::string(arguments));
    return halodi_controller_to_c_str(reply);
}


void halodi_controller_attach_current_thread()
{
    halodi_controller_ptr->attachCurrentThread();
}

void halodi_controller_deattach_current_thread()
{
    halodi_controller_ptr->deattachCurrentThread();
}


void halodi_controller_destroy()
{
    halodi_controller_ptr.reset();
}

void halodi_controller_free_string(char *str)
{
    delete str;
}


char *halodi_controller_get_last_error()
{
    return halodi_controller_to_c_str(halodi_controller_last_error);
}

void halodi_controller_set_output_delegate(halodi_controller_c_output_handler_delegate delegate)
{
    halodi_controller_c_output_handler = delegate;
}
