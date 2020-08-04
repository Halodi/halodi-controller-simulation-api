package com.halodi.controllerAPI.util;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import org.apache.commons.lang3.SystemUtils;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import com.halodi.controllerAPI.NativeHalodiControllerJavaBridge;
import com.halodi.controllerAPI.demo.HalodiControllerDemo;

/**
 * This tool writes the current JVM configuration  to [LOCAL APPDATA]/halodi/controller-plugin-config.json
 * 
 * @author jesper
 *
 */
public class HalodiControllerPluginConfigurationInstaller
{
   
   
   private static final String company = "halodi";
   private static final String configFile = "controller-plugin-config.json";

   private static Path getPersistantDataPath() throws IOException
   {
      Path path;
      if(SystemUtils.IS_OS_WINDOWS)
      {
         String localAppData = System.getenv("LOCALAPPDATA");
         
         if(localAppData == null || localAppData.trim().isEmpty())
         {
            throw new IOException("Environment variable LOCALAPPDATA not set");
         }
         
         path = Paths.get(localAppData, company);
         
      }
      else if(SystemUtils.IS_OS_LINUX)
      {
         String xdgDataHome = System.getenv("XDG_DATA_HOME");
         
         if(xdgDataHome == null || xdgDataHome.trim().isEmpty())
         {
            path = Paths.get(SystemUtils.getUserHome().getPath(), ".local", "share", company);
         }
         else
         {
            path = Paths.get(xdgDataHome, company);
         }
      }
      else
      {
         throw new IOException("Cannot find local application data for your OS");
      }
      
      if(Files.exists(path) && !Files.isDirectory(path))
      {
         throw new IOException(path.toString() + " exists but is not a directory");
      }
      
      return Files.createDirectories(path);
   }
   
   public static void install(Class<? extends NativeHalodiControllerJavaBridge> pluginClass, String... vmArguments) throws IOException
   {
      ObjectMapper mapper = new ObjectMapper().enable(SerializationFeature.INDENT_OUTPUT);
      Path config = getPersistantDataPath().resolve(configFile);
      
      HalodiControllerPluginConfigurationList list = new HalodiControllerPluginConfigurationList();
      if(Files.exists(config))
      {
         list = mapper.readValue(config.toFile(), HalodiControllerPluginConfigurationList.class);
      }
      
      
      HalodiControllerPluginConfiguration halodiControllerPluginConfiguration = HalodiControllerPluginConfigurationCreator.create(pluginClass, vmArguments);
      list.plugins.remove(halodiControllerPluginConfiguration);
      list.plugins.add(halodiControllerPluginConfiguration);
      
      mapper.writeValue(config.toFile(), list);
      
   }
   
   

   public static void main(String[] args) throws IOException
   {
      install(HalodiControllerDemo.class);
   }
}

