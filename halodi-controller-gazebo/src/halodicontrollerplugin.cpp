#include <functional>
#include <memory>
#include <iostream>
#include <vector>

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
        controllerJoint->setMeasuredEffort(0.0);
    }
    void write()
    {
        gazeboJoint->SetForce(0, controllerJoint->getDesiredEffort());
        gazeboJoint->SetDamping(0, controllerJoint->getDampingScale());
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

        ControllerConfiguration config;

        config.mainClass = "com.halodi.controllerAPI.HalodiControllerJavaBridge";
        config.classPath = "/home/jesper/eve/lib/*";

        try
        {

            controller = HalodiController::create(config);


            for(auto &joint : this->model->GetJoints())
            {
                addJoint(joint);
            }


            sensors::SensorManager* mgr = sensors::SensorManager::Instance();

            for(sensors::SensorPtr &sensor : mgr->GetSensors())
            {
                std::cerr << sensor->Type() << std::endl;


                if("imu" == sensor->Type())
                {
                    addIMU(sensor);
                }
                else if ("force_torque" == sensor->Type())
                {
                    addForceTorqueSensor(sensor);
                }


            }


            if(controller->initialize())
            {
                firstTick = true;
                this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                            std::bind(&HalodiControllerPlugin::OnUpdate, this));

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

    void OnUpdate()
    {
        if(firstTick)
        {
            // Make sure this thread is attached to the controller
            controller->attachCurrentThread();
            firstTick = false;
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
        firstTick = true;
    }

    virtual ~HalodiControllerPlugin()
    {
        // Stop generating update() calls
        updateConnection.reset();

    }



private:

    void addJoint(physics::JointPtr joint)
    {
        if(joint->DOF() == 1)
        {

            auto controllerJoint = controller->addJoint(joint->GetName());
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

        auto controllerIMU = controller->addIMU(imuSensor->ParentName(), imuSensor->Name());
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

        auto controllerForceTorqueSensor = controller->addForceTorqueSensor(forceTorqueSensor->ParentName(), forceTorqueSensor->Name());
        updateables.push_back(std::make_shared<GazeboForceTorqueSensorHandle>(controllerForceTorqueSensor, forceTorqueSensor));
    }


    std::shared_ptr<HalodiController> controller;


    // Pointer to the model
    physics::ModelPtr model;

    // Pointer to the update event connection
    event::ConnectionPtr updateConnection;

    std::vector<std::shared_ptr<GazeboHandle>> updateables;


    long long lastUpdateTime = 0;

    bool firstTick = true;

};

GZ_REGISTER_MODEL_PLUGIN(HalodiControllerPlugin)
}
