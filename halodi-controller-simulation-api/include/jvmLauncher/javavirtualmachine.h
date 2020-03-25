#ifndef LAUNCHER_H
#define LAUNCHER_H
#include <jni.h>
#include <string>
#include <memory>
#include <vector>

class JavaVirtualMachine;
class JavaObject;


typedef void* jargument_t;
static const jargument_t jargument = nullptr;

class JavaMethod
{
public:
    JavaMethod(std::shared_ptr<JavaVirtualMachine> launcher_, jclass clazz_, jmethodID methodID_);
    virtual ~JavaMethod();

    void callVoidMethod(std::shared_ptr<JavaObject> obj, ...);
    void* callBytebufferMethod(std::shared_ptr<JavaObject> obj, int minimumCapacity, ...);
    jboolean callBooleanMethod(std::shared_ptr<JavaObject> obj, ...);
    jboolean callDoubleMethod(std::shared_ptr<JavaObject> obj, ...);

    std::shared_ptr<JavaObject> createObject(jargument_t arg, ...);

private:


    std::shared_ptr<JavaVirtualMachine> launcher;
    jclass clazz;
    jmethodID methodID;


};

class StaticJavaMethod
{
public:
    StaticJavaMethod(std::shared_ptr<JavaVirtualMachine> launcher_, jclass clazz_, jmethodID methodID_);
    virtual ~StaticJavaMethod();
    void callVoidMethod(jargument_t arg, ...);
private:
    std::shared_ptr<JavaVirtualMachine> launcher;
    jclass clazz;
    jmethodID methodID;

};

class JavaObject
{
public:
    JavaObject(std::shared_ptr<JavaVirtualMachine> launcher_, jobject object_);
    virtual ~JavaObject();

    jobject native()
    {
        return object;
    }

private:
    std::shared_ptr<JavaVirtualMachine> launcher;
    jobject object;

};

class JavaString
{
public:
    JavaString(std::shared_ptr<JavaVirtualMachine> launcher_, std::string string_);
    virtual ~JavaString();

    jstring native()
    {
        return jdata;
    }

private:
    std::shared_ptr<JavaVirtualMachine> launcher;
    jstring jdata;

};


class JavaVirtualMachine : public std::enable_shared_from_this<JavaVirtualMachine>
{
private:
#ifdef _WIN32
    static const char classpathSeperator = ';';
#else
    static const char classpathSeperator = ':';
#endif

    JavaVirtualMachine(std::string workingDirectory, std::string classpath, std::string vmArguments);
    std::string expandClasspath(std::string classpath);


    JavaVM *jvm;
    JavaVMInitArgs vmArguments;
    void displayJNIError(std::string prefix, int error);

    jclass getClass(std::string className);

public:

    static std::shared_ptr<JavaVirtualMachine> startVM(std::string workingDirectory, std::string classpath, std::string vmArguments);


    std::shared_ptr<JavaMethod> getJavaMethod(std::string className, std::string method, std::string signature);
    std::shared_ptr<StaticJavaMethod> getStaticJavaMethod(std::string className, std::string methodName, std::string signature);

    std::shared_ptr<JavaString> createJavaString(std::string stdStr);

    void registerNativeMethod(std::string className, std::string method, std::string signature, void* functionPointer);

    bool isAssignableFrom(std::string subclass, std::string superclass);

    JNIEnv* getEnv();

    void attachCurrentThread();
    void detachCurrentThread();

    ~JavaVirtualMachine();
};

#endif // LAUNCHER_H
