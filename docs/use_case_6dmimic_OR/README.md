# OR+Photoneo and 6DMimic Use-Case

## Overview

TODO


## Installation


### Plugins:
The following plugins should be placed at plugins folder.
- **libinterface_6dmimic.so** (tested version v25_01_2022) [Dev Package](https://gitlab.inesctec.pt/CRIIS/mimicgrasping/mimic_grasping_plugin_generator)
- **libphoxi_cam_object_localization_ros.so** (tested version v14_01_2022) [Dev Package](https://gitlab.inesctec.pt/CRIIS/mimicgrasping/mimic_grasping_ros_plugin_generator)


### External Servers

In this use-case all plugins work as clients. Therefore, external server packages need to be configured. These packages are listed bellow:


* [Mimic Grasping ROS Interface Package](https://github.com/ItzMeJP/mimic_grasping_ros_interface_package): ROS package which supports OR interface and sensing devices, e.g. Photoneo cameras. The OR and any camera ROS package should be installed in a priori following the package README. Since the package is based on ROS, the mimic grasping API and this package communication is established by ROS protocols.

* [6DMimic Grasping Server](https://gitlab.inesctec.pt/CRIIS/mimicgrasping/mimic_grasping_6dmimic_interface): Lazarus package which implements the 6DMimic Grasping server (Teaching module) over 6D Mimic companion computer. It also allows to call 6DMimic modules with different configurations focusing in Mimic Grasping. Thus, the 6D Mimic setup should be a priori installed. This package allows a UDP configuration to send data back to the mimic grasping API. It also sends a hearbit signal which can be verified by the mimic grasping API when using computers in a network.

* [6DMimic Grasping Heartbit Checker](https://gitlab.inesctec.pt/CRIIS/mimicgrasping/mimic_grasping_6dmimic_local_interface): Lazarus server application which is loaded by mimic grasping API to monitoring the connection with 6DMimic Grasping server.

## Configuration files:

Use the configuration profile ``iilab_inesctecrobotics_wifi`` as reference.


## Calibration
See [this](/docs/use_case_6dmimic_OR/calibration.md) document for a detailed system calibration description;

See [this](/docs/use_case_6dmimic_OR/error_compensation.md) document for a detailed error estimation and result compensation procedure description.


## Usage

0. Turn on the 6DMimic System, the IRB1600 Robot and the Tool Handler.
1. Connect the tool Handler USB cable to the PC that is running mimic_grasping_api.
2. Open the serial Tool Handler communication ```sudo chmod 666 @DEVICE_NAME``` and set its port name in ```/config/profile/tool_firmware_config.json ```
3. Connect both, 6DMimic PC and the mimic_grasping_api at the same network.
4. Set the proper IP address into ```/config/profile/plugin_tool_localization_6dmimic.json ``` and into 6DMimic Pc (/mimic_grasping/TODO)
5. Place the robot at pose [p2](/abb_irb_1600_programs/mimic_grasping_poses/MainModule.md).
6. Put the object on the table.
7. Set the object model into ```/config/profile/plugin_phoxi_cam_object_localization_ros ```
8. Run mimic_grasping script into 6DMimic PC (/mimic_grasping/TODO)
9. Run mimic_grasping_api (usefull to use API)
10. Wait for object detection, verified it in RVIZ that will be pop-up and start the demonstration process following the [tool firmware convention](https://gitlab.inesctec.pt/CRIIS/mimicgrasping/mimic_grasping_tool_firmware).
11. Finish it and export the dataset that wil be placed at output folder.


[Object Recognition (OR) package](https://github.com/carlosmccosta/object_recognition)

[6D Mimic](https://gitlab.inesctec.pt/CRIIS/6D_MIMIC)
