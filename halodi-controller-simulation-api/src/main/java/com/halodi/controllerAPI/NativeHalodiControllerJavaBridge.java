package com.halodi.controllerAPI;

import java.io.PrintStream;
import java.nio.ByteBuffer;
import java.nio.file.StandardOpenOption;
import java.util.HashMap;
import java.util.Set;
import java.util.stream.Collectors;

import com.halodi.controllerAPI.util.CallbackPrintStream;
import com.halodi.controllerAPI.wholeRobot.EffortJointHandle;
import com.halodi.controllerAPI.wholeRobot.EffortJointHandleImpl;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandle;
import com.halodi.controllerAPI.wholeRobot.ForceTorqueSensorHandleImpl;
import com.halodi.controllerAPI.wholeRobot.IMUHandle;
import com.halodi.controllerAPI.wholeRobot.IMUHandleImpl;
import com.halodi.controllerAPI.wholeRobot.SharedBuffer;
import com.halodi.controllerAPI.wholeRobot.SharedBufferImpl;

public abstract class NativeHalodiControllerJavaBridge implements HalodiControllerJavaBridge, HalodiControllerElements
{
   private boolean initialized;
   private String controllerArguments;
   
   private Object nativeLock = new Object();
   private long nativePtr = 0;
   
   private final HashMap<String, EffortJointHandleImpl> joints = new HashMap<>();
   private final HashMap<String, IMUHandleImpl> imus = new HashMap<>();
   private final HashMap<String, ForceTorqueSensorHandleImpl> forceTorqueSensors = new HashMap<>();
   private final HashMap<String, SharedBufferImpl> sharedBuffers = new HashMap<>();
   
   
   private static native void outputHandler(long ptr, boolean stderr, String message);
   
   void redirectOutput(long ptr)
   {
      synchronized (nativeLock)
      {
         this.nativePtr = ptr;
      }
      
      System.setOut(new CallbackPrintStream()
      {
         
         @Override
         public void outputLine(String message)
         {
            log(false, message);
         }
      });
      
      System.setErr(new CallbackPrintStream()
      {
         
         @Override
         public void outputLine(String message)
         {
            log(true, message);
         }
      });
      
      System.out.println("Calling from Java");
      System.err.println("STD Err from java");
   }
   
   public void log(boolean stderr, String out)
   {
      synchronized(nativeLock)
      {
         if(nativePtr != 0)
         {
            outputHandler(nativePtr, stderr, out);
         }
      }
   }
   

   /**
    * Create a new JointStateHandle. Called from native layer
    *
    * @param jointName
    * @return
    */
   synchronized ByteBuffer createEffortJointHandle(String jointName)
   {
      if(joints.containsKey(jointName))
      {
         return joints.get(jointName).getBuffer();
      }
      
      EffortJointHandleImpl jointHandle = new EffortJointHandleImpl(jointName, getInitialJointAngle(jointName));
      
      joints.put(jointName, jointHandle);
      return jointHandle.getBuffer();
   }

   /**
    * Return a new IMUHandle. Called from native layer
    * 
    * @param imuName
    * @return
    */
   synchronized ByteBuffer createIMUHandle(String parentLink, String imuName)
   {
      String name = HalodiControllerElements.createQualifiedName(parentLink, imuName);
      if(imus.containsKey(name))
      {
         return imus.get(name).getBuffer();
      }
      
      IMUHandleImpl imuHandle = new IMUHandleImpl(parentLink, imuName);
      imus.put(name, imuHandle);
      return imuHandle.getBuffer();
   }

