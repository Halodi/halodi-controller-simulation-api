#include <functional>
#include <memory>
#include <iostream>
#include <vector>

#include "gazebo/gazebo.hh"
#include "gazebo/common/common.hh"
#include "gazebo/physics/physics.hh"

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

    virtual ~GazeboJointHandle()
    {
    }

private:
    std::shared_ptr<JointHandle> controllerJoint;
    physics::JointPtr gazeboJoint;
};



class HalodiControllerPlugin : public ModelPlugin
{
public:
    void Load(physics::ModelPtr _model, sdf::ElementPtr /*_sdf*/)
    {
        this->model = _model;

        ControllerConfiguration config;

        config.mainClass = "com.halodi.controllerAPI.HalodiControllerJavaBridge";
        config.classPath = "/home/jesper/git/halodi/ros2_ws/src/halodi-controller-simulation-api/halodi-controller-simulation-api/bin/main";

        try
        {

            controller = HalodiController::create(config);


            for(auto &joint : this->model->GetJoints())
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
