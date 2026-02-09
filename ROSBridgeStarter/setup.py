from setuptools import find_packages, setup

package_name = 'ROSBridgeStarter'

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
    maintainer='mbturton',
    maintainer_email='mbturton33@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'ros_firmware_bridge = ROSBridgeStarter.ros_bridge:main',
            'ros_bridge_motor_test = ROSBridgeStarter.motor_test_bridge:main'
        ],
    },
)
