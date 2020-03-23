package com.halodi.controllerAPI.wholeRobot;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;

public class EffortJointHandleImpl implements EffortJointHandle
{
   private final String jointName;

   private final ByteBuffer buffer;
   private final DoubleBuffer doubleBuffer;

   public EffortJointHandleImpl(String jointName)
   {
      this.jointName = jointName;
      this.buffer = ByteBuffer.allocateDirect(5 * 8);
      this.buffer.order(ByteOrder.nativeOrder());

      this.doubleBuffer = this.buffer.asDoubleBuffer();
   }

   @Override
   public double getPosition()
   {
      return doubleBuffer.get(0);
   }

   @Override
   public double getVelocity()
   {
      return doubleBuffer.get(1);
   }

   @Override
   public double getEffort()
   {
      return doubleBuffer.get(2);
   }

   @Override
   public void setDesiredEffort(double desiredEffort)
   {
      doubleBuffer.put(3, desiredEffort);
   }

   @Override
   public String getName()
   {
      return jointName;
   }

   @Override
   public void setDampingScale(double dampingScale)
   {
      doubleBuffer.put(4, dampingScale);
   }

   public ByteBuffer getBuffer()
   {
      return buffer;
   }
}
