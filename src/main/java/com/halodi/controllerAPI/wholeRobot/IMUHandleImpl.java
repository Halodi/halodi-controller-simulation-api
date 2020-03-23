package com.halodi.controllerAPI.wholeRobot;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;

public class IMUHandleImpl implements IMUHandle
{
   private final String name;                  
   private final String parentLink;            
   

   private final ByteBuffer buffer;
   private final DoubleBuffer doubleBuffer;
   
   
   
   public IMUHandleImpl(String parentLink, String name)
   {
      this.name = name;
      this.parentLink = parentLink;
      
      this.buffer = ByteBuffer.allocateDirect(10 * 8);
      this.buffer.order(ByteOrder.nativeOrder());

      this.doubleBuffer = this.buffer.asDoubleBuffer();

   }
   
   @Override
   public String getName()
   {
      return name;
   }

   @Override
   public double getQ_x()
   {
      return doubleBuffer.get(0);
   }

   @Override
   public double getQ_y()
   {
      return doubleBuffer.get(1);
   }

   @Override
   public double getQ_z()
   {
      return doubleBuffer.get(2);
   }

   @Override
   public double getQ_w()
   {
      return doubleBuffer.get(3);
   }


   @Override
   public double getTheta_x()
   {
      return doubleBuffer.get(7);
   }

   @Override
   public double getTheta_y()
   {
      return doubleBuffer.get(8);
   }

   @Override
   public double getTheta_z()
   {
      return doubleBuffer.get(9);
   }

   @Override
   public double getXdd()
   {
      return doubleBuffer.get(4);
   }

   @Override
   public double getYdd()
   {
      return doubleBuffer.get(5);
   }

   @Override
   public double getZdd()
   {
      return doubleBuffer.get(6);
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
