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
    virtual void read();
    virtual void write();
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



class HalodiControllerPlugin : public ModelPlugin
{
public:
    void Load(physics::ModelPtr _model, sdf::ElementPtr /*_sdf*/)
    {
        this->model = _model;


        ControllerConfiguration config;

        config.mainClass = "com.halodi.controllerAPI.HalodiControllerJavaBridge";
        config.classPath = "/home/jesper/git/halodi/ros2_ws/src/halodi-controller-simulation-api/halodi-controller-simulation-api/bin/main";


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



        this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                    std::bind(&HalodiControllerPlugin::OnUpdate, this));

    }

    void OnUpdate()
    {
        for(auto updatable : updatables)
        {
            updatable->read();
        }

        controller->update();


        for(auto updatable : updatables)
        {
            updatable->write();
        }
    }





private:

    std::shared_ptr<HalodiController> controller;


    // Pointer to the model
    physics::ModelPtr model;

    // Pointer to the update event connection
    event::ConnectionPtr updateConnection;

    std::vector<std::shared_ptr<GazeboHandle>> updateables;
};

GZ_REGISTER_MODEL_PLUGIN(HalodiControllerPlugin)
}
