#pragma once

#include <string>
#include <memory>

#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace halodi_controller
{

class DLL_EXPORT JointHandle
{
public:
    /**
             * @brief setPosition Set the current position of this joint
             * @param position [rad]
             */
    virtual void setPosition(double position) = 0;

    /**
             * @brief setVelocity Set the current velocity of this joint
             * @param velocity [rad/s]
             */
    virtual void setVelocity(double velocity) = 0;

    /**
             * @brief setMeasuredForce Set the current effort of the joint
             * @param force
             */
    virtual void setMeasuredEffort(double effort) = 0;


    /**
             * @brief getForce Get the desired effort for this joint
             *
             * Angular joint: Effort in Nm
             * Linear joint: effort in N
             *
             * @return Desired effort applied to this 5joint
             */
    virtual double getDesiredEffort() = 0;

    /**
             * @brief getDampingScale Get the desired relative damping for this joint
             *
             * This functionality is to stablize the controller in the simulation and is also used on the actual robot.
             *
             *
             * @return Desired damping scale on this joint, between 0 - 1
             */
    virtual double getDampingScale() = 0;


    /**
     * @brief getInitialAngle Get the intial angle on start for the controller
     * @return Initial angle [rad]
     */
    virtual double getInitialAngle() = 0;
};

class DLL_EXPORT IMUHandle
{
public:

    /**
          * @brief setOrientationQuaternion Set the estimated IMU orientation.
          *
          * This is to simulate an IMU with a built-in orientation estimation. If the IMU has no built-in orientation estimation, set each element to NAN
          *
          * @param qx
          * @param qy
          * @param qz
          * @param qs
          */
    virtual void setOrientationQuaternion(double qx, double qy, double qz, double qs) = 0;

    /**
          * @brief setLinearAcceleration Set the measured linear acceleration from the IMU
          * @param xdd
          * @param ydd
          * @param zdd
          */
    virtual void setLinearAcceleration(double xdd, double ydd, double zdd) = 0;


    /**
          * @brief setAngularVelocity Set the measured angular velocity from the IMU
          * @param wx
          * @param wy
          * @param wz
          */
    virtual void setAngularVelocity(double wx, double wy, double wz) = 0;
};


class DLL_EXPORT ForceTorqueSensorHandle
{
public:
    /**
             * @brief setForce Set the measured force of this force torque sensor
             * @param fx [N]
             * @param fy [N]
             * @param fz [N]
             */
    virtual void setForce(double fx, double fy, double fz) = 0;

    /**
             * @brief setTorque Set the measured torque of this force torque sensor
             * @param tx [Nm]
             * @param ty [Nm]
             * @param tz [Nm]
             */
    virtual void setTorque(double tx, double ty, double tz) = 0;

};

class DLL_EXPORT SharedBuffer
{
public:
    /**
     * @brief size size of the data
     *
     * Due to addressing inside the virtual machine, the maximum size is 31 bits (signed 32bit int).
     */
    virtual int32_t size() = 0;

    /**
     * @brief data Data array
     */
    virtual char* data() = 0;
};

class DLL_EXPORT HalodiController
{
public:
    /**
         * @brief HalodiController Create a new Halodi Controller
         * @param classpath Path to controller jar
         */
    static std::shared_ptr<HalodiController> create(std::string controllerName, std::string working_directory = ".");


    /**
         * @brief addJoint Register a joint to the controller
         * @param name Name of the joint as defined in the URDF
         * @return A shared pointer to the joint handle. nullptr if the joint is unknown to the controller
         */
    virtual std::shared_ptr<JointHandle> addJoint(std::string name) = 0;

    /**
         * @brief addIMU Register an IMU to the controller
         * @param parentLink Name of the parent link as defined in the URDF
         * @param name Name of the IMU as defined in the URDF
         * @return A shared pointer to the IMU handle. nullptr if the IMU is unknown to the controller
         */
    virtual std::shared_ptr<IMUHandle> addIMU(std::string parentLink, std::string name) = 0;


    /**
         * @brief addForceTorqueSensor Register a Force Torque sensor to the controller
         *
         * TODO: Not implemented
         *
         * @param parentLink Name of the parent link as defined in the URDF
         * @param name Name of the Force Torque sensor as defined in the URDF
         * @return A shared pointer to the Force Torque sensor. nullptr if the force-torque sensor in unknown to the controller
         */
    virtual std::shared_ptr<ForceTorqueSensorHandle> addForceTorqueSensor(std::string parentLink, std::string name) = 0;


    /**
         * @brief initialize Initialize the controller. Call after all handles are added to the controller.
         * @return false if the controller cannot initialize
         */
    virtual bool initialize(std::string arguments) = 0;


    /**
     * @brief start Start the controller. Call after initialize.
     * @return false if the controller cannot intiialize
     */
    virtual bool start() = 0;

    /**
         * @brief update Run the update loop of the controller. This copies the current states from the joint handles to the controller and sets the desired torque and damping from the controller.
         * @param timeInNanoseconds Monotonic clock time in nanoseconds
         * @param duration Duration of last cycle in nanoseconds (expected to be 1ms / 10^6 nanoseconds)
         *
         * @return false if the controller threw an exception
         */
    virtual bool update(long long timeInNanoseconds, long long duration) = 0;


    /**
     * @brief stop Stop the controller and reset to the initial condition.
     *
     * @return false if the controller was not able to stop cleanly
     */
    virtual bool stop() = 0;


    /**
     * @brief getControllerDescription Get a JSON description of the current controller setup
     *
     *
     * Data format:
     * {
     *  "initialized": [true/false],
     *  "joints": [],
     *  "imus": [],
     *  "forceTorqueSensors": []
     * }
     *
     * @return json description of controller setup
     */
    virtual std::string getControllerDescription() = 0;


    /**
     * @brief getControllerConfiguration Get a JSON description of the virtual machine configuration
     *
     * Data format:
     * {
     *      "name": ""
     *      "classPath": [ "" ],
     *      "mainClass": "",
     *      "vmArgs": [ "" ],
     *      "javaHome": ""
     * }
     *
     * @return json description of the Virtual machine configuration
     */
    virtual std::string getVirtualMachineConfiguration() = 0;


    /**
     * @brief attachCurrentThread Attach the current thread to the controller.
     *
     * The current thread has to be attached to the controller to be able to interact with the controller.
     *
     * Subsequent calls are no-ops
     */
    virtual void attachCurrentThread() = 0;

    /**
     * @brief deattachCurrentThread
     *
     * When possible, detach a thread when not used anymore.
     *
     * Note: Detatching and re-attaching is expensive, avoid as much as possible.
     */
    virtual void deattachCurrentThread() = 0;


    /**
     * @brief callController [Optional] Call a function on the controller
     *
     * @param request Request to be send to the controller.
     * @param arguments Arguments to the request
     * @return reply Reply from the controller
     */
    virtual std::string callController(std::string request, std::string arguments) = 0;


    /**
     * @brief createSharedBuffer [Optional] Create a buffer shared between the controller and the simulator
     *
     * Size and layout is to be negotiated using callController().
     *
     * @param name Name of the buffer
     * @param size Size in bytes
     * @return Pointer to shared buffer
     */
    virtual std::shared_ptr<SharedBuffer> createSharedBuffer(std::string name, int32_t size)  = 0;

};

}
