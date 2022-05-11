import ctypes
from ctypes import *
from pathlib import Path
import json
import threading
import logging

class EffortJointHandle:
    """
    Wrapper class around the shared double array between the simulator and controller for Joints
    """

    def __init__(self, address) -> None:
        self.buffer = (ctypes.c_double * 6).from_address(address)
        
    def SetPosition(self, position: float):
        """
        Set the position of the joint from the simulator [rad]
        """
        self.buffer[0] = position

    def SetVelocity(self, velocity: float):
        """
        Set the velocity of the joint from the simulator [rad/s]
        """
        self.buffer[1] = velocity

    def SetEffort(self, effort: float):
        """
        Set the effort of the joint from the simulator [Nm]
        """
        self.buffer[2] = effort

    @property
    def DesiredEffort(self) -> float:
        """
        Get the desired effort from the controller [Nm]
        """
        return self.buffer[3]

    @property
    def DampingScale(self) -> float:
        """
        Get the desired damping scale from the controller [0.0 - 1.0]
        """
        return self.buffer[4]
        
    @property
    def InitialAngle(self) -> float:
        """
        Get the initial angle to initialize the simulator to start the controller in a stable pose
        """
        return self.buffer[5]




class IMUHandle:
    """
    Wrapper class around the shared double buffer for the IMU state between the simulator and the controller
    """
    def __init__(self, address) -> None:
        self.buffer = (ctypes.c_double * 10).from_address(address)


    def SetOrientation(self, qx, qy, qz, qw):
        """
        Set the orientation quaternion measured by the (simulated) IMU
        """
        self.buffer[0] = qx
        self.buffer[1] = qy
        self.buffer[2] = qz
        self.buffer[3] = qw

    def SetAngularVelocity(self, wx, wy, wz):
        """
        Set the angular velocity measured by the (simulated) IMU
        """
        self.buffer[7] = wx
        self.buffer[8] = wy
        self.buffer[9] = wz

    def SetLinearAcceleration(self, xdd, ydd, zdd):
        """
        Set the linear acceleration measured by the (simulated) IMU
        """
        self.buffer[4] = xdd
        self.buffer[5] = ydd
        self.buffer[6] = zdd



