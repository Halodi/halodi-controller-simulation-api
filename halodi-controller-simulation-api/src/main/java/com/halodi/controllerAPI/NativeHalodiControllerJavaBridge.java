package com.halodi.controllerAPI;

import java.nio.ByteBuffer;
import java.util.HashMap;

import com.halodi.controllerAPI.wholeRobot.EffortJointHandle;
import com.halodi.controllerAPI.wholeRobot.EffortJointHandleImpl;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandle;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandleImpl;
import com.halodi.controllerAPI.wholeRobot.IMUHandle;
import com.halodi.controllerAPI.wholeRobot.IMUHandleImpl;

public abstract class NativeHalodiControllerJavaBridge implements HalodiControllerJavaBridge, HalodiControllerElements
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

      imus.put(HalodiControllerElements.createQualifiedName(parentLink, imuName), imuHandle);

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
      forceTorqueSensors.put(HalodiControllerElements.createQualifiedName(parentLink, forceTorqueSensorName), forceTorqueSensorHandle);
      return forceTorqueSensorHandle.getBuffer();
   }

   boolean initFromNative()
   {
      try
      {
         init();
         return true;
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return false;
      }

   }

   void updateFromNative(long time, long duration)
   {
      try
      {
         doControl(time, duration);
      }
      catch (Throwable t)
      {
         t.printStackTrace();
      }

   }

   void resetFromNative()
   {
      try
      {
         reset();
      }
      catch (Throwable t)
      {
         t.printStackTrace();
      }
   }
   
   double getInitialJointAngleFromNative(String name)
   {
      try
      {
         return getInitialJointAngle(name);
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return 0.0;
      }
   }
      

   @Override
   public EffortJointHandle getJoint(String name)
   {
      return joints.get(name);
   }

   @Override
   public IMUHandle getIMU(String qualifiedName)
   {
      return imus.get(qualifiedName);
   }

   @Override
   public ForceTorqueSensorHandle getForceTorqueSensor(String qualifiedName)
   {
      return forceTorqueSensors.get(qualifiedName);
   }
   
}
