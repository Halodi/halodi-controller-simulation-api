package us.ihmc.rosControl;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public abstract class IHMCRosControlJavaBridge
{
   private ByteBuffer readBuffer;
   private ByteBuffer writeBuffer;
   
   private final native boolean addJointToBufferN(long thisPtr, String jointName);
   
   private final native ByteBuffer createReadBuffer(long thisPtr);
   private final native ByteBuffer createWriteBuffer(long thisPtr);
   
   private boolean inInit = false;
   private long thisPtr;

   
   private final ArrayList<NativeUpdateableInterface> updatables = new ArrayList<>();
   
   
   protected boolean inInit()
   {
      return inInit;
   }
   
   protected long getThisPtr()
   {
      return thisPtr;
   }
   
   protected void addUpdatable(NativeUpdateableInterface updatable)
   {
      if(!inInit)
      {
         throw new RuntimeException("Cannot call from outside init()");
      }
      
      updatables.add(updatable);
   }
   
   /**
    * Return a new JointHandle. Call from init()
    * 
    * @param jointName
    * @return
    */
   protected final JointHandle createJointHandle(String jointName)
   {
      if(!inInit)
      {
         throw new RuntimeException("createJointHandle should only be called from init()");
      }
      if(!addJointToBufferN(thisPtr, jointName))
      {
         throw new IllegalArgumentException("Cannot find joint with name " + jointName);
      }
      JointHandleImpl jointHandle = new JointHandleImpl(jointName);
      updatables.add(jointHandle);
      return jointHandle;
   }
   
   
   void initFromNative(long thisPtr)
   {
      try
      {
         this.thisPtr = thisPtr;
         inInit = true;
         init();
         inInit = false;
         
         readBuffer = createReadBuffer(thisPtr);
         writeBuffer = createWriteBuffer(thisPtr);
         
         readBuffer.order(ByteOrder.nativeOrder());
         writeBuffer.order(ByteOrder.nativeOrder());
      }
      catch(Throwable e)
      {
         e.printStackTrace();
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
    * the effort interfaces.
    * 
    * @see createEffortJointInterface
    */
   protected abstract void init();
   
   /**
    * This method gets called cyclically in the update.
    */
   protected abstract void doControl(long time, long duration);
}