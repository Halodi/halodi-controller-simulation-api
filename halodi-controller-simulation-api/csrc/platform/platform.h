#pragma once
#include <string>
#include <jni.h>


#include <filesystem>


namespace  halodi_platform {
    std::filesystem::path getLocalAppData();


    typedef jint(JNICALL *CreateJavaVM)(JavaVM**, void**, void*);
    bool loadJNIFunctions(std::filesystem::path javaHome, CreateJavaVM* createJavaVM);

}

