#include "javavirtualmachine.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <vector>

#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

void JavaVirtualMachine::displayJNIError(std::string prefix, int error)
{
    switch(error)
    {
        case JNI_OK: std::cout << prefix << ": success" << std::endl; return;
        case JNI_ERR: std::cerr << prefix << ": unknown error" << std::endl; return;
        case JNI_EDETACHED: std::cerr << prefix << ": thread detached from the VM" << std::endl; return;
        case JNI_EVERSION : std::cerr << prefix << ": JNI version error" << std::endl; return;
        case JNI_ENOMEM : std::cerr << prefix << ": not enough memory" << std::endl; return;
        case JNI_EEXIST : std::cerr << prefix << ": VM already created" << std::endl; return;
        case JNI_EINVAL : std::cerr << prefix << ": invalid arguments" << std::endl; return;
    }
}

JNIEnv* JavaVirtualMachine::getEnv()
{
    JNIEnv* ret;

    if(!jvm)
    {
        std::cerr << "JVM not started" << std::endl;
        return nullptr;
    }
    jvm->GetEnv((void**) &ret, JNI_VERSION_1_6);
    if(!ret)
    {
        std::cerr << "Cannot get env" << std::endl;
        return nullptr;
    }

    return ret;
}

void JavaVirtualMachine::attachCurrentThread()
{
    if(!jvm)
    {
        std::cerr << "JVM not started" << std::endl;
        return;
    }

    JNIEnv* tmp;
    jvm->AttachCurrentThread((void**)&tmp, 0);
}

void JavaVirtualMachine::detachCurrentThread()
{
    if(!jvm)
    {
        std::cerr << "JVM not started" << std::endl;
        return;
    }

    jvm->DetachCurrentThread();
}


JavaVirtualMachine::JavaVirtualMachine(std::string workingDirectory, std::string vmOptions)
{
    vmArguments.version = JNI_VERSION_1_6;

    std::istringstream vmOptionsTokens(vmOptions);

    std::vector<std::string> options;
    while(!vmOptionsTokens.eof())
    {
        std::string option;
        std::getline(vmOptionsTokens, option, ' ');
        options.push_back(option);
    }


    JavaVMOption* javaOptions = new JavaVMOption[options.size()];
    for(uint i = 0; i < options.size(); i++)
    {
        javaOptions[i].optionString = new char[options.at(i).length() + 1];
        std::strcpy (javaOptions[i].optionString, options.at(i).c_str());
    }

    vmArguments.nOptions = (jint) options.size();
    vmArguments.options = javaOptions;
    vmArguments.ignoreUnrecognized = JNI_TRUE;




    DIR* currentDirectory = opendir(".");
    if(workingDirectory != "." && workingDirectory != "")
    {
        if(chdir(workingDirectory.c_str()) == -1)
        {
            std::cerr << "Cannot change directory to " << workingDirectory << std::endl;
            closedir(currentDirectory);
            jvm = nullptr;
            return;
        }

    }

    char temp [PATH_MAX];
    if(getcwd(temp, PATH_MAX) == 0); // Ignore return type
    std::cout << "Starting Java VM from path  " << temp << std::endl;

    JNIEnv* env;
    jint res = JNI_CreateJavaVM(&jvm, (void**) &env, &vmArguments);
    displayJNIError("Started Java VM", res);

    if(workingDirectory != ".")
    {
        if(fchdir(dirfd(currentDirectory)) != 0)
        {
            std::cerr << "Cannot return to previous working directory" << std::endl;
        }
    }
    closedir(currentDirectory);
    if(res != JNI_OK)
    {
        jvm = nullptr;
    }
}

jclass JavaVirtualMachine::getClass(std::string className)
{
    JNIEnv* env = getEnv();
    if(!env) return nullptr;

    std::string classNameCopy(className);
    std::replace (classNameCopy.begin(), classNameCopy.end(), '.', '/');

    jclass cls = env->FindClass(classNameCopy.c_str());
    if(!cls)
    {
        std::cerr << "Cannot find class " << classNameCopy << std::endl;
        return nullptr;
    }
    return cls;
}

std::shared_ptr<JavaVirtualMachine> JavaVirtualMachine::startVM(std::string workingDirectory, std::string vmArguments)
{
    return std::shared_ptr<JavaVirtualMachine>(new JavaVirtualMachine(workingDirectory, vmArguments));
}


std::shared_ptr<StaticJavaMethod> JavaVirtualMachine::getStaticJavaMethod(std::string className, std::string methodName, std::string signature)
{

    JNIEnv* env = getEnv();
    if(!env) return nullptr;

    jclass cls = getClass(className);
    if(!cls) return nullptr;


    jmethodID mid = env->GetStaticMethodID(cls, methodName.c_str(), signature.c_str());
    if(!mid)
    {
        std::cerr << "Cannot find method " << methodName << signature << std::endl;
        return nullptr;
    }

    std::shared_ptr<StaticJavaMethod> method = std::make_shared<StaticJavaMethod>(shared_from_this(), (jclass) env->NewGlobalRef(cls), mid);

    return method;
}


