package us.ihmc.rosControl;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import us.ihmc.rosControl.util.CallbackPrintStream;
import us.ihmc.rosControl.wholeRobot.EffortJointHandle;
import us.ihmc.rosControl.wholeRobot.EffortJointHandleImpl;
import us.ihmc.rosControl.wholeRobot.ForceTorqueSensorHandle;
import us.ihmc.rosControl.wholeRobot.ForceTorqueSensorHandleImpl;
import us.ihmc.rosControl.wholeRobot.IMUHandle;
import us.ihmc.rosControl.wholeRobot.IMUHandleImpl;

public abstract class IHMCRosControlJavaBridge
{
   
   
   private ByteBuffer readBuffer;
   private ByteBuffer writeBuffer;
   
   public final native boolean addJointToBufferN(long thisPtr, String jointName);
   
   private final native static void rosError(String error);
   private final native static void rosInfo(String msg);
   
   private final native ByteBuffer createReadBuffer(long thisPtr);
   private final native ByteBuffer createWriteBuffer(long thisPtr);
   
   private boolean inInit = false;
   private long thisPtr;
   private long delegatePtr;

   
   private final ArrayList<NativeUpdateableInterface> updatables = new ArrayList<>();
   
   
   protected IHMCRosControlJavaBridge()
   {
      System.setOut(new CallbackPrintStream()
      {
         
         @Override
         public void outputLine(String message)
         {
            rosInfo(message);
         }
      });
      
      System.setErr(new CallbackPrintStream()
      {
         
         @Override
         public void outputLine(String message)
         {
            rosError(message);
         }
      });
   }
   
   private final native boolean addIMUToBufferN(long thisPtr, String imuName);
   private final native boolean addForceTorqueSensorToBufferN(long thisPtr, String forceTorqueSensorName);


   /**
    * Return a new JointStateHandle. Call from init()
    *
    * @param jointName
    * @return
    */
   protected final EffortJointHandle createEffortJointHandle(String jointName)
   {
      if(!inInit())
      {
         throw new RuntimeException("createEffortJointHandle should only be called from init()");
      }
      if(!addJointToBufferN(getDelegatePtr(), jointName))
      {
         throw new IllegalArgumentException("Cannot find joint with name " + jointName);
      }
      EffortJointHandleImpl jointHandle = new EffortJointHandleImpl(jointName);
      addUpdatable(jointHandle);
      return jointHandle;
   }


   
   /**
    * Return a new IMUHandle. Call from init()
    * 
    * @param imuName
    * @return
    */
   protected final IMUHandle createIMUHandle(String imuName)
   {
      if(!inInit())
      {
         throw new RuntimeException("createIMUHandle should only be called from init()");
      }
      if(!addIMUToBufferN(getDelegatePtr(), imuName))
      {
         throw new IllegalArgumentException("Cannot find IMU with name " + imuName);
      }
      IMUHandleImpl imuHandle = new IMUHandleImpl(imuName);
      addUpdatable(imuHandle);
      return imuHandle;
   }

   /**
    * Return a new force torque sensor handle. Call from init()
    * 
    * @param forceTorqueSensorName
    * @return
    */
   protected final ForceTorqueSensorHandle createForceTorqueSensorHandle(String forceTorqueSensorName)
   {
      if(!inInit())
      {
         throw new RuntimeException("createForceTorqueSensorHandle should only be called from init()");
      }
      if(!addForceTorqueSensorToBufferN(getDelegatePtr(), forceTorqueSensorName))
      {
         throw new IllegalArgumentException("Cannot find force torque sensor with name " + forceTorqueSensorName);
      }
      ForceTorqueSensorHandleImpl imuHandle = new ForceTorqueSensorHandleImpl(forceTorqueSensorName);
      addUpdatable(imuHandle);
      return imuHandle;
   }
   
   
   public boolean inInit()
   {
      return inInit;
   }
   
   public long getDelegatePtr()
   {
      return delegatePtr;
   }
   
   public void addUpdatable(NativeUpdateableInterface updatable)
   {
      if(!inInit)
      {
         throw new RuntimeException("Cannot call from outside init()");
      }
      
      updatables.add(updatable);
   }
   

   boolean initFromNative(long thisPtr, long delegatePtr)
   {
      try
      {
         this.thisPtr = thisPtr;
         this.delegatePtr = delegatePtr;
         inInit = true;
         init();
         inInit = false;
         
         readBuffer = createReadBuffer(thisPtr);
         writeBuffer = createWriteBuffer(thisPtr);
         
         readBuffer.order(ByteOrder.nativeOrder());
         writeBuffer.order(ByteOrder.nativeOrder());
         
         return true;
      }
      catch(Throwable e)
      {
         e.printStackTrace();
         return false;
      }
   }
   
   void updateFromNative(long time, long duration)
   {
      try
      {
         readBuffer.clear();
         for(int i = 0; i < updatables.size(); i++)
         {
            updatables.get(i).readFromBuffer(readBuffer);
         }
         
         doControl(time, duration);
         
         writeBuffer.clear();
         for(int i = 0; i < updatables.size(); i++)
         {
            updatables.get(i).writeToBuffer(writeBuffer);
         }
      }
      catch(Throwable e)
      {
         e.printStackTrace();
      }
   }
   
   
   /**
    * This function gets called when the controller gets initialized. Use this function to setup
    * the resource interfaces.
    * 
    * If an Exception gets thrown, the controller will shutdown.
    */
   protected abstract void init();
   
   /**
    * This method gets called cyclically in the update.
    * 
    * Exceptions will get caught and output to ROS_ERROR. Execution will continue.
    * 
    */
   protected abstract void doControl(long time, long duration);
   
   
}
