
>***Created by Eco Sun on 2015-11-03***



### debug.txt

    #DataTime	#RunTime	#TransProb	#Spoken	#TTL	#Period	#DefaultDC	
	(#HotspotDC	#Alpha	#Beta	#Memory)	#Delivery	#Delay	#Energy	#Log


### Output

	...

### Usage

`HDC -help` / `help.txt`


	                                 !!!!!! ALL CASE SENSITIVE !!!!!!

	<node>      -sink           [][];  -range           [];   -prob-trans  [];   -energy      [];   
				-time-data        [];  -time-run        [];
	<data>      -buffer           [];  -data-rate       [];   -data-size   [];
	<mac>       -hdc;                  -cycle           [];   -dc-default  [];   -dc-hotspot  [];
	<route>     -epidemic;             -prophet;              -har;              -hop         [];   
				-ttl              [];
	<prophet>   -spoken           [];  -queue           [];
	<hs>        -ihar;                 -mhar;                 -alpha       [];   -beta        [];   
				-heat           [][];
	<ihar>      -lambda           [];  -lifetime        [];
	<mhar>      -merge            [];  -old             [];
	<test>      -dynamic-node-number;  -hotspot-similarity;   -balanced-ratio;



### Configuration

    Scheme                        smac, prophet
	Data Time	                  15000
	Run Time	                  15000
	Prob Trans	                  1
    Hotspot Similarity            false
    Dynamic Node Number           false
	    
	<MAC>
	
	Slot Total	                  300
	Default DC	                  0
	Hotspot DC	                  0.4
	
	<Routing>
	
	TTL	                          0
	Data Rate	                  1 / 150
	Data Size	                  400
	Buffer Capacity	              200
	Node Energy	                  0

	<HAR>

    Alpha                         0.03
    Beta                          0.0025
    Heat                          1, 30
    Min Wait                      0
    
    <IHAR>

    Lambda                        0
    Lifetime                      3600
    
    <mHAR>

    Merge                         1.0
    Old                           1.0


### Dependencies

- **TODO：**TinyXML


### Environment

- Visual Studio Ultimate 2012 Update 5 ( 11.0.61219.00 )
- Resharper 10.0.1


### Files

    Entity.h & .cpp
    
    BasicEntity.h & .cpp
    
    GeoEntity.h & .cpp
    
    GeneralNode.h & .cpp
    
    Helper.h & .cpp
    
    Algorithm.h & .cpp
    
    Protocol.h & .cpp
    
    RoutingProtocol.h & .cpp
    
    ---
    
    README.txt
    
    GlobalParameters.h
    
    main.cpp
    
    ---
    
    Data.h & .cpp
    
    Base.h & .cpp
    
    Position.h & .cpp
    
    Hotspot.h & .cpp
    
    Route.h & .cpp
    
    Sink.h & .cpp
    
    Node.h & .cpp
    
    MANode.h & .cpp
    
    FileHelper.h & .cpp
    
    Preprocesor.h & .cpp
    
    ---
    
    HotspotSelect.h & .cpp
    
    PostSelect.h & .cpp
    
    HAR.h & .cpp
    
    ---
    
    NodeRepair.h & .cpp


### Interface

	...



---


# Update LOG    * 2015-11-13 -- *

###### Previous Modifications  ·  *< 2.0.0 >*

- 修正旧版本的热点选取、后续选取中的错误（int 除法等）；

- 所有调试用参数均改为命令行传入，具体参数格式可`-help`查看（但是没有实现足够的合法性检查，因此需要在输入时注意）；

- 归并 HAR (mHAR) 的引入，参数包含`-merge`, `-old`, `-heat[-exp/-ln]`，均由命令行传入；

- 为mHAR中的热度函数添加了指数增长、对数增长的选项（测试结果显示没有影响，原因尚未进行分析，之后的测试都应暂时使用 flat 选项）；

- 加入相邻两轮热点选取结果的相似度比较功能，从命令行传入`-hotspot-similarity`即可；

- 在所有的输出文件前打印时间戳和具体的 scheme 用于识别每一次运行，打印说明信息用于识别每一列信息，并统一内容和文件名的格式；

- 改正数据投递率计算的错误（当前值 instead of 增量）；

- 将调试用的重要参数格式化保存在`debug.txt`中，调试时直接复制到 xls 文档即可；

- 将数据投递分类为 (a) MA在热点上得到投递和 (b) MA在路径上得到投递，并统计两种投递计数的百分比；

- 并对每一个热点统计其投递计数，按照热点选取时槽输出，并伴随着`atHotspot`赋`nullptr`时机的优化；

- mHAR中由于所有选取 ratio 指数增长，在贪婪选取中可能出现 position 集合还没有得到全覆盖，`hotspotsAboveAverage`为空的情况，此时直接选中`unselectedHotspots`中cover数最大的元素即可；


###### 2015-11-13  ·  *< 2.0.1 >*

- ADD：加入最小等待时间(`MIN_WAITING_TIME`)，默认为 0 即不使用该修改，可以从命令行`-min-wait`赋值；

- ADD：`delivery-hotspot.txt`中的信息改为降序排序之后再输出，便于分析热点投递计数的集中程度，减少数据后期处理的工作量；


###### 2015-11-16  ·  *< 2.0.2 >*

- ADD：规范化命令行参数输入的关键字，将动态节点数的选项也加入命令行参数；


###### 2015-11-20  ·  *< 2.0.3 >*

- ADD：将node的ID改为自动生成，并随之修改节点数动态变化时的 node 是否存在的判定，添加函数 `HAR::ifNodeExists(int id)` 代替 `id >= m_nodes.size()` 的判定方法（暂未测试）；


###### 2015-11-24  ·  *< 2.1.0 >*

- FIX：<动态节点个数测试> 在下一次热点选取之前`HAR::ChangeNodeNumber()`删除被抛弃节点的位置点信息（注意：节点个数变化周期应该为热点选取周期的倍数）；
- RFCT：将所有的`g_`系列全局变量都改为相关类内的静态变量（GA等未采用的方法相关的变量未改动）；
- ADD：增加`CHotspot::ratio`及相关方法和改动，用于测试新的 ratio 计算方法，将在贪婪选取和后续选取过程中用到，可使用命令行参数`-balanced-ratio`选项开启；
- ADD：增加`poor-hotspot.txt`用于测试投递计数为 0 的热点信息；
- **TODO：**ADD：应该将动态节点个数测试时的增删节点过程改成随机选取节点；
- **TODO：**MOD：针对热点的排序应该改成从大到小的；


###### 2015-11-25  ·  *< 2.1.1 >*

- RFCT：对`mergesort`对应的 comp 类的名字进行规范化；
- ADD：按照投递计数降序输出热点cover的position数、node数和ratio到`hotspot-rank.txt`，用于统计和分析热点质量；
- ADD：按照任期统计等待时间的累积值，同样输出到`hotspot-rank.txt`文件；


###### 2015-11-27  ·  *< 2.1.2 >*

- TRY：为每个 position 定于初始值为 1 的权值，增加`CSortHelper::DecayPositionsWithoutDeliveryCount`，按照投递计数为 0 的热点对其 position 执行衰减，可使用命令行参数`-learn`选项开启，`-decay`设置指数型衰减系数；
- NOTE：由于衰减后的 position 整体权值水平持续降低，后续选取过程中的固定alpha值将导致选取出的热点总数持续上升，在测试阶段暂时使用`-max-hotspot`来暂时稳定热点总数；
- TRY：增加`MIN_POSITION_WEIGHT`，权值衰减之后如果低于此值将被直接删除，使用命令行参数`-min-weight`赋值（默认值为 1 即不删除）（未测试）；
- FIX：修复`CHotspot::calculateRatio`中的 int 除法问题；
- FIX：修复`CHotspot::waitingTimes`忘记统计的问题；


###### 2015-12-04  ·  *< 2.2.0 >*  ·  *类重构*

- RFCT：对整体的类结构做出了调整，类的继承结构规范化；



> ***以上为 HDC 分支的初始状态***

> ***创建自 master 分支 2015-12-04 的 `commit { 0dd0e561bf88035d1cfc557e2083dfa602ab9d43 }`***

> ***以下为 HDC 分支下的更改***



###### 2015-12-04  ·  *< 2.2.1 >*

- MOD：更改工程名及工作路径名为`HDC`；
- OPT：整理删除未使用的函数定义，包括 GA 算法相关的全局变量和处理函数；


###### 2015-12-09

- MOD：修改`CData`和`CNode`类以适应 Epidemic 路由，其中 HOP 或 TTL 的选项通过赋给相应的最大值标明，在`CData`类的各种操作中可以自动判断；


###### 2015-12-10

- MOD：继续修改`CData`和`CNode`类，`CNode`类中原来是针对全部 node 统一统计能耗的，现在改为对于每个节点单独统计能耗，全局的 node 能耗改为调用时统计；
- ADD：在原来的能耗统计相关宏中添加`DATA_SIZE`，`CTRL_SIZE`，用于指示单个数据包和控制包的大小；
- ADD：增加`CHDC`类、`Epidemic`类和`CMacProtocol`基类，原`HAR`类中有关热点选择的函数暂时放入`CHDC`类，有关路由的类暂时放入`Epidemic`类，其成员属性和函数仍待修改；


###### 2015-12-11  ·  *< 2.3.0 >*  ·  *Epidemic + HDC*

- ADD：用`CNode::state`标识和计算节点的工作状态，取值范围在`[ - SLOT_TOTAL, + SLOT_LISTEN )`之间，值大于等于 0 即代表 Listen 状态；
- ADD：按照通信成功率，多次往返通信时一旦有一次通信失败，之后的通信都不再成立；
- ADD：节点的通信成功率和能量损耗，都已内置在`CNode`类内所有的数据收发操作中；
- ADD：传感器节点的能量损耗来源包括：监听能耗、休眠能耗、数据发送能耗、数据接收能耗；
- ADD：添加节点的总能量属性，以及相应的节点失效、网络运行终止的判定，默认为0即无限能量，可以通过命令行参数`-node-energy`赋值；
- ADD：对于 HDC 中的热点选取操作：暂时只提供 HAR 和 IHAR 可选，参数可通过命令行参数修改；
- NOTE：基本修改完毕，各类参数待测试：Epidemic 路由的相关参数、能耗参数、数据生成速率及大小参数、时隙及占空比参数；
- NOTE：MAC 协议的无关内容：网络初始假定所有节点时隙同步、忽略 Listen 时隙内的 Sync 时隙即全部视作 Data 时隙、忽略传输延迟（之后也可以加入计算）；


###### 2015-12-23  ·  *< 2.3.1 >*

- MOD：将 HDC 和普通 DC 的对比改成：普通 DC 始终使用默认 DC 值（`DEFAULT_DUTY_CYCYLE`）；HDC 在非热点处使用更低（甚至为 0 的）默认 DC 值、热点处使用与普通 DC 中的默认 DC 值相等或更高的热点 DC 值（`HOTSPOT_DUTY_CYCYLE`）；


###### 2015-12-28  ·  *< 2.3.2 >*

- MNT：将`README.txt`改为 MarkDown 文档`README.md`，并调整结构和格式；


###### 2016-01-22

- OPT：将 sink 节点 ID 改为 0（从而在 map 中排在最前方便查找），`CData`、`CPosition`、`CNode`、`CHotspot`的 ID 都改为从 1 开始，实时值仍然等于当前实例总数；
- ADD：添加`Prophet`类，主要包括`CNode::DeliveryPreds`及相关操作函数，Prophet 路由中使用的数据索引仍然使用 Epidemic 路由中的`CNode::SummaryVector`来实现，具体函数待测试；


###### 2016-02-09  ·  *< 2.4.0 >*  ·  *Prophet + HDC*

- FIX：完成`Prophet`类相关的调试；
- OPT：轨迹文件 .newlocation 的序号改为从 1 开始，序号对应`CNode::ID`，增删节点时仍保持对应关系；
- ADD：在解决方案中新增 file-formatter 项目，用于自动修改轨迹文件序号；


###### 2016-02-10  ·  *< 2.4.1 >*

- OPT：`debug.txt`中自动判断并输出文件头；
- OPT：更新`cout`输出格式，将数据传输和热点选取过程中的输出信息改为改写当前行（即动态显示）的方式（仅更改了 HDC 中调用的部分，`Epidemic`和`HAR`等类中的相关输出暂未更新）；
- OPT：程序运行出错和结束之后蜂鸣提醒（`BEEP 0x07 '\a'`）；


###### 2016-02-11  ·  *< 2.4.2 >*

- RFCT：将`Epidemic`类和`Prophet`类中的共有函数整理到父类`CRoutingProtocol`中（包括时槽判断和统一的输出格式），并整理和规范这两个子类中的剩余函数；
- RFCT：将`CSortHelper`类中的部分辅助函数整理到相关类中；


###### 2016-02-14  ·  *< 2.5.0 >*  ·  *ReSharper*

- RFCT：引入 ReSharper 插件，按照 ReSharper 的建议优化代码细节（getter 函数赋 const，前缀操作符，auto 类型等）；
- RFCT：针对 Epidemic、Prophet、HAR 三种路由类及其父类的函数做出调整和统一，以及 HAR 和 HDC 中的热点选取函数的调整和统一；
- RFCT：整理所有的继承结构及构造函数（访问控制、参数统一、子类函数合并、父类虚析构函数等）；
- OPT：为所有的输出文件保存文件头字符串（`string INFO_DEBUG`等）；
- RFCT：将一些选项保存到全局定义的配置类型中（`SEND::COPY/DUMP`、`RECEIVE::LOOSE/SELFISH`、`QUEUE::FIFO/LIFO`），并作为函数参数标识具体操作；
- RFCT：将之前使用全局变量（`DO_IHAR`等）标识的 scheme 选项保存到新定义的枚举类型（`_MacProtocol`，`_RoutingProtocol`，`_HotspotSelect`）的全局变量（`MAC_PROTOCOL`，`ROUTING_PROTOCOL`，`HOTSPOT_SELECT`），仍然通过命令行参数赋值；


###### 2016-02-15  ·  *< 2.5.1 > < 2.5.2 >*


###### 2016-02-16  ·  *< 2.5.3 >*


###### 2016-02-18  ·  *< 2.5.4 >*


###### 2016-02-20  ·  *< 2.5.5 >*

- RFCT：原宏定义`DATA_GENERATE_RATE`、`BYTE_PER_CTRL`和`BYTE_PER_DATA`移入`CNode`中的静态成员变量`CNode::DEFAULT_DATA_RATE`、`CNode::DATA_SIZE`和`CNode::CTRL_SIZE`；
- NOTE：`CNode::DATA_SIZE`和`CNode::CTRL_SIZE`的比例太小时，HDC 的平均能耗将比普通 DC 更大；比例越大，改善效果越明显；
- NOTE：`CNode::BUFFER_CAPACITY`和`CNode::DEFAULT_DATA_RATE`的比例减小，投递率整体下降；比例越大，投递率整体上升；


###### 2016-02-21  ·  *< 2.5.6 >*

- FIX：`energy-consumption.txt`中输出错误（为按照 x 坐标排序），修复`CNode::getAllNodes()`中使用到的节点排序函数；


###### 2016-02-23  ·  *< 2.5.7 >*

- OPT：<有限能量的测试> 部分节点能量耗尽而死亡时，不应该以全部节点生存期为指标，应以总投递计数为指标；


###### 2016-02-23  ·  *< 2.5.8 >*

- FIX：<有限能量的测试> 输出平均能耗时，应将死亡节点的能耗也计入；


###### 2016-02-23  ·  *< 2.5.9 >*

- OPT：<有限能量的测试>不论运行时间是否超出轨迹文件范围，始终以总投递计数为指标；
- RFCT：将保留小数点后 n 位的操作独立成`NDigitFloat`，放入`GlobalParameters.h`；
- OPT：<有限能量的测试> 针对部分节点死亡的情况，修改`buffer-node.txt`和`buffer-node-statistics.txt`文件中的输出，保证节点信息的前后对应，并使用`-`标出死亡节点；


###### 2016-03-01  ·  *< 2.5.10 >*

- RFCT：类名和文件名
	* `CProcessor`改为`CHelper`；
	* 将所有辅助函数分类整理到全局（`GlobalParameters.h`）、`CFileHelper`、`CSortHelper`三处；
	* `CRoute`类继承自`CGeoEntity`；
- **TODO：**RFCT：将配置参数相关的定义和操作整理到单独的`CConfiguration`类；
- **TODO：**RFCT：将配置参数的默认值读取改用 XML 实现；


###### 2016-03-02  ·  *< 2.5.11 >*

- RFCT：部分类名；
- RFCT：删除所有`using namespace std;`的使用，改为`using cin/ cout/ endl/ string/ ...;`；
- RFCT：修改部分函数参数类型为基类的引用来实现多态，以避免对象分割；
- MNT：将 Resharper 的配置导出到文件`HDC.sln.DotSettings`，并添加到追踪列表；


###### 2016-03-03

- MNT：在`README.md`中的更新日志中加上分类标签，目前包括`ADD：`添加新功能、`MOD：`修改现有实现、`OPT：`功能优化、`BUG：`发现错误、`FIX：`修复错误、`RFCT：`代码重构、`MNT：`版控维护、`NOTE：`附加说明；
- MNT：在`README.md`中添加 Environment 说明；
- MNT：将类图文件`ClassDiagram.cd`添加到追踪列表；
- **TODO：**MNT：重构完成后，将代码合并到master分支；


###### 2016-03-04

- MNT：更新`README.md`文档格式；


###### 2016-03-07  ·  *< 2.6.0 >*  ·  *类重构*

- RFCT：将`GlobalParameters.h`中的全局变量尽量地移入类内静态变量；
- RFCT：对于发送、接收、队列选项的定义和使用，将`Mode`、`SEND`、`RECEIVE`、`QUEUE`等类定义改为类内的枚举类定义`CGeneralNode::_Send`、`CGeneralNode::_Receive`、`CGeneralNode::_Queue`；
- RFCT：对于热点类型的定义和使用，将`TYPE_MERGE_HOTSPOT`、`TYPE_NEW_HOTSPOT`、`TYPE_OLD_HOTSPOT`等宏定义改为类内的枚举类定义`CHotspot::_TypeHotspot`；
- RFCT：新增`CConfiguration`顶级类，为之后添加从文件中读取参数配置的功能做准备；
- RFCT：将主函数中的运行部分移入单独的`Run()`函数；