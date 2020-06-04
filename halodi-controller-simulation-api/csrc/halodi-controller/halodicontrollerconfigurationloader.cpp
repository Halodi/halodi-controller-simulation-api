#include "halodicontrollerconfigurationloader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <sstream>
#include <filesystem>

#include "../platform/platform.h"

namespace  halodi_controller
{
using json = nlohmann::json;


HalodiControllerConfigurationLoader::HalodiControllerConfigurationLoader(std::string controllerName_)
{

    std::filesystem::path localAppData = halodi_platform::getLocalAppData();

    std::filesystem::path configurationFile = localAppData / company / configFile;



    std::ifstream configurationStream(configurationFile, std::ifstream::in);

    if(!configurationStream.is_open())
    {
        std::stringstream err;
        err << "Cannot open " << configurationFile;

        std::cerr << err.str() << std::endl;
        throw new std::runtime_error(err.str());
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

            std::cout << name << std::endl;
            std::cout << javaHome << std::endl;
            std::cout << mainClass << std::endl;

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
