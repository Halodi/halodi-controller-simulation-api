package com.halodi.controllerAPI;

public interface HalodiControllerJavaBridge
{
   /**
    * This function gets called when the plugin gets initialized.
    * 
    *  This function only gets called once.
    * 
    * Use this function to setup long-lived resource interfaces. If an Exception gets thrown, the controller will shutdown.
    */
   void initialize(String arguments);
   
   
   /**
    * Start the controller plugin.
    * 
    * This function gets called after the joints and sensors are setup.
    * 
    *  This function can get called multiple times, after stop()
    */
   void start();
   

   /**
    * This method gets called cyclically in the update. Exceptions will get caught and printed.
    * Execution will continue.
    */
   void doControl(long time, long duration);
   
   /**
    * Stop the controller. Allow for restart using start()
    * 
    * After stop() is called, all sensor and joint ahndles are cleared.
    */
   void stop();
   
   /**
    * Shutdown the controller. 
    */
   void shutdown();
   
   double getInitialJointAngle(String name);
}
