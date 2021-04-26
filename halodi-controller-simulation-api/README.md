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


# Building

## Windows


Note: Due to path length limitations, the commpilation can fail. If you did not setup long paths on windows, it is recommended to checkout in C:\ws and compile from there.

##### Requirements:

- CMake [https://cmake.org/download/](https://cmake.org/download/). Recommended edition: Windows win64-x64 installer. Make sure to add to your path.
- Visual Studio 2019 Community [https://www.visualstudio.com/downloads/](https://www.visualstudio.com/downloads/).
        - Make sure to select "Desktop Development with C++" and select "C++ MFC for latest v142 build tools" under Optional.
        - Restart after installation and start visual studio (!)
##### Configuration and compilation:

Use CMake GUI to create the Visual Studio makefiles.
- Start the x64 Native Tools Command Prompt for VS 2019

```
cd [Source directory]\halodi-controller-simulation-api\halodi-controller-simulation-api
md buildc
cd buildc
"C:\Program Files\CMake\bin\cmake.exe" -G "Visual Studio 16 2019" -A x64 ..
"C:\Program Files\CMake\bin\cmake.exe" --build . --config Release
```

# Publishing
To publish the maven package from command line:
```
ARTIFACTS_USERNAME=[username] ARTIFACTS_PASSWORD=[password] gradle publish
```
