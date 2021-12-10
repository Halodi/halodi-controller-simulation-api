# Halodi Controller Simulation API

This package provides a bridge between the Halodi controller written in Java and a generic C++ API. Additionally, Python bindings can be built, providing a Python API for the controller.

This code is based on [IHMC ROS Control](https://github.com/ihmcrobotics/ihmc-ros-control) and rewritten to be idependent off ROS Control. 

## Usage

### C++
The api is available in a single header [halodicontroller.h](include/halodi-controller/halodicontroller.h).

```cpp
#include "halodi-controller/halodicontroller.h"
using namespace halodi_controller;
```

To create a new controller (you can only create a single controller per process. Due to limitations in the JVM, you cannot restart the controller).

```cpp
    ControllerConfiguration config;

    config.mainClass = "com.halodi.eve.simulation.NativePluginEveSimulation";
    config.classPath = "[path to halodi-controller-all.jar]";


    std::shared_ptr<HalodiController> controller = HalodiController::create(config);
```

You need to register all joints and IMU's in the Eve URDF to run the controller. For the joints, use the name in the URDF. For the IMU, give the name of the parent link and the sensor name.

```cpp
std::shared_ptr<JointHandle> joint1 = controller->addJoint("[joint name]");
std::shared_ptr<IMUHandle> imu = controller->addIMU("[parent link]", "[imu name]");
```

After registering all joints and imu's, you can initialize the controller and update the simulation.

```cpp
    controller->initialize();


    while(1)
    {
        joint1->setPosition([angle in radians]);
        controller->update(time in nanoseconds, delta time in nanoseconds);

        double torque = joint1->getDesiredEffort();
        
        // Do simulation
    }

```
### Python usage
> __WARNING: Clone the pybind11 submodule to be able to build the Python bindings:__
```sh
# while cloning the halodi-controller-simulation-api repo
git clone --recurse-submodules path/to/halodi-controller-simulation-api.git
# Or after cloning the repo, from the root of the repo
git submodule update --init
```
>__If you want build the Python bindings, set the `BUILD_PYTHON_BINDINGS` cmake variable to `TRUE`:__
```sh
colcon build --cmake-args -DBUILD_PYTHON_BINDINGS=TRUE [your extra args]
```
>__A custom Python install can be used to build the bindings, the `CUSTOM_PYTHON_VERSION` (specify the custom version number) and `Python_ROOT_DIR` (specify the path to the root of the custom Python) variables have to be set:__
```sh
# Build example for a specific Python 3.7 install, shipped with Isaac sim
colcon build --cmake-args -DBUILD_PYTHON_BINDINGS=TRUE -DCUSTOM_PYTHON_VERSION=3.7 -DPython_ROOT_DIR=/home/user/.local/share/ov/pkg/isaac_sim-2021.2.0/kit/python [your extra args]
```
The same can be done from Python. The following snippet can be used to import the package:

```py
from ament_index_python.packages import get_package_share_directory

try:
    lib_path = get_package_share_directory(
        'halodi-controller-simulation-api')

    lib_path += "/../../lib/"
    sys.path.append(lib_path)

    import halodi_controller_python

except LookupError:
    print("halodi-controller-simulation-api package not found")
    print("Make sure to build that package and source the workspace.")

except ImportError as e:
    print("Could not import halodi_controller_python.")

```

The different class defined by the bindings are:
* __PyJointHandle__: Python class for a JointHandle object
* __PyIMUHandle__: Python class for an IMUHandle object
* __PyForceTorqueSensorHandle__: Python class for a ForceTorqueSensorHandle object
* __PyHalodiController__: Python class for a HalodiController object

This snippets showcases the controller's usage in Python:
```py
# Extra controller args can be specified, see the examples section of this README
controller_args = "--pubsub FAST_RTPS"

# Create the controller
controller = halodi_controller_python.PyHalodiController("NativePluginEveSimulation")

# Register all the joints and IMUs
joint1 = controller.addJoint("[joint_name]")
#...
imu1 = controller.addIMU("[parent link]", "[imu name]")
#...

# Initialization and update of the controller
controller.initialize(controller_args)

# start the controller
controller.start()

while True:
    joint1.setPosition(angle) # in radians
    controller.update(time, delta_time) # in nanoseconds
    torque = joint1.getDesiredEffort()
    
    # Do simulation

controller.stop()
```

## Linking

See the build files in the halodi-controller-gazebo plugin for hints how to link to this controller

- [CMakeLists.txt](../halodi-controller-gazebo/CMakeLists.txt)
- [package.xml](../halodi-controller-gazebo/package.xml)


If setup in a ROS2 workspace, the halodi-controller package contains a fully functional JVM and the library path will be setup correctly. Otherwise, you'll need to add the path to libjvm.so to your LD_LIBRARY_PATH envirioment variable.

The halodi-controller-gazebo plugin showcases how to link the halodi-controllers-simulation-api lib with `ament_target_dependencies`. Alternatively, you can manually link the library:
```cmake
target_link_libraries(your_lib halodi-controller-simulation-api::halodi-controller-api)
```

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

```bash
cd [Source directory]\halodi-controller-simulation-api\halodi-controller-simulation-api
md buildc
cd buildc
"C:\Program Files\CMake\bin\cmake.exe" -G "Visual Studio 16 2019" -A x64 ..
"C:\Program Files\CMake\bin\cmake.exe" --build . --config Release
```

# Publishing
To upload snapshot to artifacts.halodi.com:
```
ARTIFACT_USERNAME=[username] ARTIFACT_PASSWORD=[password] ARTIFACT_VERSION=[version] gradle publish
```

To upload release to artifacts.halodi.com:
```
ARTIFACT_USERNAME=[username] ARTIFACT_PASSWORD=[password] ARTIFACT_VERSION=[version] gradle publish -PenvironmentName=release
```