std::shared_ptr<JavaMethod> JavaVirtualMachine::getJavaMethod(std::string className, std::string methodName, std::string signature)
{

    JNIEnv* env = getEnv();
    if(!env) return nullptr;

    jclass cls = getClass(className);
    if(!cls) return nullptr;


    jmethodID mid = env->GetMethodID(cls, methodName.c_str(), signature.c_str());
    if(!mid)
    {
        std::cerr << "Cannot find method " << methodName << "()" << std::endl;
        return nullptr;
    }


    std::shared_ptr<JavaMethod> method = std::make_shared<JavaMethod>(shared_from_this(), (jclass) env->NewGlobalRef(cls), mid);


    return method;
}

bool JavaVirtualMachine::registerNativeMethod(std::string className, std::string methodName, std::string signature, void *functionPointer)
{
    JNIEnv* env = getEnv();
    if(!env) return false;

    jclass cls = getClass(className);
    if(!cls) return false;

    JNINativeMethod *method = new JNINativeMethod[1];
    method[0].name = new char[methodName.length() + 1];
    method[0].signature = new char[signature.length() + 1];

    std::strcpy(method[0].name, methodName.c_str());
    std::strcpy(method[0].signature, signature.c_str());

    method[0].fnPtr = functionPointer;

    int res = env->RegisterNatives(cls, method, 1);
    if(res != JNI_OK)
    {
        displayJNIError("Cannot register native method", res);
        return false;
    }
    else
    {
        return true;
    }
}

bool JavaVirtualMachine::isAssignableFrom(std::string subclass, std::string superclass)
{
    JNIEnv* env = getEnv();
    if(!env) return false;

    jclass sub = getClass(subclass);
    jclass sup = getClass(superclass);

    if(sub && sup)
    {
        return env->IsAssignableFrom(sub, sup);
    }
    else
    {
        return false;
    }
}

JavaVirtualMachine::~JavaVirtualMachine()
{
//    if(jvm)
//        stopVM();

    std::cout << "DTOR Launcher" << std::endl;


    if(!jvm)
    {
        return;
    }
    jint res = jvm->DestroyJavaVM();
    displayJNIError("Stopping Java VM", res);

    if(res == JNI_OK)
    {
        jvm = nullptr;
    }


    delete vmArguments.options;
}


JavaMethod::JavaMethod(std::shared_ptr<JavaVirtualMachine> launcher_, jclass clazz_, jmethodID methodID_) :
    launcher(launcher_),
    clazz(clazz_),
    methodID(methodID_)
{
    std::cout << "CTOR JavaMethod" << std::endl;
}

JavaMethod::~JavaMethod()
{
    std::cout << "DTOR JavaMethod" << std::endl;

    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    env->DeleteGlobalRef(clazz);
}

void JavaMethod::callVoidMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, obj);
        env->CallVoidMethodV(obj->native(), methodID, arglist);
        va_end(arglist);
    }
    else
    {
        std::cerr << __PRETTY_FUNCTION__ << ": Unexpected object type" << std::endl;
    }
}

jboolean JavaMethod::callBooleanMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) return false;

    bool returnValue = false;
    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, obj);
        returnValue = env->CallBooleanMethodV(obj->native(), methodID, arglist);
        va_end(arglist);
    }
    else
    {
        std::cerr << __PRETTY_FUNCTION__ << ": Unexpected object type" << std::endl;
    }

    return returnValue;
}

std::shared_ptr<JavaObject> JavaMethod::createObject(jargument_t arg, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) return nullptr;


    va_list arglist;
    va_start(arglist, arg);
    jobject newObject = env->NewObjectV(clazz, methodID, arglist);
    va_end(arglist);
    if(newObject)
    {
        return std::make_shared<JavaObject>(launcher, env->NewGlobalRef(newObject));
    }
    else
    {
        return nullptr;
    }
}



StaticJavaMethod::StaticJavaMethod(std::shared_ptr<JavaVirtualMachine> launcher_, jclass clazz_, jmethodID methodID_) :
    launcher(launcher_),
    clazz(clazz_),
    methodID(methodID_)
{
    std::cout << "CTOR StaticJavaMethod " << std::endl;
}

StaticJavaMethod::~StaticJavaMethod()
{
    std::cout << "DTOR StaticJavaMethod" << std::endl;


    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    env->DeleteGlobalRef(clazz);
}

void StaticJavaMethod::callVoidMethod(jargument_t arg, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    va_list arglist;
    va_start(arglist, arg);
    env->CallStaticVoidMethodV(clazz, methodID, arglist);
    va_end(arglist);
}



JavaObject::JavaObject(std::shared_ptr<JavaVirtualMachine> launcher_, jobject object_) :
    launcher(launcher_),
    object(object_)
{

}

JavaObject::~JavaObject()
{
    std::cout << "DTOR JavaObject" << std::endl;

    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    env->DeleteGlobalRef(object);
}
