package com.halodi.controllerAPI;

import java.nio.ByteBuffer;
import java.util.HashMap;

import com.halodi.controllerAPI.wholeRobot.EffortJointHandle;
import com.halodi.controllerAPI.wholeRobot.EffortJointHandleImpl;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandle;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandleImpl;
import com.halodi.controllerAPI.wholeRobot.IMUHandle;
import com.halodi.controllerAPI.wholeRobot.IMUHandleImpl;

public class HalodiControllerJavaBridge
{     
   private final HashMap<String, EffortJointHandle> joints = new HashMap<>();
   private final HashMap<String, IMUHandle> imus = new HashMap<>();
   private final HashMap<String, ForceTorqueSensorHandle> forceTorqueSensors = new HashMap<>();
   
   
   /**
    * Create a new JointStateHandle. Called from native layer
    *
    * @param jointName
    * @return
    */
   ByteBuffer createEffortJointHandle(String jointName)
   {
      EffortJointHandleImpl jointHandle = new EffortJointHandleImpl(jointName);
      joints.put(jointName, jointHandle);
      return jointHandle.getBuffer();
   }


   
   /**
    * Return a new IMUHandle. Called from native layer
    * 
    * @param imuName
    * @return
    */
   ByteBuffer createIMUHandle(String parentLink, String imuName)
   {
      IMUHandleImpl imuHandle = new IMUHandleImpl(parentLink, imuName);
      
      imus.put(parentLink + "_" + imuName, imuHandle);
      
      return imuHandle.getBuffer();
   }

   /**
    * Return a new force torque sensor handle. Called from native layer
    * 
    * @param forceTorqueSensorName
    * @return
    */
   ByteBuffer createForceTorqueSensorHandle(String parentLink, String forceTorqueSensorName)
   {
      ForceTorqueSensorHandleImpl forceTorqueSensorHandle = new ForceTorqueSensorHandleImpl(parentLink, forceTorqueSensorName);
      forceTorqueSensors.put(parentLink + "_" + forceTorqueSensorName, forceTorqueSensorHandle);
      return forceTorqueSensorHandle.getBuffer();
   }
   
   
   

   boolean initFromNative()
   {
      System.out.println("Calling init from native");
      System.out.println(joints);
      System.out.println(imus);
      System.out.println(forceTorqueSensors);
      
      return true;
   }
   
   void updateFromNative(long time, long duration)
   {
      System.out.println(time + " " + duration);
   }
   
   void resetFromNative()
   {
      System.out.println("RESET");
   }
   
//   
//   /**
//    * This function gets called when the controller gets initialized. Use this function to setup
//    * the resource interfaces.
//    * 
//    * If an Exception gets thrown, the controller will shutdown.
//    */
//   protected abstract void init();
//   
//   /**
//    * This method gets called cyclically in the update.
//    * 
//    * Exceptions will get caught and output to ROS_ERROR. Execution will continue.
//    * 
//    */
//   protected abstract void doControl(long time, long duration);
   
   
}