   /**
    * Return a new force torque sensor handle. Called from native layer
    * 
    * @param forceTorqueSensorName
    * @return
    */
   synchronized ByteBuffer createForceTorqueSensorHandle(String parentLink, String forceTorqueSensorName)
   {
      String name = HalodiControllerElements.createQualifiedName(parentLink, forceTorqueSensorName);
      
      if(forceTorqueSensors.containsKey(name))
      {
         return forceTorqueSensors.get(name).getBuffer();
      }
      
      ForceTorqueSensorHandleImpl forceTorqueSensorHandle = new ForceTorqueSensorHandleImpl(parentLink, forceTorqueSensorName);
      forceTorqueSensors.put(name, forceTorqueSensorHandle);
      return forceTorqueSensorHandle.getBuffer();
   }
   
   
   /**
    * Return a shared buffer. Called from native layer
    * 
    * @param name
    * @param size
    * @return
    */
   synchronized ByteBuffer createSharedBuffer(String name, int size)
   {
      
      if(sharedBuffers.containsKey(name))
      {
         if(sharedBuffers.get(name).getBuffer().capacity() < size)
         {
            System.err.println("Requested buffer with name " + name + " already exists but is smaller than requested size.");
            return null;
         }
         
         return sharedBuffers.get(name).getBuffer();
      }
      
      SharedBufferImpl sharedBuffer = new SharedBufferImpl(name, size);
      sharedBuffers.put(name, sharedBuffer);
      return sharedBuffer.getBuffer();
   }
   
   

   synchronized boolean initFromNative(String arguments)
   {
      if(initialized)
      {
         System.err.println("[NativeHalodiControllerJavaBridge] Controller already initialized. Cannot initialize twice.");
         return false;
      }
      
      try
      {        
         initialize(arguments);
         initialized = true;
         this.controllerArguments = arguments;
         
         return true;
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return false;
      }

   }
   
   
   synchronized boolean startFromNative()
   {
      if(!initialized)
      {
         System.err.println("[NativeHalodiControllerJavaBridge] Controller not initialized. Cannot start controller.");
         return false;
      }
      
      try
      {        
         start();
         
         return true;
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return false;
      }
      
   }

   boolean updateFromNative(long time, long duration)
   {
      try
      {
         doControl(time, duration);
         return true;
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return false;
      }

   }

   boolean stopFromNative()
   {
      try
      {
         stop();
         return true;
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return false;
      }
   }
   
   void shutdownFromNative()
   {
      try
      {
         shutdown();
      }
      catch (Throwable t)
      {
         t.printStackTrace();
      } 
      
      synchronized(nativeLock)
      {
         nativePtr = 0;
      }
   }
   
   String callControllerFromNative(String request, String arguments)
   {
      try
      {
         return callController(request, arguments);
      }
      catch (Throwable t)
      {
         t.printStackTrace();
         return "";
      }
   }
   
   
   
   private void addSetToJSON(StringBuilder json, String name, Set<String> elements)
   {
      json.append('"');
      json.append(name);
      json.append('"');
      json.append(':');
      
      if(elements.size() > 0)
      {
         json.append("[\"");
         json.append(elements.stream().collect(Collectors.joining("\", \"")));
         json.append("\"]");
      }
      else
      {
         json.append("[]");
      }
      
   }
   
   
   /**
    * @return A JSON description of the controller data
    */
   String getControllerDescriptionFromNative()
   {
      StringBuilder json = new StringBuilder();
      json.append("{");
      
      json.append("\"initialized\":\""); 
      json.append(initialized); 
      json.append("\"");
      
      json.append(',');
      
      json.append("\"arguments\":\"");
      json.append(controllerArguments);
      json.append("\"");
      
      json.append(',');
      
      addSetToJSON(json, "joints", joints.keySet());
      json.append(',');
      addSetToJSON(json, "imus", imus.keySet());
      json.append(',');
      addSetToJSON(json, "forceTorqueSensors", forceTorqueSensors.keySet());
      json.append(',');
      addSetToJSON(json, "sharedBuffers", sharedBuffers.keySet());
      
      
      json.append("}");
      
      return json.toString();
   }
      

   @Override
   public EffortJointHandle getJoint(String name)
   {
      return joints.get(name);
   }

   @Override
   public IMUHandle getIMU(String qualifiedName)
   {
      return imus.get(qualifiedName);
   }

   @Override
   public ForceTorqueSensorHandle getForceTorqueSensor(String qualifiedName)
   {
      return forceTorqueSensors.get(qualifiedName);
   }
 
   public SharedBuffer getSharedBuffer(String name)
   {
      return sharedBuffers.get(name);
   }
}
