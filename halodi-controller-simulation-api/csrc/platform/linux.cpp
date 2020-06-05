#include "platform.h"
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <iostream>


#include <dlfcn.h>


namespace  halodi_platform {

fs::path getHomeDirectory()
{
    const char *homedir;


    if ((homedir = getenv("HOME")) == NULL)
    {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return fs::path(homedir);
}

fs::path getLocalAppData()
{
    const char *localAppData;

    if ((localAppData = getenv("XDG_DATA_HOME")) == NULL)
    {
        return getHomeDirectory() / ".local" /  "share";
    }
    else
    {
        return fs::path(localAppData);
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

void loadJNIFunctions(fs::path javaHome, CreateJavaVM* createJavaVM)
{


#if defined(__LP64__)
    fs::path libJVMPath = javaHome / "lib" / "amd64" / "server" / "libjvm.so";
#else
    fs::path libJVMPath = javaHome / "lib" / "i386" / "server" / "libjvm.so";
#endif

    void* handle = dlopen(libJVMPath.c_str(), RTLD_LAZY);
    if (handle == NULL) {
        throw std::runtime_error("Cannot open libjvm.so: " + std::string(dlerror()));
    }

    *createJavaVM = (CreateJavaVM) dlsym(handle, "JNI_CreateJavaVM");

    if (*createJavaVM == nullptr) {
        throw std::runtime_error("JNI_CreateJavaVM not found: " + std::string(dlerror()));
    }
}


}

