#include "platform.h"
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <iostream>


#include <dlfcn.h>


namespace  halodi_platform {

std::filesystem::path getHomeDirectory()
{
    const char *homedir;


    if ((homedir = getenv("HOME")) == NULL)
    {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return std::filesystem::path(homedir);
}

std::filesystem::path getLocalAppData()
{
    const char *localAppData;

    if ((localAppData = getenv("XDG_DATA_HOME")) == NULL)
    {
        return getHomeDirectory() / ".local" /  "share";
    }
    else
    {
        return std::filesystem::path(localAppData);
    }
}

/**
 * @brief loadJNIFunctions
 *
 * Adapted from https://github.com/libgdx/packr/blob/master/src/main/native/packr/src/linux/packr_linux.cpp
 *
 * @param getDefaultJavaVMInitArgs
 * @param createJavaVM
 * @return
 */

bool loadJNIFunctions(std::string javaHome, CreateJavaVM* createJavaVM)
{


#if defined(__LP64__)
    std::filesystem::path libJVMPath = javaHome / "lib" / "amd64" / "server" / "libjvm.so");
#else
    std::filesystem::path libJVMPath = javaHome / "lib" / "i386" / "server" / "libjvm.so");
#endif

    void* handle = dlopen(libJVMPath.c_str(), RTLD_LAZY);
    if (handle == NULL) {
        std::cerr << dlerror() << std::endl;
        return false;
    }

    *createJavaVM = (CreateJavaVM) dlsym(handle, "JNI_CreateJavaVM");

    if (*createJavaVM == nullptr) {
        std::cerr << dlerror() << std::endl;
        return false;
    }

    return true;
}


}

