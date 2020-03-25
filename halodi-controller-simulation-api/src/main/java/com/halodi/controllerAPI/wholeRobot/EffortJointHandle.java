package com.halodi.controllerAPI.wholeRobot;

public interface EffortJointHandle
{   
   public double getEffort();
   public double getPosition();
   public double getVelocity();
   
   public void setDesiredEffort(double desiredEffort);
   public void setDampingScale(double dampingScale);
}
