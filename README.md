
	
# Development Environment

- OS: Windows 10 (64 bit)
- IDE: Visual Studio Enterprise 2015 Update 2 ( 14.0.25123.00 )
- C++ Standard: Microsoft Visual C++ 2015 ( 00322-90150-00888-AA891 )
- Platform Toolset: Visual Studio 2015 (v140)


# File List

	README.md
    
	CHANGE.md

	version.h & sim.rc | 版本信息

	Global.h & .cpp | 库文件、命名空间、宏定义、全局类/全局函数定义
    
	main.cpp | 程序入口
    
	---
    
	Entity.h & .cpp | 一些与实体类有关的虚类/简单类定义(CEntity, CDecorator, CGeoEntity, CUnique, CSpatial, CBasicEntity，CCoordinate)
    
	Data.h & .cpp | 一些与数据有关的类定义

	|-- CGeneralData | 通用数据基类

		|-- CPacket | 包数据虚类

			|-- CData | 控制包

			|-- CCtrl | 控制包

		|-- CFrame MAC层帧数据

	GeneralNode.h & .cpp | 通用节点基类 CGeneralNode
	
	|-- Sink.h & .cpp | Sink节点类 CSink

	|-- Node.h & .cpp | 传感器节点类 CNode 

	|-- MANode.h & .cpp | MA节点类 CMANode

	CGeoEntity | 与地理位置相关的类的空基类

	|-- CTrace.h & .cpp | 传感器节点轨迹类 CTrace
     
	|-- Position.h & .cpp | 节点位置点类 CPosition
     
	|-- Hotspot.h & .cpp | 热点类 CHotspot
	 
	|-- Route.h & .cpp | MA节点路径类 CRoute
    
	---

	Process.h | 一些与过程类有关的虚类定义(CProcess, CHelper, CAlgorithm, CProtocol)

	|-- CAlgorithm | 算法类

		|-- HotspotSelect.h & .cpp | 热点选取算法(HAR)
    
		|-- PostSelect.h & .cpp | 后续选取算法(HAR)
    
		|-- NodeRepair.h & .cpp | 疏漏节点修复算法(iHAR)

		|-- CProtocol | 特殊的协议类

			|-- RoutingProtocol.h & .cpp | 所有路由协议的通用基类

				|-- Epidemic.h & .cpp | Epidemic路由
	    
				|-- Prophet.h & .cpp | Prophet路由
	
				|-- HAR.h & .cpp | xHAR(HAR/iHAR)路由

				|-- PFerry.h & .cpp | LTPR(PFerry)路由

			|-- MacProtocol.h & .cpp | 所有MAC协议的通用基类
	
				|-- SMac.h & .cpp | SMac
	
				|-- HDC.h & .cpp | HDC

	|-- Helper.h & .cpp | 辅助工具类
    
		|-- FileHelper.h & .cpp | 文件操作相关
    
		|-- SortHelper.h & .cpp | 排序相关
    
		|-- ParseHelper.h & .cpp | 字符串解析相关
    
		|-- RunHelper.h & .cpp | 运行相关

		|-- PrintHelper.h & .cpp | 控制台/日志输出相关
    
	---

	Configuration.h & .cpp | 用于实现配置参数的定义、初始化、访问、更新的类
        


# Simulation Configurations

- All the configurations and their initial values are defined in `CConfiguration::InitConfiguration()` in source file `src/Configuration.cpp`.
- Default values are defined in disk file `bin/default.config`, which get loaded at the start of each execution.

- Order of overwriting:

  1. initial value (defined in source file)
  2. default value (`bin/default.config`)
  3. command line arguments

- Configurations are seperated in different groups to avoid name collision.


### Group "simulation"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
mac_protocol | EnumMacProtocolScheme | _smac | Choose MAC protocol. 
routing_protocol | EnumRoutingProtocolScheme | _xhar | Choose routing protocol.
hotspot_select | EnumHotspotSelectScheme | _original | Choose hotspot select scheme for hotspot based protocols (HAR/HDC).

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
runtime | int | 15000 | End time of simulation (in terms of seconds).
datatime | int | 15000 | End time of data generating for sensor nodes.
slot | int | 1 | Time precision of simulation. ( >= 1s )

