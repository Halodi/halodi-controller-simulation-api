
#include "jvmLauncher/javavirtualmachine.h"
#include <iostream>

JNIEXPORT void JNICALL callVoidFunctionWithString
  (JNIEnv *env, jobject obj, jstring str)
{
    const char * cstr = env->GetStringUTFChars(str, 0);
    std::cout << "Got string from java: " << cstr << std::endl;
    env->ReleaseStringUTFChars(str, cstr);
}

JNIEXPORT jint JNICALL callIntFunctionWithBoolean
  (JNIEnv *env, jobject obj, jboolean a, jboolean b)
{
    return (a?1000:100) + (b?10:1);
}


timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int main(int argc, char *argv[])
{
    std::shared_ptr<JavaVirtualMachine> launcher = JavaVirtualMachine::startVM(".", "-Djava.class.path=/home/jesper/git/halodi/halodi-controller-simulation-api/bin/main");


    launcher->registerNativeMethod("com.halodi.controllerAPI.launcher.TestJVMLaunchCallback", "callVoidFunctionWithString", "(Ljava/lang/String;)V", (void *)&callVoidFunctionWithString);
    launcher->registerNativeMethod("com.halodi.controllerAPI.launcher.TestJVMLaunchCallback", "callIntFunctionWithBoolean", "(ZZ)I", (void *)&callIntFunctionWithBoolean);


    std::shared_ptr<JavaMethod> ctor = launcher->getJavaMethod("com.halodi.controllerAPI.launcher.TestJVMLaunchCallback", "<init>", "(I)V");
    std::shared_ptr<JavaMethod> method = launcher->getJavaMethod("com.halodi.controllerAPI.launcher.TestJVMLaunchCallback", "execute", "(I)V");

    std::shared_ptr<JavaMethod> add = launcher->getJavaMethod("com.halodi.controllerAPI.launcher.TestJVMLaunchCallback", "add", "()V");

    if(ctor && method && add)
    {
        std::shared_ptr<JavaObject> obj = ctor->createObject(jargument, 42);


        timespec start;
        timespec end;
        for(int c = 0; c < 10; c++)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
            for(int i = 0; i < 100000; i++)
            {
                add->callVoidMethod(obj);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            timespec elapsed = diff(start, end);
            std::cout << "Took " << elapsed.tv_sec << "s, " << elapsed.tv_nsec << "nsec for 100000 iterations" << std::endl;

        }




        method->callVoidMethod(obj, 124);

    }
}
