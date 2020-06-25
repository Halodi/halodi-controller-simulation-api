#include <functional>
#include <memory>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "gazebo/gazebo.hh"
#include "gazebo/common/common.hh"
#include "gazebo/physics/physics.hh"
#include "gazebo/sensors/SensorManager.hh"
#include "gazebo/sensors/Sensor.hh"
#include "gazebo/sensors/sensors.hh"

#include "halodi-controller/halodicontroller.h"


using namespace gazebo;

namespace halodi_controller
{

class GazeboHandle
{
public:
    virtual void read() = 0;
    virtual void write() = 0;
    virtual void initialize() = 0;
};

class GazeboJointHandle : public GazeboHandle
{
public:

    GazeboJointHandle(std::shared_ptr<JointHandle> controllerJoint_, physics::JointPtr gazeboJoint_) :
        controllerJoint(controllerJoint_),
        gazeboJoint(gazeboJoint_)
    {

    }

    void read()
    {
        controllerJoint->setPosition(gazeboJoint->Position());
        controllerJoint->setVelocity(gazeboJoint->GetVelocity(0));
        controllerJoint->setMeasuredEffort(gazeboJoint->GetForce(0));
    }
    void write()
    {
        gazeboJoint->SetForce(0, controllerJoint->getDesiredEffort());
        gazeboJoint->SetDamping(0, controllerJoint->getDampingScale());
    }

    void initialize()
    {
        std::cout << "Setting " << gazeboJoint->GetName() << " to  " << controllerJoint->getInitialAngle() << std::endl;
        gazeboJoint->SetPosition(0, controllerJoint->getInitialAngle());
    }

private:
    std::shared_ptr<JointHandle> controllerJoint;
    physics::JointPtr gazeboJoint;
};


class GazeboIMUhandle : public GazeboHandle
{
public:

    GazeboIMUhandle(std::shared_ptr<IMUHandle> controllerIMU_, sensors::ImuSensorPtr gazeboIMU_) :
        controllerIMU(controllerIMU_),
        gazeboIMU(gazeboIMU_)
    {

    }

    void read()
    {
        ignition::math::Quaterniond orientation = gazeboIMU->Orientation();
        controllerIMU->setOrientationQuaternion(orientation.X(), orientation.Y(), orientation.Z(), orientation.W());

        ignition::math::Vector3d accel = gazeboIMU->LinearAcceleration(false);
        controllerIMU->setLinearAcceleration(accel.X(), accel.Y(), accel.Z());

        ignition::math::Vector3d gyro = gazeboIMU->AngularVelocity(false);
        controllerIMU->setAngularVelocity(gyro.X(), gyro.Y(), gyro.Z());

    }
    void write()
    {
    }

    void initialize()
    {
    }

private:
    std::shared_ptr<IMUHandle> controllerIMU;
    sensors::ImuSensorPtr gazeboIMU;
};

class GazeboForceTorqueSensorHandle : public GazeboHandle
{
public:

    GazeboForceTorqueSensorHandle(std::shared_ptr<ForceTorqueSensorHandle> controllerForceTorqueSensor_, sensors::ForceTorqueSensorPtr gazeboForceTorqueSensor_) :
        controllerForceTorqueSensor(controllerForceTorqueSensor_),
        gazeboForceTorqueSensor(gazeboForceTorqueSensor_)
    {

    }

    void read()
    {
        ignition::math::Vector3d force = gazeboForceTorqueSensor->Force();
        controllerForceTorqueSensor->setForce(force.X(), force.Y(), force.Z());

        ignition::math::Vector3d torque = gazeboForceTorqueSensor->Torque();
        controllerForceTorqueSensor->setTorque(torque.X(), torque.Y(), torque.Z());

    }
    void write()
    {
    }

