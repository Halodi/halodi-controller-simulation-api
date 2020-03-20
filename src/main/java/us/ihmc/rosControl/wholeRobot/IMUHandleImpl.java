package us.ihmc.rosControl.wholeRobot;

import java.nio.ByteBuffer;

import us.ihmc.rosControl.NativeUpdateableInterface;

public class IMUHandleImpl implements IMUHandle, NativeUpdateableInterface
{
   private final String name;                       ///< The name of the sensor
   
   
   // Orientation
   private double q_x, q_y, q_z, q_w;
   
   
   
   // Angular velocity
   private double theta_x, theta_y, theta_z;
   
   
   
   
   // Acceleration
   private double xdd, ydd, zdd;
   
   
   public IMUHandleImpl(String name)
   {
      this.name = name;
   }
   
   @Override
   public String getName()
   {
      return name;
   }

   @Override
   public double getQ_x()
   {
      return q_x;
   }

   @Override
   public double getQ_y()
   {
      return q_y;
   }

   @Override
   public double getQ_z()
   {
      return q_z;
   }

   @Override
   public double getQ_w()
   {
      return q_w;
   }


   @Override
   public double getTheta_x()
   {
      return theta_x;
   }

   @Override
   public double getTheta_y()
   {
      return theta_y;
   }

   @Override
   public double getTheta_z()
   {
      return theta_z;
   }

   @Override
   public double getXdd()
   {
      return xdd;
   }

   @Override
   public double getYdd()
   {
      return ydd;
   }

   @Override
   public double getZdd()
   {
      return zdd;
   }


   @Override
   public void readFromBuffer(ByteBuffer buffer)
   {
      q_x = buffer.getDouble();
      q_y = buffer.getDouble();
      q_z = buffer.getDouble();
      q_w = buffer.getDouble();
      
      theta_x = buffer.getDouble();
      theta_y = buffer.getDouble();
      theta_z = buffer.getDouble();
      
      xdd = buffer.getDouble();
      ydd = buffer.getDouble();
      zdd = buffer.getDouble();

   }

   @Override
   public void writeToBuffer(ByteBuffer buffer)
   {
      // Only has state
   }
   
   

}
