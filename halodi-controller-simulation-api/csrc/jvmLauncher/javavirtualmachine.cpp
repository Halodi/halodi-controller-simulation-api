#include "jvmLauncher/javavirtualmachine.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <vector>

#include <stdexcept>

#include "../platform/platform.h"

std::string JavaVirtualMachine::getJNIError(int error)
{
    switch(error)
    {
        case JNI_OK: return "success";
        case JNI_ERR: return "unknown error";
        case JNI_EDETACHED: return "thread detached from the VM";
        case JNI_EVERSION : return "JNI version error";
        case JNI_ENOMEM : return "not enough memory";
        case JNI_EEXIST : return "VM already created";
        case JNI_EINVAL : return "invalid arguments";
    }
}

JNIEnv* JavaVirtualMachine::getEnv()
{
    JNIEnv* ret;

    if(!jvm)
    {
        throw std::runtime_error("JVM Not started");
    }
    jvm->GetEnv((void**) &ret, JNI_VERSION_1_8);
    if(!ret)
    {
        throw std::runtime_error("Cannot get env from JVM. Make sure the current thread is attached.");
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
    jvm->AttachCurrentThreadAsDaemon((void**)&tmp, 0);
}

void JavaVirtualMachine::detachCurrentThread()
{
    if(!jvm)
    {
        throw std::runtime_error("JVM Not started");
    }

    jvm->DetachCurrentThread();
}


std::string JavaVirtualMachine::expandClasspath(std::vector<std::string> classpathElements)
{

    if(classpathElements.empty())
    {
        return "";
    }
    else
    {
        std::string combinedClassPath = *classpathElements.begin();
        std::for_each(std::next(classpathElements.begin()), classpathElements.end(), [&combinedClassPath] (const std::string& s) { combinedClassPath += classpathSeperator + s; });
        return combinedClassPath;
    }


}


JavaVirtualMachine::JavaVirtualMachine(std::string javaHome, std::string workingDirectory, std::vector<std::string> classpath, std::vector<std::string> vmOptions)
{
    vmArguments.version = JNI_VERSION_1_8;

    std::vector<std::string> options;
    options.insert(options.end(), vmOptions.begin(), vmOptions.end());

    // Set classpath
    options.push_back("-Djava.class.path=" + expandClasspath(classpath));

    // Disable signal handling by the VM
    options.push_back("-Xrs");





   halodi_platform::CreateJavaVM createJavaVM = nullptr;

   fs::path javaPath(javaHome);

    halodi_platform::loadJNIFunctions(javaPath, &createJavaVM);

    JavaVMOption* javaOptions = new JavaVMOption[options.size()];

    int argc = 0;
    for(uint32_t i = 0; i < options.size(); i++)
    {
        if(options.at(i) != "-")
        {
            javaOptions[argc].optionString = new char[options.at(i).length() + 1];
            std::strcpy (javaOptions[argc].optionString, options.at(i).c_str());
            ++argc;
        }
    }
    vmArguments.nOptions = (jint) argc;
    vmArguments.options = javaOptions;
    vmArguments.ignoreUnrecognized = JNI_FALSE;



    fs::path currentDirectory = fs::current_path();

    if (workingDirectory != "." && workingDirectory != "")
    {
        fs::path workingPath(workingDirectory);

        try
        {
            fs::current_path(workingPath);
        }
        catch (...)
        {
            jvm = nullptr;

            throw std::runtime_error("Cannot change directory to " + workingDirectory);
        }

    }

    std::cout << "Starting Java VM from path  " << fs::current_path() << std::endl;




    JNIEnv* env;
    jint res = createJavaVM(&jvm, (void**) &env, &vmArguments);

    if(workingDirectory != ".")
    {
        try
        {
            fs::current_path(currentDirectory);
        }
        catch (...)
        {
            std::cerr << "Cannot return to previous working directory" << std::endl;
        }
    }

    if(res != JNI_OK)
    {
        jvm = nullptr;
        throw std::runtime_error("JVM failed to start. Error: " + getJNIError(res));
    }


    // Re-attach current thread as daemon. This makes DestroyVM not hang if called from a different thread
    detachCurrentThread();
    attachCurrentThread();

}

jclass JavaVirtualMachine::getClass(std::string className)
{
    JNIEnv* env = getEnv();

    std::string classNameCopy(className);
    std::replace (classNameCopy.begin(), classNameCopy.end(), '.', '/');

    jclass cls = env->FindClass(classNameCopy.c_str());
    if(!cls)
    {
        throw std::runtime_error("Cannot find class " + classNameCopy);
    }
    return cls;
}


std::shared_ptr<JavaVirtualMachine> JavaVirtualMachine::startVM(std::string javaHome, std::string workingDirectory, std::vector<std::string> classpath, std::vector<std::string> vmArguments)
{
    JavaVirtualMachine* vm = new JavaVirtualMachine(javaHome, workingDirectory, classpath, vmArguments);
    return std::shared_ptr<JavaVirtualMachine>(vm);
}


std::shared_ptr<StaticJavaMethod> JavaVirtualMachine::getStaticJavaMethod(std::string className, std::string methodName, std::string signature)
{

    JNIEnv* env = getEnv();

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
        throw std::runtime_error("Cannot register native method" + std::to_string(res));
    }
}

