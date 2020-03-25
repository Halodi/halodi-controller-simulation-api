
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
    
    
    model, plugin, media = GazeboRosPaths.get_paths()
    
    print(model)
    print(plugin)
    print(media)
    
    return LaunchDescription([
        
        DeclareLaunchArgument(
            'world',
            default_value=os.path.join(package_prefix, 'worlds' ,'eve_flat_ground.world'),
            description='Set world to laucnh',
            ),
        DeclareLaunchArgument(
            'verbose',
            default_value='true',
            description='Enable verbosity',
            ),
        DeclareLaunchArgument('pause', default_value='true',
                              description='Default to paused state.'),
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(gazebo_package_prefix, 'launch', 'gzserver.launch.py'))
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(gazebo_package_prefix, 'launch', 'gzclient.launch.py'))
        ),
    ])
