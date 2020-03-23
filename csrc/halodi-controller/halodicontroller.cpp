#include "halodi-controller/halodicontroller.h"
#include "halodicontrollerimplementation.h"


namespace halodi_controller
{

std::shared_ptr<HalodiController> HalodiController::create(std::string classpath)
{
    return std::make_shared<HalodiControllerImplementation>(classpath);
}


}
