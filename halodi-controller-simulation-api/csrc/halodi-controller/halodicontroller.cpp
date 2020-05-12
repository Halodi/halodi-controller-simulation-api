#include "halodi-controller/halodicontroller.h"
#include "halodicontrollerimplementation.h"


namespace halodi_controller
{

std::shared_ptr<HalodiController> HalodiController::create(std::string controllerName, std::string working_directory)
{
    return std::make_shared<HalodiControllerImplementation>(controllerName, working_directory);
}


}
