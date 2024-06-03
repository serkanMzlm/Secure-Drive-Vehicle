from setuptools import find_packages, setup

package_name = 'camera_streamer'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='serkan',
    maintainer_email='serkanmazlum306@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'recorder_node = camera_streamer.recorder:main',
            'player_node = camera_streamer.player:main',
        ],
    },
)
