package com.halodi.controllerAPI.util;

import java.util.ArrayList;
import java.util.List;

public class HalodiControllerPluginConfiguration
{
   public String name;
   
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

   @Override
   public int hashCode()
   {
      final int prime = 31;
      int result = 1;
      result = prime * result + ((name == null) ? 0 : name.hashCode());
      return result;
   }

   @Override
   public boolean equals(Object obj)
   {
      if (this == obj)
         return true;
      if (obj == null)
         return false;
      if (getClass() != obj.getClass())
         return false;
      HalodiControllerPluginConfiguration other = (HalodiControllerPluginConfiguration) obj;
      if (name == null)
      {
         if (other.name != null)
            return false;
      }
      else if (!name.equals(other.name))
         return false;
      return true;
   }
   
   
   
   
}
