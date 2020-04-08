
import os 
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch.substitutions import ThisLaunchFileDir

from ament_index_python.packages import get_package_share_directory



from scripts import GazeboRosPaths

def generate_launch_description():
    gazebo_package_prefix = get_package_share_directory('gazebo_ros')
    package_prefix = get_package_share_directory('halodi-controller-gazebo')
    halodi_controller_prefix = get_package_share_directory('halodi-controller')
        
    model, plugin, media = GazeboRosPaths.get_paths()
    
    halodi_controller_classpath = os.path.join(halodi_controller_prefix, "*.jar")
    
    os.environ["HALODI_CONTROLLER_CLASSPATH"] = halodi_controller_classpath;
    
    return LaunchDescription([
        
        DeclareLaunchArgument(
            'world',
            default_value=os.path.join(package_prefix, 'worlds' ,'eve_flat_ground.world'),
            description='Set world to launch',
            ),
        DeclareLaunchArgument(
            'verbose',
            default_value='true',
            description='Enable verbosity',
            ),
        
        DeclareLaunchArgument('init', default_value='false',
                              description='Set "false" not to load "libgazebo_ros_init.so"'),
        DeclareLaunchArgument('factory', default_value='false',
                              description='Set "false" not to load "libgazebo_ros_factory.so"'),
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(gazebo_package_prefix, 'launch', 'gzserver.launch.py'))
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(gazebo_package_prefix, 'launch', 'gzclient.launch.py'))
        ),
    ])