(See definitions of `Enum***Scheme` in `Configuration.h`.)

	
### Group "log"

Logging related configurations: log frequency, log filenames, ...

#### Basic Logging Configurations

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
slot_log | int | 0 (Undefined) | Print cycle of normal logs.
slot_brief | int | 100 | Print cycle of brief logs.
detail | int | 1 | Level of details for log printing (to console & files): 0~4 from least to most detailed.
tag | string | "" | An string tag attached to log folder for this simulation for convenience. e.g. `log/[some-tag]2018-03-22-20-37-13/`

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
dir_root | string | "../" | Root directory (for project).
dir_project | string | "../.project/" | Location of project files.
dir_run | string | "" | Working directory (location of `sim.exe`).
dir_resource | string | "../res/" | Location of trace files & prediction files.
dir_log | string | "../log/" | Location of all the log folders.
timestamp | string | "" | Start time of this simulation, used as identifier during logging.
path_timestamp | string | "" | Related path of log folder for this simulation. e.g. "[some-tag]2018-03-22-20-37-13/"


#### Log Files {#log-file-description}

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
info_log | string | "" | Description of this simulation (e.g. "@2018-03-22-20-37-13"), which would be printed to the start of all the log files.
file_default_config | string | "default.config" |
file_help | string | "../README.md" | File to display for `sim.exe -help`.
file_version | string | "sim.version" | Version of source files used for building of `sim.exe`.

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
file_console | string | "console.log" | Log file for console output.
file_error | string | "error.log" | Log file for errors during execution.
file_config | string | "config.log" | Configurations used for this simulation.

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
file_node | string | "node.log" | Count of sensor nodes.
info_node | string | "#Time	#NodeCount " | Description of columns in the related log file.
file_death | string | "death.log" | Death of sensor nodes.
info_death | string | "#Time	#DeathCount	#Delivery #DeliveryRatio " | 
file_encounter | string | "encounter.log" | Encounter statistics between MA & sensor nodes. (only available in hotspot based protocols)
info_encounter | string | "#Time	(#EncounterAtHotspot%	#EncounterAtHotspot)	#Encounter	 " | 
file_transmit | string | "transmit.log" | Transmission statistics between nodes.
info_transmit | string | "#Time	(#TransmitSuccessful%	#TransmitSuccessful	#Transmit " | 
file_activation | string | "activation.log" | Activation statistics for sensor nodes.
info_activation | string | "#Time	#PercentAwake... " | 
file_energy_consumption | string | "energy-consumption.log" | Energy consumption statistics for sensor nodes.
info_energy_consumption | string | "#Time	#AvgEC	(#SumEC	#NodeCount	#CurrentEnergy...) " | 
file_sink | string | "sink.log" | Encounter statistics between sink & sensor nodes.
info_sink | string | "#Time	#EncounterAtSink " | 

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
file_delivery_ratio_brief | string | "delivery-ratio-brief.log" | Brief log for delivery ratio statistics.
info_delivery_ratio_brief | string | "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% " | 
file_delivery_ratio_detail | string | "delivery-ratio-detail.log" | Normal (detailed) log for delivery ratio statistics.
info_delivery_ratio_detail | string | "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% " | 
file_delay | string | "delay.log" | Delivery delay statistics.
info_delay | string | "#Time	#AvgDelay " | 
file_hop | string | "hop.log" | Delivery HOP statistics.
info_hop | string | "#Time	#AvgHOP " | 
file_buffer | string | "buffer.log" | Real-time buffer states of sensor nodes.
info_buffer | string | "#Time	#BufferStateOfEachNode " | 
file_buffer_statistics | string | "buffer-statistics.log" | Buffer state statistics for sensor nodes.
info_buffer_statistics | string | "#Time	#AvgBufferStateInHistoryOfEachNode " | 

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
file_hotspot | string | "hotspot.log" | Hotspot count.
info_hotspot | string | "#Time	#HotspotCount " | 
file_hotspot_details | string | "hotspot-details.log" | Detail information about selected hotspots.
info_hotspot_details | string | "#Time #ID	#X	#Y " | 
file_hotspot_similarity | string | "hotspot-similarity.log" | Similarity statistics with last selection.
info_hotspot_similarity | string | "#Time	#Overlap/Old	#Overlap/New	#OverlapArea	#OldArea	#NewArea " | 
file_visit | string | "visit.log" | Sensor node visits at hotspot areas.
info_visit | string | "#Time	#VisitAtHotspotPercent	#VisitAtHotspot	#VisitSum " | 
file_hotspot_statistics | string | "hotspot-statistics.log" | Delivery statistics for each hotspot.
info_hotspot_statistics | string | "#Cycle	#ID	#Location	#nPosition, nNode	#Ratio	#Tw	#DeliveryCount " | 
file_merge | string | "merge.log" | Hotspot merge statistics.
info_merge | string | "#Time	#MergeHotspotCount	#MergeHotspotPercent	#OldHotspotCount	#OldHotspotPercent	#NewHotspotCount	#NewHotspotPercent " | 
file_merge_details | string | "merge-details.log" | Hotspot merge details.
info_merge_details | string | "#Time	#HotspotType/#MergeAge ... " | 
file_ma | string | "ma.log" | MA & waypoint count.
info_ma | string | "#Time	#MACount	#AvgMAWayPointCount " | 
file_ma_route | string | "ma-route.log" | MA routes.
info_ma_route | string | "#Time	#WayPoints ... " | 
file_buffer_ma | string | "buffer-ma.log" | Real-time buffer states of MAs.
info_buffer_ma | string | "#Time	#BufferStateOfEachMA " | 
file_ed | string | "ed.log" | Estimated delay in HAR.
info_ed | string | "#Time	#EstimatedDelay " | 
file_task | string | "task.log" | Statistics of data collection tasks in LTPR(PFerry).
info_task | string | "#Time	#PercentTaskMet	#CountTaskMet	#CountTask" | 
file_task_node | string | "task_node.log" | Statistics of tasks assigned for each sensor node in LTPR(PFerry).
info_task_node | string | "#Time" | 


