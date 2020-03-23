package com.halodi.controllerAPI.wholeRobot;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;

public class ForceTorqueSensorHandleImpl implements ForceTorqueSensorHandle
{
   private final String name;                  
   private final String parentLink;            
   

   private final ByteBuffer buffer;
   private final DoubleBuffer doubleBuffer;
   
   public ForceTorqueSensorHandleImpl(String parentLink, String name)
   {
      this.name = name;
      this.parentLink = parentLink;
      
      this.buffer = ByteBuffer.allocateDirect(6 * 8);
      this.buffer.order(ByteOrder.nativeOrder());

      this.doubleBuffer = this.buffer.asDoubleBuffer();
   }
   

   @Override
   public String getName()
   {
      return name;
   }

   @Override
   public double getFx()
   {
      return doubleBuffer.get(0);
   }

   @Override
   public double getFy()
   {
      return doubleBuffer.get(1);
   }

   @Override
   public double getFz()
   {
      return doubleBuffer.get(2);
   }

   @Override
   public double getTx()
   {
      return doubleBuffer.get(3);
   }

   @Override
   public double getTy()
   {
      return doubleBuffer.get(4);
   }

   @Override
   public double getTz()
   {
      return doubleBuffer.get(5);
   }

   @Override
   public String getParentLink()
   {
      return parentLink;
   }
  

   public ByteBuffer getBuffer()
   {
      return buffer;
   }
}
