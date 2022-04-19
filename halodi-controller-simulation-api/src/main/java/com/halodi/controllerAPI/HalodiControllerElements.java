package com.halodi.controllerAPI;

import com.halodi.controllerAPI.wholeRobot.EffortJointHandle;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandle;
import com.halodi.controllerAPI.wholeRobot.IMUHandle;
import com.halodi.controllerAPI.wholeRobot.SharedBuffer;

public interface HalodiControllerElements
{
   EffortJointHandle getJoint(String name);

   default IMUHandle getIMU(String parentLink, String name)
   {
      return getIMU(name);
   }

   IMUHandle getIMU(String qualifiedName);

   default ForceTorqueSensorHandle getForceTorqueSensor(String parentLink, String name)
   {
      return getForceTorqueSensor(name);
   }
   
   ForceTorqueSensorHandle getForceTorqueSensor(String qualifiedName);
   
   
   default SharedBuffer getSharedBuffer(String name)
   {
      return null;
   }

   
}