bool JavaVirtualMachine::isAssignableFrom(std::string subclass, std::string superclass)
{
    JNIEnv* env = getEnv();

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
    if(!jvm)
    {
        std::cerr << "Cannot shutdown JVM. JVM not started" << std::endl;
        return;
    }

    jint res = jvm->DestroyJavaVM();

    if(res == JNI_OK)
    {
        jvm = nullptr;
    }
    else
    {
        std::cerr << "Error stopping JVM: " << getJNIError(res) << std::endl;
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

    try
    {
        // Attach the current thread to the JVM to be able to destroy this object
        launcher->attachCurrentThread();

        JNIEnv* env = launcher->getEnv();
        env->DeleteGlobalRef(clazz);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void JavaMethod::callVoidMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();

    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, obj);
        env->CallVoidMethodV(obj->native(), methodID, arglist);
        va_end(arglist);
    }
    else
    {
        throw std::runtime_error(std::string(__FUNCTION__) + ": Unexpected object type");
    }
}

void* JavaMethod::callBytebufferMethod(std::shared_ptr<JavaObject> obj, int minimumCapacity,...)
{
    JNIEnv* env = launcher->getEnv();

    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, minimumCapacity);
        jobject byteBuffer = env->CallObjectMethodV(obj->native(), methodID, arglist);
        va_end(arglist);

        if(!byteBuffer)
        {
            throw std::runtime_error(std::string(__FUNCTION__) + ": Bytebuffer is null");
        }

        if(env->GetDirectBufferCapacity(byteBuffer) < minimumCapacity)
        {
            throw std::runtime_error(std::string(__FUNCTION__) + ": Bytebuffer is smaller than minimumCapacity");
        }

        return env->GetDirectBufferAddress(byteBuffer);

    }
    else
    {
        throw std::runtime_error(std::string(__FUNCTION__) + ": Unexpected object type");
    }
}

std::string JavaMethod::callStringMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();

    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, obj);
        jstring javaString = (jstring) env->CallObjectMethodV(obj->native(), methodID, arglist);
        va_end(arglist);

        if(!javaString)
        {
            throw std::runtime_error(std::string(__FUNCTION__) + ": String is null");
        }

        return launcher->toCppString(env, javaString);
    }
    else
    {
        throw std::runtime_error(std::string(__FUNCTION__) + ": Unexpected object type");
    }
}

std::string JavaVirtualMachine::toCppString(JNIEnv* env, jstring javaString)
{
    const char* cstr = env->GetStringUTFChars(javaString, 0);
    std::string cppStr = std::string(cstr);
    env->ReleaseStringUTFChars(javaString, cstr);
    return cppStr;
}

jboolean JavaMethod::callBooleanMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();

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
        throw std::runtime_error(std::string(__FUNCTION__) + ": Unexpected object type");
    }

    return returnValue;
}

jdouble JavaMethod::callDoubleMethod(std::shared_ptr<JavaObject> obj, ...)
{
    JNIEnv* env = launcher->getEnv();

    jdouble returnValue = 0.0;
    if(env->IsInstanceOf(obj->native(), clazz))
    {
        va_list arglist;
        va_start(arglist, obj);
        returnValue = env->CallDoubleMethodV(obj->native(), methodID, arglist);
        va_end(arglist);
    }
    else
    {
        throw std::runtime_error(std::string(__FUNCTION__) + ": Unexpected object type");
    }

    return returnValue;
}


std::shared_ptr<JavaObject> JavaMethod::createObject(jargument_t arg, ...)
{
    JNIEnv* env = launcher->getEnv();


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
        throw std::runtime_error(std::string(__FUNCTION__) + ": Cannot create new object");
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
    try
    {
        // Attach the current thread to the JVM to be able to destroy this object
        launcher->attachCurrentThread();

        JNIEnv* env = launcher->getEnv();
        env->DeleteGlobalRef(clazz);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void StaticJavaMethod::callVoidMethod(jargument_t arg, ...)
{
    JNIEnv* env = launcher->getEnv();

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
    try
    {
        // Attach the current thread to the JVM to be able to destroy this object
        launcher->attachCurrentThread();

        JNIEnv* env = launcher->getEnv();
        env->DeleteGlobalRef(object);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
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
    try
    {
        if(!jdata) return;

        // Attach the current thread to the JVM to be able to destroy this object
        launcher->attachCurrentThread();

        JNIEnv* env = launcher->getEnv();
        env->DeleteGlobalRef(jdata);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
