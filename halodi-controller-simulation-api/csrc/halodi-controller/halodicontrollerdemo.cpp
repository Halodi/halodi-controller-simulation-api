#include "halodi-controller/halodicontroller.h"

#include <iostream>

using namespace halodi_controller;

int main(int argc, char *argv[])
{
    std::shared_ptr<HalodiController> controller = HalodiController::create("HalodiControllerDemo");

    std::shared_ptr<JointHandle> joint1 = controller->addJoint("joint1");
    std::shared_ptr<JointHandle> joint2 = controller->addJoint("joint2");
    std::shared_ptr<IMUHandle> imu = controller->addIMU("pelvis", "imu");
    std::shared_ptr<ForceTorqueSensorHandle> forceTorqueSensor = controller->addForceTorqueSensor("foot", "sensor");

    controller->initialize("--help");

    std::cout << "Initialization: " << controller->getControllerDescription() << std::endl;

    controller->start();


    for(unsigned int i = 0; i < 10; ++i)
    {
        joint1->setPosition((double) i);
        joint2->setPosition(-2.0 * i);


        std::cout << " ----- " << i << " ----- " << std::endl;
        controller->update(i * 1000000, 1000000);

        std::cout << "Torques: " << joint1->getDesiredEffort() << " " << joint2->getDesiredEffort() << std::endl;
    }

    controller->stop();

}
