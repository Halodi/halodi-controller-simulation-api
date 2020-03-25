package com.halodi.controllerAPI.wholeRobot;

public interface IMUHandle
{

   double getZdd();

   double getYdd();

   double getXdd();

   double getTheta_z();

   double getTheta_y();

   double getTheta_x();

   double getQ_w();

   double getQ_z();

   double getQ_y();

   double getQ_x();
   
}
