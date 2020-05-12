#pragma once
#include <string>
#include <jni.h>

namespace  halodi_platform {
    std::string getLocalAppData();

    std::string appendToPath(std::string path, std::string pathToAppend);



    typedef jint(JNICALL *CreateJavaVM)(JavaVM**, void**, void*);
    bool loadJNIFunctions(std::string javaHome, CreateJavaVM* createJavaVM);
}

