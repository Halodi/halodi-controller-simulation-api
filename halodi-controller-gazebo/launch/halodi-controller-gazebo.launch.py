
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch.substitutions import ThisLaunchFileDir

from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    gazebo_package_prefix = get_package_share_directory('gazebo_ros')
    package_prefix = get_package_share_directory('halodi-controller-gazebo')
    
    return LaunchDescription([
        
        DeclareLaunchArgument(
            'world',
            default_value=[package_prefix, '/worlds/eve_flat_ground.world'],
            description='Set world to laucnh',
            ),
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([gazebo_package_prefix, '/launch/gzserver.launch.py'])
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([gazebo_package_prefix, '/launch/gzclient.launch.py'])
        ),
    ])