    void initialize()
    {
    }

private:
    std::shared_ptr<ForceTorqueSensorHandle> controllerForceTorqueSensor;
    sensors::ForceTorqueSensorPtr gazeboForceTorqueSensor;
};




class HalodiControllerPlugin : public ModelPlugin
{
public:
    void Load(physics::ModelPtr _model, sdf::ElementPtr /*_sdf*/)
    {
        this->model = _model;



        try
        {

            std::string controllerArguments = "--pubsub FAST_RTPS_SYSTEM_LIBRARY";

            if(const char* halodi_trajectory_api = std::getenv("HALODI_TRAJECTORY_API"))
            {
                bool enableApi = std::string(halodi_trajectory_api) == "true";
                if(!enableApi)
                {
                    std::cout << "Trajectory API: Disabled" << std::endl;
                    controllerArguments += " --no-trajectory";
                }
                else
                {
                    std::cout << "Trajectory API: Enabled" << std::endl;
                }
            }

            controller = HalodiController::create("NativePluginEveSimulation");


            for(auto &joint : this->model->GetJoints())
            {
                addJoint(joint);
            }


            sensors::SensorManager* mgr = sensors::SensorManager::Instance();

            for(sensors::SensorPtr &sensor : mgr->GetSensors())
            {
                if("imu" == sensor->Type())
                {
                    addIMU(sensor);
                }
                else if ("force_torque" == sensor->Type())
                {
                    addForceTorqueSensor(sensor);
                }


            }


            if(controller->initialize(controllerArguments))
            {
                for(auto updatable : updateables)
                {
//                    updatable->initialize();
                }

                if(controller->start())
                {

                    reattachController = true;
                    this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                                std::bind(&HalodiControllerPlugin::OnUpdate, this));
                    this->pauseConnection = event::Events::ConnectPause(
                                std::bind(&HalodiControllerPlugin::OnPause, this));
                }
                else
                {
                    controller.reset();
                    std::cerr << "Cannot start controller." << std::endl;
                }

            }
            else
            {
                std::cerr << "Cannot initialize controller. Disabling OnUpdate" << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            std::cerr << "Cannot load controller. Disabling OnUpdate" << std::endl;
        }

    }

    void OnPause()
    {
        reattachController = true;
    }

    void OnUpdate()
    {
        if(reattachController)
        {
            // Make sure this thread is attached to the controller
            controller->attachCurrentThread();
            reattachController = false;
        }


        for(auto updatable : updateables)
        {
            updatable->read();
        }

        common::Time time = model->GetWorld()->SimTime();


        long long rawTime = ((long long)common::Time::nsInSec) * ((long long)time.sec) + ((long long) time.nsec);
        long long dt = lastUpdateTime - rawTime;



        controller->update(rawTime, dt);


        for(auto updatable : updateables)
        {
            updatable->write();
        }

        lastUpdateTime = rawTime;

    }

    void Reset()
    {
        reattachController = true;
    }

    virtual ~HalodiControllerPlugin()
    {
        controller->attachCurrentThread();
        if(pauseConnection)
        {
            pauseConnection.reset();
        }

        if(updateConnection)
        {
            // Stop generating update() calls
            updateConnection.reset();

            controller->stop();
        }

        controller->deattachCurrentThread();
    }



private:

    std::string removeScope(std::string scopedName)
    {
        size_t lastDelim = scopedName.rfind("::");

        if(lastDelim < scopedName.npos)
        {
            return scopedName.substr(lastDelim + 2);
        }
        else
        {
            return scopedName;
        }
    }

    void addJoint(physics::JointPtr joint)
    {
        if(joint->DOF() == 1)
        {
            auto controllerJoint = controller->addJoint(removeScope(joint->GetName()));
            updateables.push_back(std::make_shared<GazeboJointHandle>(controllerJoint, joint));
        }
        else
        {
            std::cerr << joint->GetName() << " is not a OneDoFJoint, ignoring.";
        }
    }

    void addIMU(sensors::SensorPtr sensor)
    {
        sensors::ImuSensorPtr imuSensor = std::dynamic_pointer_cast<sensors::ImuSensor>(sensor);

        if(!imuSensor)
        {
            std::cerr << sensor->Name() << " is not of type ImuSensor" << std::endl;
            return;
        }

        auto controllerIMU = controller->addIMU(removeScope(imuSensor->ParentName()), removeScope(imuSensor->Name()));
        updateables.push_back(std::make_shared<GazeboIMUhandle>(controllerIMU, imuSensor));
    }

    void addForceTorqueSensor(sensors::SensorPtr sensor)
    {
        sensors::ForceTorqueSensorPtr forceTorqueSensor = std::dynamic_pointer_cast<sensors::ForceTorqueSensor>(sensor);

        if(!forceTorqueSensor)
        {
            std::cerr << sensor->Name() << " is not of type ForceTorqueSensor" << std::endl;
            return;
        }

        auto controllerForceTorqueSensor = controller->addForceTorqueSensor(removeScope(forceTorqueSensor->ParentName()), removeScope(forceTorqueSensor->Name()));
        updateables.push_back(std::make_shared<GazeboForceTorqueSensorHandle>(controllerForceTorqueSensor, forceTorqueSensor));
    }


    std::shared_ptr<HalodiController> controller;


    // Pointer to the model
    physics::ModelPtr model;

    // Pointer to the update event connection
    event::ConnectionPtr updateConnection;
    event::ConnectionPtr pauseConnection;

    std::vector<std::shared_ptr<GazeboHandle>> updateables;


    long long lastUpdateTime = 0;

    bool reattachController = true;

};

GZ_REGISTER_MODEL_PLUGIN(HalodiControllerPlugin)
}
