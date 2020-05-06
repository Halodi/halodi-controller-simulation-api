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
      
      getJoint("joint1").setDesiredEffort(time * 2.0);
      getJoint("joint2").setDesiredEffort(-time);
      
   }

   @Override
   public void stop()
   {
      System.out.println("Stopping controller");
   }

   @Override
   public double getInitialJointAngle(String name)
   {
      return 0;
   }

   @Override
   public void shutdown()
   {
      System.out.println("Shutting down controller");

   }

}