### Group "data"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
size_data | int | 200 | Size of each data packet (in terms of bytes).
size_ctrl | int | 10 | Size of each control packet.
size_header_mac | int | 8 | Size of MAC layer header in each frame.
max_hop | int | INVALID | HOP limit in some protocols.


### Group "mac"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
cycle | int | 30 | Cycle duration of sensor nodes.
duty_rate | double | 1.0 | Duty (activation) rate of sensor nodes in each cycle.
sync_cycle | bool | true | Whether to let all the sensor nodes start with synchronized cycling.

	
### Group "hdc"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
hotspot_duty_rate | double | 1.0 | Duty rate of sensor nodes at hotspot areas in HDC.


### Group "trans"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
speed | int | 2500 | Transmission speed of nodes (Byte/second).
range | int | 100 | Transmissiong range of nodes.
probability | double | 1.0 | Success probability of transmission.
consumption_byte_send | double | 0.008 | Energy consumption for each byte sent.
consumption_byte_receive | double | 0.004 | Energy consumption for each byte received.
consumption_wake | double | 13.5 |  Energy consumption for each second awake.
consumption_sleep | double | 0.015 | Energy consumption for each second asleep.

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
size_window | int | 10 | Transmission window for nodes (in terms of frames).
constant_trans_delay | double | 0 | Use constant transmission delay (in terms of seconds). 0 for unused.


### Group "sink"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
x | double | 0 | X coordinate of sink.
y | double | 0 | Y coordinate of sink.
id | int | 0 | ID reserved for sink.
buffer | int | INFINITE_INT | Buffer capacity of sink.


### Group "node"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
default_data_rate | double | 6.8 | Default date generating rate (Byte/second) of sensor nodes.
buffer | int | 0 (Undefined) | Buffer capacity of sensor nodes.
energy | int | 0 (Undefined) | Energy capacity of sensor nodes.
lifetime_communication_history | int | INVALID | Lifetime of communication history records stored to unnecessary RTS.
scheme_queue | EnumQueueScheme | _fifo | Queuing scheme of sensor node buffer.


### Group "ma"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
speed | int | 30 | Moving speed of MAs.
buffer | int | 100 | Buffer capacity of MAs.
base_id | int | 100 | Base of ID generating for MAs. e.g. 101, 102 ...
init_num | int | INVALID (Undefined) | Initial number of MAs.
max_num | int | INVALID (Undefined) | Maximum number of MAs.


