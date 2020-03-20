package us.ihmc.rosControl.demo;

import us.ihmc.rosControl.wholeRobot.EffortJointHandle;
import us.ihmc.rosControl.wholeRobot.EffortJointHandleImpl;
import us.ihmc.rosControl.wholeRobot.ForceTorqueSensorHandle;
import us.ihmc.rosControl.wholeRobot.ForceTorqueSensorHandleImpl;
import us.ihmc.rosControl.wholeRobot.IHMCWholeRobotControlJavaBridge;
import us.ihmc.rosControl.wholeRobot.IMUHandle;
import us.ihmc.rosControl.wholeRobot.IMUHandleImpl;

public class WholeRobotJavaDemo extends IHMCWholeRobotControlJavaBridge
{
   private final static String jointName = "leftKneePitch";
   private EffortJointHandle joint;

   private double q;
   private double direction;
   
   private boolean firstTick = true;
   
   @Override
   protected void init()
   {
      joint = createEffortJointHandle(jointName);
   }

   @Override
   protected void doControl(long time, long duration)
   {
      if(firstTick)
      {
         q = joint.getPosition();
         firstTick = false;
      }
      
      q += 0.001 * direction;
      if(q <= 0.0)
      {
         direction = 1.0;
      }
      else if(q > 1.0)
      {
         direction = -1.0;
      }
      
      double tau = 1000 * (q - joint.getPosition()) + 10 * (0 - joint.getVelocity());
      joint.setDesiredEffort(tau);

   }

   /**
    * Return a new JointStateHandle. Call from init()
    *
    * @param jointName
    * @return
    */
   protected final EffortJointHandle createEffortJointHandle(String jointName)
   {
      if(!inInit())
      {
         throw new RuntimeException("createEffortJointHandle should only be called from init()");
      }
      if(!addJointToBufferN(getDelegatePtr(), jointName))
      {
         throw new IllegalArgumentException("Cannot find joint with name " + jointName);
      }
      EffortJointHandleImpl jointHandle = new EffortJointHandleImpl(jointName);
      addUpdatable(jointHandle);
      return jointHandle;
   }

   /**
    * Return a new IMUHandle. Call from init()
    * 
    * @param imuName
    * @return
    */
   protected final IMUHandle createIMUHandle(String imuName)
   {
      if(!inInit())
      {
         throw new RuntimeException("createIMUHandle should only be called from init()");
      }
      if(!addIMUToBufferN(getDelegatePtr(), imuName))
      {
         throw new IllegalArgumentException("Cannot find IMU with name " + imuName);
      }
      IMUHandleImpl imuHandle = new IMUHandleImpl(imuName);
      addUpdatable(imuHandle);
      return imuHandle;
   }

   /**
    * Return a new force torque sensor handle. Call from init()
    * 
    * @param forceTorqueSensorName
    * @return
    */
   protected final ForceTorqueSensorHandle createForceTorqueSensorHandle(String forceTorqueSensorName)
   {
      if(!inInit())
      {
         throw new RuntimeException("createForceTorqueSensorHandle should only be called from init()");
      }
      if(!addForceTorqueSensorToBufferN(getDelegatePtr(), forceTorqueSensorName))
      {
         throw new IllegalArgumentException("Cannot find force torque sensor with name " + forceTorqueSensorName);
      }
      ForceTorqueSensorHandleImpl imuHandle = new ForceTorqueSensorHandleImpl(forceTorqueSensorName);
      addUpdatable(imuHandle);
      return imuHandle;
   }

}
