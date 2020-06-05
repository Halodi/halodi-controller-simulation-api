#include <windows.h>
#include <initguid.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <wchar.h>
#include <string>
#include <sstream>

#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <direct.h>

#include <filesystem>
#include "platform.h"
#include <sstream>>

namespace  halodi_platform {

std::filesystem::path getLocalAppData()
{

    PWSTR path = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

    if (!SUCCEEDED(hr))
    {
        throw std::runtime_error("Cannot get path to local app data.");
    }

    CoTaskMemFree(path);

    std::filesystem::path fsPath(path);
    return fsPath;

}


static std::string getLastError(const char* reason) {

        LPTSTR buffer;
        DWORD errorCode = GetLastError();

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  nullptr, errorCode, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &buffer, 0, nullptr);


        std::stringstream stream;
        stream << "Error code [" << errorCode << "] when trying to " << reason << ": " << buffer;

        LocalFree(buffer);

        return stream.str();
}

void loadJNIFunctions(std::filesystem::path javaHome, CreateJavaVM* createJavaVM)
{
        std::filesystem::path jvmDll = javaHome / "bin" / "server" / "jvm.dll";

        HINSTANCE hinstLib = LoadLibraryW(jvmDll.c_str());
        if (hinstLib == nullptr) {
                DWORD errorCode = GetLastError();
                if (errorCode == 126) {

                        // "The specified module could not be found."
                        // load msvcr100.dll from the bundled JRE, then try again
                        
                       std::cout << "Failed to load jvm.dll. Trying to load msvcr100.dll first ..." << std::endl;
                
                        std::filesystem::path msvcr100 = javaHome / "bin" / "msvcr100.dll";

                        HINSTANCE hinstVCR = LoadLibraryW(msvcr100.c_str());
                        if (hinstVCR != nullptr) {
                                hinstLib = LoadLibraryW(jvmDll.c_str());
                        }
                }
        }

        if (hinstLib == nullptr) {
            throw std::runtime_error(getLastError("load jvm.dll"));
        }


        *createJavaVM = (CreateJavaVM) GetProcAddress(hinstLib, "JNI_CreateJavaVM");
        if (*createJavaVM == nullptr) {
                throw std::runtime_error(getLastError("obtain JNI_CreateJavaVM address"));
        }
}

}
