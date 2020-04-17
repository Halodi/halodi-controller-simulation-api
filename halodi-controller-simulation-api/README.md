# Halodi Controller Simulation API

This package provides a bridge between the Halodi controller written in Java and a generic C++ API.

This code is based on [IHMC ROS Control](https://github.com/ihmcrobotics/ihmc-ros-control) and rewritten to be idependent off ROS Control. 

## Usage

The api is available in a single header [halodicontroller.h](include/halodi-controller/halodicontroller.h).

```
#include "halodi-controller/halodicontroller.h"
using namespace halodi_controller;
```

To create a new controller (you can only create a single controller per process. Due to limitations in the JVM, you cannot restart the controller).

```
    ControllerConfiguration config;

    config.mainClass = "com.halodi.eve.simulation.NativePluginEveSimulation";
    config.classPath = "[path to halodi-controller-all.jar]";


    std::shared_ptr<HalodiController> controller = HalodiController::create(config);
```

You need to register all joints and IMU's in the Eve URDF to run the controller. For the joints, use the name in the URDF. For the IMU, give the name of the parent link and the sensor name.

```
std::shared_ptr<JointHandle> joint1 = controller->addJoint("[joint name]");
std::shared_ptr<IMUHandle> imu = controller->addIMU("[parent link]", "[imu name]");
```

After registering all joints and imu's, you can initialize the controller and update the simulation.

```
    controller->initialize();


    while(1)
    {
        joint1->setPosition([angle in radians]);
        controller->update(time in nanoseconds, delta time in nanoseconds);

        double torque = joint1->getDesiredEffort();
        
        // Do simulation
    }

```


## Linking

See the build files in the halodi-controller-gazebo plugin for hints how to link to this controller

- [CMakeLists.txt](../halodi-controller-gazebo/CMakeLists.txt)
- [package.xml](../halodi-controller-gazebo/package.xml)


If setup in a ROS2 workspace, the halodi-controller package contains a fully functional JVM and the library path will be setup correctly. Otherwise, you'll need to add the path to libjvm.so to your LD_LIBRARY_PATH envirioment variable.


# Examples

- [Gazebo plugin](../halodi-controller-gazebo)
