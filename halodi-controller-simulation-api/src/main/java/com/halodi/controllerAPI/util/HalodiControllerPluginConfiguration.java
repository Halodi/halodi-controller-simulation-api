package com.halodi.controllerAPI.util;

import java.util.ArrayList;
import java.util.List;

public class HalodiControllerPluginConfiguration
{
   public List<String> classPath = new ArrayList<>();

   public String mainClass;

   public List<String> vmArgs;
   
   public String javaHome;

   @Override
   public String toString()
   {
      StringBuilder builder = new StringBuilder();
      builder.append("HalodiControllerPluginConfiguration [");
      builder.append(System.lineSeparator());

      
      builder.append("\tclassPath=");
      builder.append(classPath);
      builder.append(System.lineSeparator());
      
      builder.append("\tmainClass=");
      builder.append(mainClass);
      builder.append(System.lineSeparator());
      
      builder.append("\tvmArgs=");
      builder.append(vmArgs);
      builder.append(System.lineSeparator());
      
      builder.append("\tjavaHome=");
      builder.append(javaHome);
      
      builder.append(System.lineSeparator());
      builder.append("]");
      return builder.toString();
   }
   
   
}
