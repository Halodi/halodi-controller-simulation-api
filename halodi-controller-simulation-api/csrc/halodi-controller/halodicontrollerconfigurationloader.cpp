#include "halodicontrollerconfigurationloader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../platform/platform.h"

namespace  halodi_controller
{
using json = nlohmann::json;


HalodiControllerConfigurationLoader::HalodiControllerConfigurationLoader(std::string controllerName_)
{

    fs::path localAppData = halodi_platform::getLocalAppData();

    fs::path configurationFile = localAppData / company / configFile;



    std::ifstream configurationStream(configurationFile, std::ifstream::in);

    if(!configurationStream.is_open())
    {
        std::stringstream err;
        err << "Cannot open " << configurationFile;
        throw std::runtime_error(err.str());
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
        throw std::runtime_error("Cannot find controller named " + controllerName_);
    }


}

}
