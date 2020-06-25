package com.halodi.controllerAPI.demo;

import com.halodi.controllerAPI.NativeHalodiControllerJavaBridge;

public class HalodiControllerDemo extends NativeHalodiControllerJavaBridge
{

   @Override
   public void initialize(String arguments)
   {
      System.out.println("Initialize with contorller arguments " + arguments);
   }

   @Override
   public void start()
   {
      System.out.println("Starting controller");
   }

   @Override
   public void doControl(long time, long duration)
   {
      System.out.println("Running tick " + time +  " for " + duration);
      
      System.out.println("\tShared buffer value:" + getSharedBuffer("DemoBuffer").getBuffer().getDouble(0));
      
      getJoint("joint1").setDesiredEffort(time * 2.0);
      getJoint("joint2").setDesiredEffort(-time);
      
      
      getSharedBuffer("DemoBuffer").getBuffer().putDouble(8, time * 4.0);
   }

   @Override
   public void stop()
   {
      System.out.println("Stopping controller");
   }

   @Override
   public void shutdown()
   {
      System.out.println("Shutting down controller");

   }

   @Override
   public String callController(String functionName, String arguments)
   {
      return "Called " +  functionName + " with arguments: " + arguments;
      
   }

}
