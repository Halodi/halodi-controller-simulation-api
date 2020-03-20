package us.ihmc.rosControl.wholeRobot;

import java.nio.ByteBuffer;

import us.ihmc.rosControl.NativeUpdateableInterface;

public class EffortJointHandleImpl implements EffortJointHandle, NativeUpdateableInterface
{
   private final String jointName;
   
   private double effort;
   private double position;
   private double velocity;
   
   private double desiredEffort;
   private double dampingScale;
   
   
   public EffortJointHandleImpl(String jointName)
   {
      this.jointName = jointName;
   }

   public void readFromBuffer(ByteBuffer buffer)
   {
      effort = buffer.getDouble();
      position = buffer.getDouble();
      velocity = buffer.getDouble();
   }
   
   public void writeToBuffer(ByteBuffer buffer)
   {
      buffer.putDouble(desiredEffort);
      buffer.putDouble(dampingScale);
   }
   
   
   @Override
   public double getEffort()
   {
      return effort;
   }

   @Override
   public double getPosition()
   {
      return position;
   }

   @Override
   public double getVelocity()
   {
      return velocity;
   }

   @Override
   public void setDesiredEffort(double desiredEffort)
   {
      this.desiredEffort = desiredEffort;
   }

   @Override
   public String getName()
   {
      return jointName;
   }

   @Override
   public void setDampingScale(double dampingScale)
   {
      this.dampingScale = dampingScale;
   }
   
}
