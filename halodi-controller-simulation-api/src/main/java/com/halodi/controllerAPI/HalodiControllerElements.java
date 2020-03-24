package com.halodi.controllerAPI;

import com.halodi.controllerAPI.wholeRobot.EffortJointHandle;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandle;
import com.halodi.controllerAPI.wholeRobot.IMUHandle;

public interface HalodiControllerElements
{
   static String createQualifiedName(String parentLink, String name)
   {
      return parentLink + "_" + name;
   }

   EffortJointHandle getJoint(String name);

   IMUHandle getIMU(String parentLink, String name);

   ForceTorqueSensorHandle getForceTorqueSensor(String parentLink, String name);

}
