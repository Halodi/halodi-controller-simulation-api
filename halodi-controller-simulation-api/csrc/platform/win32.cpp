#include <windows.h>
#include <initguid.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <wchar.h>
#include <string>

#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <direct.h>

#include "platform.h"

namespace  halodi_platform {

std::string getLocalAppData()
{

    PWSTR path = NULL;
    HRESULT hr = SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, NULL, &path);

    if (!SUCCEEDED(hr))
    {
        throw new std::runtime_error("Cannot get local path");
    }

    std::wstring localAppDataWide = std::wstring(path);
    std::string localAppData = std::string(localAppDataWide.begin(), localAppDataWide.end());

    CoTaskMemFree(path);

    return localAppData;

}

std::string appendToPath(std::string path, std::string pathToAppend)
{
    return path + "\\" + pathToAppend;
}


static void printLastError(const char* reason) {

        LPTSTR buffer;
        DWORD errorCode = GetLastError();

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  nullptr, errorCode, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &buffer, 0, nullptr);

        std::cerr << "Error code [" << errorCode << "] when trying to " << reason << ": " << buffer;

        LocalFree(buffer);
}

bool loadJNIFunctions(std::string javaHome, GetDefaultJavaVMInitArgs* getDefaultJavaVMInitArgs, CreateJavaVM* createJavaVM)
{

        std::string jvmDll = appendToPath(javaHome, "bin\\server\\jvm.dll");

        LPCTSTR jvmDLLPath = TEXT(jvmDll.c_str());

        HINSTANCE hinstLib = LoadLibrary(jvmDLLPath);
        if (hinstLib == nullptr) {
                DWORD errorCode = GetLastError();
                if (errorCode == 126) {

                        // "The specified module could not be found."
                        // load msvcr100.dll from the bundled JRE, then try again
                        if (verbose) {
                                std::cerr << "Failed to load jvm.dll. Trying to load msvcr100.dll first ..." << std:endl;
                        }

                        std::string msvcr100 = appendToPath(javaHome, "bin\\msvcr100.dll");

                        HINSTANCE hinstVCR = LoadLibrary(TEXT(msvcr100.c_str()));
                        if (hinstVCR != nullptr) {
                                hinstLib = LoadLibrary(jvmDLLPath);
                        }
                }
        }

        if (hinstLib == nullptr) {
                printLastError("load jvm.dll");
                return false;
        }

        *getDefaultJavaVMInitArgs = (GetDefaultJavaVMInitArgs) GetProcAddress(hinstLib, "JNI_GetDefaultJavaVMInitArgs");
        if (*getDefaultJavaVMInitArgs == nullptr) {
                printLastError("obtain JNI_GetDefaultJavaVMInitArgs address");
                return false;
        }

        *createJavaVM = (CreateJavaVM) GetProcAddress(hinstLib, "JNI_CreateJavaVM");
        if (*createJavaVM == nullptr) {
                printLastError("obtain JNI_CreateJavaVM address");
                return false;
        }

        return true;
}

}
