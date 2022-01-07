
import os 
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.actions import SetEnvironmentVariable
from launch.conditions import IfCondition, UnlessCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch.substitutions import ThisLaunchFileDir

from ament_index_python.packages import get_package_share_directory



from scripts import GazeboRosPaths

def generate_launch_description():
    gazebo_package_prefix = get_package_share_directory('gazebo_ros')
    package_prefix = get_package_share_directory('halodi-controller-gazebo')
        
    model, plugin, media = GazeboRosPaths.get_paths()

    headless = LaunchConfiguration("headless")

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

        # Note that the Halodi Controller publishes /clock and therefore conflicts with gazebo_ros_init
        # Also, resetting the world state is not currently supported by the controller.
        # Therefore, we disable the gazebo_ros_init plugin
        DeclareLaunchArgument('init', default_value='false',
                              description='Set "false" not to load "libgazebo_ros_init.so"'),
        
        
        DeclareLaunchArgument('factory', default_value='true',
                              description='Set "false" not to load "libgazebo_ros_factory.so"'),


        DeclareLaunchArgument('trajectory-api', default_value='true',
                                  description='Set "false" to disable trajectory api and use realtime api'),
 
        SetEnvironmentVariable('HALODI_TRAJECTORY_API', LaunchConfiguration('trajectory-api')),
        

        DeclareLaunchArgument('variable-server', default_value='false',
                                 description='Set "true" to enable the variable server and use SCSVisualizer to tune variables'),

        SetEnvironmentVariable('SCS_VARIABLE_SERVER', LaunchConfiguration('variable-server')),


        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(gazebo_package_prefix, 'launch', 'gzserver.launch.py'))
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(gazebo_package_prefix, 'launch', 'gzclient.launch.py')), condition=UnlessCondition(headless)
        ),
    ])