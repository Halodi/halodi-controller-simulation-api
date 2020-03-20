#pragma once

#include <string>
#include <memory>

namespace halodi_controller
{

     class JointHandle
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
             * @return Desired effort applied to this joint
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
     };

     class IMUHandle
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


    class HalodiController
    {
    public:
        /**
         * @brief HalodiController Create a new Halodi Controller
         * @param classpath Path to controller jar
         */
        HalodiController(std::string classpath);


        /**
         * @brief addJoint Register a joint to the controller
         * @param name Name of the joint as defined in the URDF
         * @return A shared pointer to the joint handle. nullptr if the joint is unknown to the controller
         */
        std::shared_ptr<JointHandle> addJoint(std::string name);

        /**
         * @brief addIMU Register an IMU to the controller
         * @param parentLink Name of the parent link as defined in the URDF
         * @param name Name of the IMU as defined in the URDF
         * @return A shared pointer to the IMU handle. nullptr if the IMU is unknown to the controller
         */
        std::shared_ptr<IMUHandle> addIMU(std::string parentLink, std::string name);


        /**
         * @brief initialize Initialize the controller. Call after all handles are added to the controller.
         */
        void initialize();

        /**
         * @brief update Run the update loop of the controller. This copies the current states from the joint handles to the controller and sets the desired torque and damping from the controller.
         */
        void update(long long timeInNanoseconds);

    };

}
