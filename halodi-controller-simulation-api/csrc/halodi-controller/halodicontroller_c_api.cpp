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




bool halodi_controller_create(char *workingDirectory, char *classPath, char *mainClass, char *vmOptions)
{
     freopen("stdout.txt", "a", stdout);
     freopen("stderr.txt", "a", stderr);


    if(controller)
    {
        return false;
    }

    try
    {
        ControllerConfiguration config;
        if(workingDirectory != nullptr)
        {
            config.workingDirectory = std::string(workingDirectory);
        }
        if(classPath != nullptr)
        {
            config.classPath = std::string(classPath);
        }
        if(mainClass != nullptr)
        {
            config.mainClass = std::string(mainClass);
        }
        if(vmOptions != nullptr)
        {
            config.vmOptions = std::string(vmOptions);
        }

        controller = HalodiController::create(config);

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

char *halodi_controller_get_controller_configuration()
{
    std::string desc = controller->getControllerConfiguration();
    return to_c_str(desc);
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