### Group "trace"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
continuous | bool | true | Whether to simulate sensor node trace as continuous (polyline) between two trace samples.
extension_trace_file | string | ".trace" | Extension of trace files.
path | string | "../res/NCSU" | Location of trace files.
interval | int | 30 | Sampling interval in trace files.


### Group "hs"

(hotspot selection related)

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
slot_position_update | int | 100 | Cycle of sensor node position collection.
slot_hotspot_update | int | 900 | Update cycle of hotspot selection.
starttime_hospot_select | int | hs.slot_hotspot_update | Start time of hotspot selection.
test_hotspot_similarity | bool | true | Whether to calculate hotspot similarity.
alpha | double | 0.03 | Threshold alpha in Post Select algorithm.


### Group "ihs"

(iHAR related)

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
lambda | double | 0 | Poor node threshold lambda in Node Repair algorithm.
lifetime_position | int | 3600 | Lifetime of sensor node position record in iHAR.


### Group "mhs"

(hotspot merge related)

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
ratio_merge_hotspot | double | 1.0 |
ratio_new_hotspot | double | 1.0 |
ratio_old_hotspot | double | 1.0 |
test_balanced_ratio | bool | false |


### Group "har"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
beta | double | 0.0025 | True hotspot threshold in HAR.
heat_1 | double | 1 | Hotspot heat ratio a_1.
heat_2 | double | 30 | Hotspot heat ratio a_2.
min_waiting_time | int | 0 | Minimum waiting time of MA at each waypoint(hotspot).


### Group "prophet"

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
init_pred | double | 0.75 | Initial delivery probability.
decay_pred | double | 0.98 | Decay rate of delivery probability.
trans_pred | double | 0.25 | Transitivity rate of delivery probability.
trans_strict_by_pred | bool | Refuse to forward to nodes with equal delivery probabilities with itself.


### Group "pferry"

(LTPR related)

Keyword | Type | Initial Value | Description
------- | ---- | ------------- | -----------
path_predict | string | "" (Undefined) | Location of trace prediction files.
keyword_predict | string | "" (Undefined) | Choose ".test"/".train"/".full" files as prediction pool.
extension_pan_file | string | ".pan" | Extension of panning rule files (for prediction results).
prestage_protocol | EnumRoutingProtocolScheme | _xhar | Choose setup routing protocol (during model training phase).
starttime | int | INVALID (Undefined) | Start time of online learning phase (actual deployment of LTPR).
waiting_time | int | INVALID (Undefined) | Maximum waiting time of MA at task position.
return_once_met | bool | false | Return at once after meeting with the targeted sensor node (or wait until pferry.waiting_time).


# Usage

- Configurations must be accessed with both group name and keyword: e.g. "log.detail"
- Configuraiton updates are seperated by `\n` in `bin/default.config`:

  > simulation.routing_protocol 2
  > simulation.datatime 30000
  > simulation.runtime 35000

And by `\s` or `\t` in command line:

  > sim.exe simulation.routing_protocol 2 simulation.datatime 30000 simulation.runtime 35000

- Enum type configuration should be treated as int type: e.g. "simulation.routing_protocol 2" (_prophet = 2)
- Bool type configuration should be set with "true" or "false": e.g. "trace.continuous true"
- If a configuration is involved during the simulation while undefined in the source file, it must be set in `bin/default.config`
  or in command line, otherwise it might lead to unexpected errors.


# Simulation Results (Log Files)

- All the log file names are defined as configurations in group "log".
- The descriptions for all the log files are summarized in [Configurations -> Group "log"](#log-file-description)

- The actual printing actions are defined in `PrintInfo()` & `PrintFinal()` functions in related protocols.
- Some log files are only available in certain protocols. e.g. "hotspot.log" in xHAR
- To locate & modify the implementation of certain logging:
  1. Find the configuration keyword by log filename:
     e.g. Search for "delay.log" in `Configuration.cpp` and find keyword "file_delay".
  2. Find all the code involving access of this configuration:
     e.g. Search for "file_delay" in all the source files and find
     > src\RoutingProtocol.cpp(63):	ofstream delay(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delay"), ios::app);
  3. Modify the found code block as you wish.

