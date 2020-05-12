#pragma once
#include <stdint.h>

#ifdef WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif

extern "C"
{
    DLL_EXPORT void halodi_controller_redirect_output(char* stdoutFilename, char* stderrFilename);

    DLL_EXPORT bool halodi_controller_create(char* workingDirectory, char* classPath, char* mainClass, char* vmOptions);

    DLL_EXPORT bool halodi_controller_created();


    DLL_EXPORT double* halodi_controller_add_joint(char* name);
    DLL_EXPORT double* halodi_controller_add_imu(char* parentLink, char* name);
    DLL_EXPORT double* halodi_controller_add_force_torque_sensor(char* parentLink, char* name);


    DLL_EXPORT bool halodi_controller_initialize(char* arguments);

    DLL_EXPORT bool halodi_controller_start();
    DLL_EXPORT bool halodi_controller_update(long long timeInNanoseconds, long long duration);
    DLL_EXPORT bool halodi_controller_stop();

    DLL_EXPORT char* halodi_controller_get_controller_description();
    DLL_EXPORT char *halodi_controller_get_controller_configuration();


    DLL_EXPORT void halodi_controller_attach_current_thread();
    DLL_EXPORT void halodi_controller_deattach_current_thread();

    DLL_EXPORT void halodi_controller_destroy();

    DLL_EXPORT void halodi_controller_free_string(char* str);
}
