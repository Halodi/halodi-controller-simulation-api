package com.halodi.controllerAPI.wholeRobot;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class SharedBufferImpl implements SharedBuffer
{

   private final String name;
   private final ByteBuffer buffer;

   public SharedBufferImpl(String name, int size)
   {
      this.name = name;

      this.buffer = ByteBuffer.allocateDirect(size);
      this.buffer.order(ByteOrder.nativeOrder());
   }

   @Override
   public String getName()
   {
      return name;
   }

   @Override
   public ByteBuffer getBuffer()
   {
      return buffer;
   }

}
