#pragma once
#include <string>
#include <jni.h>


#ifdef __cpp_lib_filesystem
    #include <filesystem>
    namespace fs = std::filesystem;
#else
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#endif



namespace  halodi_platform {
    fs::path getLocalAppData();


    typedef jint(JNICALL *CreateJavaVM)(JavaVM**, void**, void*);
    void loadJNIFunctions(fs::path javaHome, CreateJavaVM* createJavaVM);

}

