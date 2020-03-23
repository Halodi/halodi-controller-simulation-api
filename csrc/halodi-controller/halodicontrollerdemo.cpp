#include "halodi-controller/halodicontroller.h"

#include <iostream>

using namespace halodi_controller;

int main(int argc, char *argv[])
{
    std::shared_ptr<HalodiController> controller = HalodiController::create("/home/jesper/git/halodi/halodi-controller-simulation-api/bin/main");

    std::shared_ptr<JointHandle> joint1 = controller->addJoint("joint1");
    std::shared_ptr<JointHandle> joint2 = controller->addJoint("joint2");
    std::shared_ptr<IMUHandle> imu = controller->addIMU("pelvis", "imu");
    std::shared_ptr<ForceTorqueSensorHandle> forceTorqueSensor = controller->addForceTorqueSensor("foot", "sensor");


    controller->initialize();

    std::cout << "DEMO" << std::endl;
}
