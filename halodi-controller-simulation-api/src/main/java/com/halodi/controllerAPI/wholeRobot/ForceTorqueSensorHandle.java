package com.halodi.controllerAPI.wholeRobot;

public interface ForceTorqueSensorHandle
{

   double getTz();

   double getTy();

   double getTx();

   double getFz();

   double getFy();

   double getFx();

   String getName();

   String getParentLink();

}
