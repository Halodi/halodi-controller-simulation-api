#pragma once

#include <string>
#include <vector>

namespace  halodi_controller
{
class HalodiControllerConfigurationLoader
{
    const std::string company = "halodi";
    const std::string configFile = "controller-plugin-config.json";

public:
    HalodiControllerConfigurationLoader(std::string controllerName_);



    std::string name;
    std::string javaHome;
    std::vector<std::string> classPath;
    std::string mainClass;
    std::vector<std::string> vmArgs;


    std::string jsonConfiguration;

private:



};
}
