#include <pybind11/pybind11.h>
#include "halodi-controller/halodicontroller.h"

namespace halodi_controller {

/**
 * @brief Python binding for a JointHandle object
 *
 */
class PyJointHandle {
 public:
  PyJointHandle(const std::shared_ptr<JointHandle>& joint_handle) { this->joint_ = joint_handle; }

  void setPosition(double position) { this->joint_->setPosition(position); }
  void setVelocity(double velocity) { this->joint_->setVelocity(velocity); }
  void setMeasuredEffort(double effort) { this->joint_->setMeasuredEffort(effort); }

  double getDesiredEffort() { return this->joint_->getDesiredEffort(); }
  double getDampingScale() { return this->joint_->getDampingScale(); }
  double getInitialAngle() { return this->joint_->getInitialAngle(); }

 private:
  std::shared_ptr<JointHandle> joint_;
};

/**
 * @brief Python binding for an IMUHandle object
 *
 */
class PyIMUHandle {
 public:
  PyIMUHandle(const std::shared_ptr<IMUHandle>& imu_handle) { this->imu_ = imu_handle; }

  void setOrientationQuaternion(double qx, double qy, double qz, double qs) {
    this->imu_->setOrientationQuaternion(qx, qy, qz, qs);
  }

  void setLinearAcceleration(double xdd, double ydd, double zdd) { this->imu_->setLinearAcceleration(xdd, ydd, zdd); }
  void setAngularVelocity(double wx, double wy, double wz) { this->imu_->setAngularVelocity(wx, wy, wz); }

 private:
  std::shared_ptr<IMUHandle> imu_;
};

/**
 * @brief Python binding for a ForceTorqueSensor object
 *
 */
class PyForceTorqueSensorHandle {
 public:
  PyForceTorqueSensorHandle(const std::shared_ptr<ForceTorqueSensorHandle>& force_torque_sensor_handle) {
    this->force_torque_sensor_ = force_torque_sensor_handle;
  }

  void setForce(double fx, double fy, double fz) { this->force_torque_sensor_->setForce(fx, fy, fz); }

  void setTorque(double tx, double ty, double tz) { this->force_torque_sensor_->setTorque(tx, ty, tz); }

 private:
  std::shared_ptr<ForceTorqueSensorHandle> force_torque_sensor_;
};

/**
 * @brief Python binding for a HalodiController object
 *
 */
class PyHalodiController {
 public:
  PyHalodiController(std::string controller_name) { this->controller = HalodiController::create(controller_name); }

  static PyHalodiController create(std::string controller_name) { return PyHalodiController(controller_name); }

  PyJointHandle addJoint(std::string name) {
    auto joint_ptr = this->controller->addJoint(name);
    return PyJointHandle(joint_ptr);
  }

  PyIMUHandle addIMU(std::string parent_link, std::string name) {
    auto imu_ptr = this->controller->addIMU(parent_link, name);
    return PyIMUHandle(imu_ptr);
  }

  PyForceTorqueSensorHandle addForceTorqueSensor(std::string parent_link, std::string name) {
    auto force_torque_sensor_ptr = this->controller->addForceTorqueSensor(parent_link, name);
    return PyForceTorqueSensorHandle(force_torque_sensor_ptr);
  }

  bool initialize(std::string arguments) { return this->controller->initialize(arguments); }
  bool start() { return this->controller->start(); }

  bool update(long long time_in_nanosec, long long duration) {
    return this->controller->update(time_in_nanosec, duration);
  }

  bool stop() { return this->controller->stop(); }

  std::string getControllerDescription() { return this->controller->getControllerDescription(); }
  std::string getVirtualMachineConfiguration() { return this->controller->getVirtualMachineConfiguration(); }

  void attachCurrentThread() { this->controller->attachCurrentThread(); }
  void deattachCurrentThread() { this->controller->deattachCurrentThread(); }

  std::string callController(std::string request, std::string arguments) {
    return this->controller->callController(request, arguments);
  }

 private:
  std::shared_ptr<HalodiController> controller;
};

PYBIND11_MODULE(halodi_controller_python, m) {
  m.doc() = "pybind11 plugin for the Halodi controller";

  pybind11::class_<PyJointHandle>(m, "PyJointHandle")
      .def("setPosition", &PyJointHandle::setPosition)
      .def("setVelocity", &PyJointHandle::setVelocity)
      .def("setMeasuredEffort", &PyJointHandle::setMeasuredEffort)
      .def("getDesiredEffort", &PyJointHandle::getDesiredEffort)
      .def("getDampingScale", &PyJointHandle::getDampingScale)
      .def("getInitialAngle", &PyJointHandle::getInitialAngle);

  pybind11::class_<PyIMUHandle>(m, "PyIMUHandle")
      .def("setOrientationQuaternion", &PyIMUHandle::setOrientationQuaternion)
      .def("setLinearAcceleration", &PyIMUHandle::setLinearAcceleration)
      .def("setAngularVelocity", &PyIMUHandle::setAngularVelocity);

  pybind11::class_<PyForceTorqueSensorHandle>(m, "PyForceTorqueSensorHandle")
      .def("setForce", &PyForceTorqueSensorHandle::setForce)
      .def("setTorque", &PyForceTorqueSensorHandle::setTorque);

  pybind11::class_<PyHalodiController>(m, "PyHalodiController")
      .def(pybind11::init(&PyHalodiController::create))
      .def("addJoint", &PyHalodiController::addJoint)
      .def("addIMU", &PyHalodiController::addIMU)
      .def("addForceTorqueSensor", &PyHalodiController::addForceTorqueSensor)
      .def("initialize", &PyHalodiController::initialize)
      .def("start", &PyHalodiController::start)
      .def("update", &PyHalodiController::update)
      .def("stop", &PyHalodiController::stop)
      .def("getControllerDescription", &PyHalodiController::getControllerDescription)
      .def("getVirtualMachineConfiguration", &PyHalodiController::getVirtualMachineConfiguration)
      .def("attachCurrentThread", &PyHalodiController::attachCurrentThread)
      .def("deattachCurrentThread", &PyHalodiController::deattachCurrentThread)
      .def("callController", &PyHalodiController::callController);
}

}  // namespace halodi_controller