class HalodiControllerPlugin:
    """
    Wrapper between the halodi-controller-simulation-plugin C API and python
    """

    def __init__(self, controllerName : str):
        self.libname = (Path(__file__) / "../../../../bin/halodi-controller-simulation-api/lib/libhalodi-controller-api.so").resolve()
        self.workingdir = (Path(__file__) / "../../../../../../").resolve()

        self.plugin = ctypes.CDLL(self.libname)

        self.plugin.halodi_controller_get_virtual_machine_configuration.restype = ctypes.c_char_p
        self.plugin.halodi_controller_create.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self.plugin.halodi_controller_get_last_error.restype = ctypes.c_char_p
        self.plugin.halodi_controller_created.restype = ctypes.c_bool
        self.plugin.halodi_controller_add_joint.argtypes = [ctypes.c_char_p]
        self.plugin.halodi_controller_add_joint.restype = ctypes.c_int64
        self.plugin.halodi_controller_add_imu.argtypes = [ctypes.c_char_p]
        self.plugin.halodi_controller_add_imu.restype = ctypes.c_int64
        self.plugin.halodi_controller_get_controller_description.restype = c_char_p
        self.plugin.halodi_controller_initialize.argtypes = [ctypes.c_char_p]
        self.plugin.halodi_controller_initialize.restype = ctypes.c_bool
        self.plugin.halodi_controller_start.restype = ctypes.c_bool
        self.plugin.halodi_controller_update.argtypes = [ctypes.c_int64, ctypes.c_int64]
        self.plugin.halodi_controller_update.restype = ctypes.c_bool
        self.plugin.halodi_controller_stop.restype = ctypes.c_bool

        if(self.plugin.halodi_controller_created()):
            logging.info("Attaching to existing VM")
            self.Attach()

            self.config = json.loads(self.plugin.halodi_controller_get_virtual_machine_configuration())

            if(self.config["name"] != controllerName):
                raise RuntimeError("Trying to load different controller. Restart omniverse")
        else:
            if(self.plugin.halodi_controller_create(bytes(controllerName, encoding='utf8'), bytes(str(self.workingdir), encoding='utf8'))):
                self.Attach()
                self.config = json.loads(self.plugin.halodi_controller_get_virtual_machine_configuration())
                logging.info("Started new Virtual Machine")
            else:
                self.PrintLastError("Cannot load Virtual Machine")

        return



    def Attach(self) -> None:
        """
        Attach the controller to the current thread, if not already attached.

        If the thread is already attached, this is a no-op.

        Used internally in this class, no need to call externally
        """
        local = threading.local()

        if (hasattr(local, 'attached')):
            attached = local.attached
        else:
            attached = False

        if(not attached):
            self.plugin.halodi_controller_attach_current_thread()
            local.attached = True
        


    def Deattach(self) -> None:
        """
        De-attach the controller from the current thread, if attached

        If the thread is not attached, this is a no-op.

        Used internally in this class, no need to call externally
        """
        local = threading.local()

        if (hasattr(local, 'attached')):
            attached = local.attached
        else:
            attached = False


        if(attached):
            self.plugin.halodi_controller_deattach_current_thread()
            local.attached = False
        


    def AddJoint(self, name: str) -> EffortJointHandle:
        """
        Add a joint to the controller and return a handle to the shared data buffer
        """
        self.Attach()

        return EffortJointHandle(self.plugin.halodi_controller_add_joint(bytes(name, encoding='utf8')))

    def AddIMU(self, parent_link: str, name: str) -> IMUHandle:
        """
        Add an IMU to the controller and return a handle to the shared data buffer
        """
        self.Attach()

        return IMUHandle(self.plugin.halodi_controller_add_imu(bytes(parent_link, encoding='utf8'), bytes(name, encoding='utf8')))
        

    def PrintLastError(self, description: str):
        """
        Print the last error returned from the controller
        """
        logging.error(f"{description}: {self.plugin.halodi_controller_get_last_error()}")


    def GetControllerConfiguration(self):
        """
        Get a json dictionary with the configuration of the controller
        """
        self.Attach()
        return json.loads(self.plugin.halodi_controller_get_controller_description())

    def Initialize(self, arguments: str):
        """
        Initialize the JVM. Call before start, but after adding all the joints and IMUs

        The jvm can only be initialized once. If the jvm is already initialized, and the configuration
        is the same the jvm can be re-used
        """
        self.Attach()
        self.controllerConfiguration = self.GetControllerConfiguration()

        if(self.controllerConfiguration["initialized"] == 'true'):
            logging.info(f"Controller loaded")
        else:
            if(not self.plugin.halodi_controller_initialize(bytes(arguments, encoding='utf8'))):
                raise RuntimeError("Cannot initialize controller")

        self.controllerConfiguration = self.GetControllerConfiguration()


    def Start(self):
        """
        Start the controller

        The controller can be stopped and restarted
        """
        self.Attach()
        if (not self.plugin.halodi_controller_start()):
            raise RuntimeError(f"Cannot start controller {self.plugin.halodi_controller_get_last_error()}")

    def Update(self, timeInSeconds: float, stepInSeconds: float):
        """
        Run a single controller tick
        """
        timeInNs = int(timeInSeconds * 1e9)
        stepInNs = int(stepInSeconds * 1e9)

        if(not self.plugin.halodi_controller_update(timeInNs, stepInNs)):
            raise RuntimeError(f"Cannot step controller {self.plugin.halodi_controller_get_last_error()}")
    
    
    def Stop(self):
        """
        Stop the controller
        """

        self.Attach()
        if (not self.plugin.halodi_controller_stop()):
            raise RuntimeError(f"Cannot stop controller {self.plugin.halodi_controller_get_last_error()}")
        self.Deattach()
