#include "jvmLauncher/javavirtualmachine.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <vector>

#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

#include <stdexcept>

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
        return nullptr;
    }
    jvm->GetEnv((void**) &ret, JNI_VERSION_1_6);
    if(!ret)
    {
        return nullptr;
    }

    return ret;
}

void JavaVirtualMachine::attachCurrentThread()
{
    if(!jvm)
    {
        throw std::runtime_error("JVM Not started");
    }

    JNIEnv* tmp;
    jvm->AttachCurrentThread((void**)&tmp, 0);
}

void JavaVirtualMachine::detachCurrentThread()
{
    if(!jvm)
    {
        throw std::runtime_error("JVM Not started");
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
            closedir(currentDirectory);
            jvm = nullptr;

            throw std::runtime_error("Cannot change directory to " + workingDirectory);
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
        throw std::runtime_error("JVM failed to start. Error code: " + res);
    }
}

jclass JavaVirtualMachine::getClass(std::string className)
{
    JNIEnv* env = getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

    std::string classNameCopy(className);
    std::replace (classNameCopy.begin(), classNameCopy.end(), '.', '/');

    jclass cls = env->FindClass(classNameCopy.c_str());
    if(!cls)
    {
        throw std::runtime_error("Cannot find class " + classNameCopy);
    }
    return cls;
}

std::shared_ptr<JavaVirtualMachine> JavaVirtualMachine::startVM(std::string workingDirectory, std::string vmArguments)
{
    JavaVirtualMachine* vm = new JavaVirtualMachine(workingDirectory, vmArguments);
    return std::shared_ptr<JavaVirtualMachine>(vm);
}


std::shared_ptr<StaticJavaMethod> JavaVirtualMachine::getStaticJavaMethod(std::string className, std::string methodName, std::string signature)
{

    JNIEnv* env = getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

    jclass cls = getClass(className);


    jmethodID mid = env->GetStaticMethodID(cls, methodName.c_str(), signature.c_str());
    if(!mid)
    {
        throw std::runtime_error("Cannot find method " + methodName + signature);
    }

    return std::make_shared<StaticJavaMethod>(shared_from_this(), (jclass) env->NewGlobalRef(cls), mid);
}

std::shared_ptr<JavaString> JavaVirtualMachine::createJavaString(std::string stdStr)
{
    return std::make_shared<JavaString>(shared_from_this(), stdStr);
}


std::shared_ptr<JavaMethod> JavaVirtualMachine::getJavaMethod(std::string className, std::string methodName, std::string signature)
{

    JNIEnv* env = getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

    jclass cls = getClass(className);


    jmethodID mid = env->GetMethodID(cls, methodName.c_str(), signature.c_str());
    if(!mid)
    {
        throw std::runtime_error("Cannot find method " + methodName + signature);
    }


    return std::make_shared<JavaMethod>(shared_from_this(), (jclass) env->NewGlobalRef(cls), mid);
}

void JavaVirtualMachine::registerNativeMethod(std::string className, std::string methodName, std::string signature, void *functionPointer)
{
    JNIEnv* env = getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

    jclass cls = getClass(className);

    JNINativeMethod *method = new JNINativeMethod[1];
    method[0].name = new char[methodName.length() + 1];
    method[0].signature = new char[signature.length() + 1];

    std::strcpy(method[0].name, methodName.c_str());
    std::strcpy(method[0].signature, signature.c_str());

    method[0].fnPtr = functionPointer;

    int res = env->RegisterNatives(cls, method, 1);
    if(res != JNI_OK)
    {
        throw std::runtime_error("Cannot register native method");
    }
}

bool JavaVirtualMachine::isAssignableFrom(std::string subclass, std::string superclass)
{
    JNIEnv* env = getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

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
}

JavaMethod::~JavaMethod()
{

    JNIEnv* env = launcher->getEnv();
    if(env)
    {
        env->DeleteGlobalRef(clazz);
    }
}

void JavaMethod::callVoidMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, obj);
        env->CallVoidMethodV(obj->native(), methodID, arglist);
        va_end(arglist);
    }
    else
    {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Unexpected object type");
    }
}

void* JavaMethod::callBytebufferMethod(std::shared_ptr<JavaObject> obj, int minimumCapacity,...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, minimumCapacity);
        jobject byteBuffer = env->CallObjectMethodV(obj->native(), methodID, arglist);
        va_end(arglist);

        if(env->GetDirectBufferCapacity(byteBuffer) < minimumCapacity)
        {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Bytebuffer is smaller than minimumCapacity");
        }

        return env->GetDirectBufferAddress(byteBuffer);

    }
    else
    {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Unexpected object type");
    }
}

jboolean JavaMethod::callBooleanMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

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
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Unexpected object type");
    }

    return returnValue;
}

std::shared_ptr<JavaObject> JavaMethod::createObject(jargument_t arg, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) throw std::runtime_error("Cannot get env");


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
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Cannot create new object");
    }
}



StaticJavaMethod::StaticJavaMethod(std::shared_ptr<JavaVirtualMachine> launcher_, jclass clazz_, jmethodID methodID_) :
    launcher(launcher_),
    clazz(clazz_),
    methodID(methodID_)
{
}

StaticJavaMethod::~StaticJavaMethod()
{
    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    env->DeleteGlobalRef(clazz);
}

void StaticJavaMethod::callVoidMethod(jargument_t arg, ...)
{
    JNIEnv* env = launcher->getEnv();
    if(!env) throw std::runtime_error("Cannot get env");

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
    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    env->DeleteGlobalRef(object);
}

JavaString::JavaString(std::shared_ptr<JavaVirtualMachine> launcher_, std::string string_) :
    launcher(launcher_)
{
    JNIEnv* env = launcher->getEnv();
    if(env)
    {
        jdata = (jstring)env->NewGlobalRef(env->NewStringUTF(string_.c_str()));
    }
    else
    {
        throw std::runtime_error("Cannot get env");
    }
}

JavaString::~JavaString()
{
    JNIEnv* env = launcher->getEnv();
    if(!env) return;

    if(!jdata) return;

    env->DeleteGlobalRef(jdata);
}
