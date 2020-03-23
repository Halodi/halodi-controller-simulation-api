#include "halodi-controller/halodicontroller.h"
#include "halodicontrollerimplementation.h"


namespace halodi_controller
{

std::shared_ptr<HalodiController> HalodiController::create(ControllerConfiguration &configuration)
{
    return std::make_shared<HalodiControllerImplementation>(configuration);
}


}
