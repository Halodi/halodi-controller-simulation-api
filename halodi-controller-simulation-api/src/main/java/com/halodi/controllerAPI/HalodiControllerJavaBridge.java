package com.halodi.controllerAPI;

public interface HalodiControllerJavaBridge
{
   /**
    * This function gets called when the controller gets initialized. Use this function to setup the
    * resource interfaces. If an Exception gets thrown, the controller will shutdown.
    */
   void init();

   /**
    * This method gets called cyclically in the update. Exceptions will get caught and printed.
    * Execution will continue.
    */
   void doControl(long time, long duration);

   /**
    * (Optional) Reset the controller to its initial state
    */
   default void reset()
   {

   }
   
   double getInitialJointAngle(String name);
}
