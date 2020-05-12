#include "halodicontrollerconfigurationloader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "../platform/platform.h"

namespace  halodi_controller
{
using json = nlohmann::json;


HalodiControllerConfigurationLoader::HalodiControllerConfigurationLoader(std::string controllerName_)
{

    std::string localAppData = halodi_platform::getLocalAppData();

    std::string configurationFile = halodi_platform::appendToPath(halodi_platform::appendToPath(localAppData, company), configFile);



    std::ifstream configurationStream(configurationFile, std::ifstream::in);

    if(!configurationStream.is_open())
    {
        std::cerr << "Cannot open " + configurationFile << std::endl;
        throw new std::runtime_error("Cannot open " + configurationFile);
    }

    json config;
    configurationStream >> config;
    configurationStream.close();



    json plugins = config["plugins"];


    bool found = false;
    for (auto& plugin : plugins)
    {
        if(plugin["name"] == controllerName_)
        {
            name = plugin["name"];
            javaHome = plugin["javaHome"];
            mainClass = plugin["mainClass"];

            for(auto& classPathElement : plugin["classPath"])
            {
                classPath.push_back(classPathElement);
            }

            for(auto& vmArg : plugin["vmArgs"])
            {
                vmArgs.push_back(vmArg);
            }

            jsonConfiguration = plugin.dump(4);

            found = true;

            break;
        }
    }

    if(!found)

    {
        std::cerr << "Cannot find controller named " << controllerName_ << std::endl;
        throw new std::runtime_error("Controller not found");
    }


}

}
