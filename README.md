# mimic_grasping_api

<a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-nd/4.0/88x31.png" />

## <a name="summary"></a>1. Summary


* [Overview](#overview)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
* [Usage](#usage)

## <a name="overview"></a>2. Overview

The mimic_grasping_api is a C++ API, plugin based, which allows the human grasping demonstration. A grasping demonstration task could be defined as two localisation problem: the human manipulation and the graspable object pose estimation. The plugin strategy is important since different techniques could be implemented as a dynamic library that is loaded in run-time into the mimic_grasping_api architecture without the need to recompile the core pipeline.

The mimic_grasping_api pipeline structured workflow is presented in figure bellow. The mimic workflow concept considers that tools are necessary to define the human grasping, such as gloves or handler mechanisms, or at least a remote control technique that allows demonstrators requests while performing the demonstration. It is expected that the communication with the tool is performed by serial communication. In addition, it is considered that the two localisation methodologies are processed in sequence after the operator record request.
<p align="center">
  <img src="./images/mimic_grasping_api.png" width="680">
</p>
<p align="center">
The mimic_grasping_api flowchart.
</p>

Namely, this workflow starts by loading API general configurations, such as tool communication parameters (see [this](/configs/iilab_inesctecrobotics_wifi/tool_firmware_config.json) example) and localisation methods definitions (see [this](/configs/iilab_inesctecrobotics_wifi/localization_config.json) example). 

When the system is started the implemented plugins are loaded into API memory with its respective parameters, the terminator is executed and the communication with demonstration tool verified. If all process is correctly executed, the demonstration process is started, i.e. the human operator positions himself in grasping configuration, execute the grasping and request the record. This procedure can be performed several times and, at end, the operator can export the grasping datatset.

## <a name="prerequisites"></a>2. Prerequisites

### Essential
* [Simple serial](https://github.com/ItzMeJP/SimpleSerialInterface)
* [Plugin system management](https://github.com/ItzMeJP/plugin_system_cpp)
* [Transform Manipulation](https://github.com/ItzMeJP/transform_manipulation)
* [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page)
* [Boost](https://www.boost.org/)
* [Jsoncpp](https://open-source-parsers.github.io/jsoncpp-docs/doxygen/index.html#_intro)

### Complementary

Since the mimic_grasping_api is based on plugin management, the plugins used could have to communicate to externals applications. Thus, verify [use-case](#use_cases) section for additional install information.

## <a name="installation"></a>3. Installation

Setup all prerequisites. (Current testes with this API was performed using gcc 10.3.0)
Then:
```
cd /mimic_grasping_server
mkdir build
cd build
cmake ..
sudo make install
```

After the installation the shared libs will be located at ```/usr/local/lib```. The headers files will be located at ```/usr/local/include```

### <a name="gui_installation"></a> GUI

A GUI was designed to easy interaction with this API, for further information and how to setup it, check [this package](https://gitlab.inesctec.pt/CRIIS/mimicgrasping/mimic_grasping_gui)


## <a name="usage"></a> 4. USAGE

## <a name="config_files"></a> Configuration Files

Three configuration file composes the mimic_grasping_api: the general, the tool communication and the localisators. Each one of these files are defined bellow:

The general configuration file is composed by (see [this](/configs/general.json) example):

* **profile:** which profile setup will be loaded by the API. Profile allows multiple pre-configured setups. All other configuration files will be placed inside the profile.

* **apply_compensation:** if true, a correction factor is performed into grasping candidate, i.e. grasping pose w.r.t object frame.

* **error_compensation_file:** the file which defines the correction factor. It is only read if "apply_compensation" flag is set. See [this](#corrector_ids) for further details.


The tool communication is composed by (see [this](/configs/iilab_inesctecrobotics_wifi/tool_firmware_config.json) example):

* **baud_rate:** serial communication baud rate;

* **port:** serial port name;

* **tool type:** gripper ID.

The localisation method definition is composed by (see [this](/configs/iilab_inesctecrobotics_wifi/localization_config.json) example):

* **plugin_name:** the plugin dynamic library implementation file name;

* **plugin_config_file:** the plugin dynamic library configuration file name. This is necessary since the parameters are custom defined according to plugin implementation;

* **executor\_cmd and terminator_cmd:** typically, but not mandatory, plugins are client implementations since the localisation server could be loaded a part. Therefore, the pipeline need to bring up these outer systems by running a user defined script (or a simple command), caller executor. At end, a shutdown scripts (or command), called terminator, is expected to turn off these systems.

* **calib_matrix_file:** the calibration matrix file name which defines the transformation between the tool and object localisation base frames, i.e. object localisation base frame w.r.t tool localisation base frame.

* **one_shoot_estimation:** this parameter is only able to object recognition. If false, every time that the user requests to capture the tool pose, the object will also be located. Otherwise, the object will be located only once before to the grasping demonstration process.

* **apply_compensation:** if true, a correction factor is performed in data acquired by the localisator (tool or object localisation), i.e. w.r.t localisator base frame.

* **error_compensation_file:** the file which defines the correction factor. It is only read if "apply_compensation" flag is set. See [this](#corrector_ids) for further details.


## <a name="grippers_id"></a> Grippers IDs

The list of supported grippers are defined below:

* **ROBOTIQ_2F_85: {Id: 0}** two adaptive fingers gripper from Robotiq with opening of 85mm. ;
* **ROBOTIQ_2F_140: {Id: 1}** two adaptive fingers gripper from Robotiq with opening of 140mm;
* **ROBOTIQ_3F: {Id: 2}** hree adaptive fingers gripper from Robotiq;
* **SUCTION: {Id: 3}** generic one cup suction gripper;
* **FESTO_2F_HGPC_16_A_30: {Id: 4}** pneumatic parallel 2F gripper from FESTO with fingers with opening of 30mm
* **SCHMALZ_FOAM_SUCTION_CUP_FMSW_N10_76x22: {Id: 5}** Small rectangular foam suction cup from Schmalz


## <a name="corrector_ids"></a> Compensation Heuristics

The compensation methods allow correct a pose value (x,y,z,roll,pitch,yaw) result according to an estimated error, i.e:
``result += estimated_error``. It is necessary to establish a .json file (see [this](/configs/iilab_inesctecrobotics_wifi/general_error_compensation.json) example) which defines each translation (displacement in meters) and rotation (roll, pitch and yaw angles in radians - ZYX order) coordinate.

Thus, the list of supported heuristics to correct this error is defined bellow:

#### 1. Constant absolute {Id: 0}: 
The estimated error is constant. Configuration description params:
* **offset:** the constant;

#### 2. Constant relative {Id: 1}:
The estimated error is relative to the measurement. Configuration description params:
* **percentage:** the relative value from 0 to 100%;

#### 3. Linear {Id: 2}:
The estimated error is defined by 

``estimated_error = a*measurement+b``

Configuration description params:
* **a:** angular coefficient;
* **b:** constant value ;

#### 4. Exponential {Id: 3}:
The estimated error is defined by 

``estimated_error = multiplier*base^(alpha*measurement)``

Configuration description params:
* **multiplier:** base multiplier coefficient;
* **base:** base coefficient to the power of an exponent;
* **alpha:** exponential multiplier value ;


### <a name="use_cases"></a> Use-Cases

#### 1. OR+Photoneo and 6DMimic
See [this](/docs/use_case_6dmimic_OR/README.md) document for a use-case overview and detailed about the installation, calibration and usage;







-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
<br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/">Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License</a>.
