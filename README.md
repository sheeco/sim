
>***Created by Eco Sun on 2015-11-03***



### debug.txt

    #DataTime	#RunTime	#TransProb	#Spoken	#TTL	#Period	#DefaultDC	
	--(#HotspotDC	#Alpha	#Beta	#Memory)	#Delivery	#Delay	#Energy	#Log


### 参数格式

                                                 !!!!!! ALL CASE SENSITIVE !!!!!!
	<mode>            -har;                  -ihar;                  -hdc;                    
	                  -hotspot-similarity;   -dynamic-node-number;
	<time>            -time-data   [];       -time-run   [];
	<node>            -energy      [];       -sink       [] [];      -range      [];
	<har>             -alpha       [];       -beta       [];         -gamma      [];          
	                  -heat   [] [];               -prob-trans [];
	<ihar>            -lambda      [];       -lifetime   [];
	<prophet>         -hop         [];       -ttl        [];         -queue      [];          
	                  -spoken [];
	<hdc>             -cycle       [];       -dc-default [];         -dc-default [];


### 参数默认值

    scheme                        smac, HAR
    hotspot_similarity            false
    dynamic_node-number           false
    min_wait                      0
    
    alpha                         0.03
    beta                          0.0025
    gamma                         0.5
    heat                          1, 30
    prob_trans                    1.0
    
    <IHAR>
    lambda                        0
    lifetime                      3600
    
    <mHAR>
    merge                         1.0
    old                           1.0
    heat                          flat


### 源文件说明

    Entity.h & .cpp
    
    BasicEntity.h & .cpp
    
    GeoEntity.h & .cpp
    
    GeneralNode.h & .cpp
    
    Processor.h & .cpp
    
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
    
    FileParser.h & .cpp
    
    Preprocesor.h & .cpp
    
    ---
    
    GreedySelection.h & .cpp
    
    PostSelector.h & .cpp
    
    HAR.h & .cpp
    
    ---
    
    NodeRepair.h & .cpp


### 输出文件说明

	-


### 关键类外部接口

	-


---


### 此前的重要更改  ·  *< 2.0.0 >*

* 修正旧版本的热点选取、后续选取中的错误（int 除法等）；

* 所有调试用参数均改为命令行传入，具体参数格式可`-help`查看（但是没有实现足够的合法性检查，因此需要在输入时注意）；

* 归并 HAR (mHAR) 的引入，参数包含`-merge`, `-old`, `-heat[-exp/-ln]`，均由命令行传入；

* 为mHAR中的热度函数添加了指数增长、对数增长的选项（测试结果显示没有影响，原因尚未进行分析，之后的测试都应暂时使用 flat 选项）；

* 加入相邻两轮热点选取结果的相似度比较功能，从命令行传入`-hotspot-similarity`即可；

* 在所有的输出文件前打印时间戳和具体的 scheme 用于识别每一次运行，打印说明信息用于识别每一列信息，并统一内容和文件名的格式；

* 改正数据投递率计算的错误（当前值 instead of 增量）；

* 将调试用的重要参数格式化保存在`debug.txt`中，调试时直接复制到 xls 文档即可；

* 将数据投递分类为 (a) MA在热点上得到投递和 (b) MA在路径上得到投递，并统计两种投递计数的百分比；

* 并对每一个热点统计其投递计数，按照热点选取时槽输出，并伴随着`atHotspot`赋`nullptr`时机的优化；

* mHAR中由于所有选取 ratio 指数增长，在贪婪选取中可能出现 position 集合还没有得到全覆盖，`hotspotsAboveAverage`为空的情况，此时直接选中`unselectedHotspots`中cover数最大的元素即可；


# 更新日志    *2015-11-13 --*

#### 2015-11-13  ·  *< 2.0.1 >*

* 加入最小等待时间(`MIN_WAITING_TIME`)，默认为 0 即不使用该修改，可以从命令行`-min-wait`赋值；

* `delivery-hotspot.txt`中的信息改为降序排序之后再输出，便于分析热点投递计数的集中程度，减少数据后期处理的工作量；


#### 2015-11-16  ·  *< 2.0.2 >*

* 规范化命令行参数输入的关键字，将动态节点数的选项也加入命令行参数；


#### 2015-11-20  ·  *< 2.0.3 >*

* 将node的ID改为自动生成，并随之修改节点数动态变化时的 node 是否存在的判定，添加函数 `HAR::ifNodeExists(int id)` 代替 `id >= m_nodes.size()` 的判定方法（暂未测试）；


#### 2015-11-24  ·  *< 2.1.0 >*

* 动态节点个数测试时，在下一次热点选取之前`HAR::ChangeNodeNumber()`删除被抛弃节点的位置点信息（注意：节点个数变化周期应该为热点选取周期的倍数）；
* 将所有的`g_`系列全局变量都改为相关类内的静态变量（GA等未采用的方法相关的变量未改动）；
* 增加`CHotspot::ratio`及相关方法和改动，用于测试新的 ratio 计算方法，将在贪婪选取和后续选取过程中用到，可使用命令行参数`-balanced-ratio`选项开启；
* 增加`poor-hotspot.txt`用于测试投递计数为 0 的热点信息；
* **应该将动态节点个数测试时的增删节点过程改成随机选取节点；*
* **针对热点的排序应该改成从大到小的；*


#### 2015-11-25  ·  *< 2.1.1 >*

* 对`mergesort`对应的 comp 类的名字进行规范化；
* 按照投递计数降序输出热点cover的position数、node数和ratio到`hotspot-rank.txt`，用于统计和分析热点质量；
* 按照任期统计等待时间的累积值，同样输出到`hotspot-rank.txt`文件；


#### 2015-11-27  ·  *< 2.1.2 >*

* 为每个 position 定于初始值为1的权值，增加`CPreprocessor::DecayPositionsWithoutDeliveryCount`，按照投递计数为0的热点对其 position 执行衰减，可使用命令行参数`-learn`选项开启，`-decay`设置指数型衰减系数；
* 由于衰减后的 position 整体权值水平持续降低，后续选取过程中的固定alpha值将导致选取出的热点总数持续上升，在测试阶段暂时使用`-max-hotspot`来暂时稳定热点总数；
* 增加`MIN_POSITION_WEIGHT`，权值衰减之后如果低于此值将被直接删除，使用命令行参数`-min-weight`赋值（默认值为 1 即不删除）（未测试）；
* 修复`CHotspot::calculateRatio`中的 int 除法问题；
* 修复`CHotspot::waitingTimes`忘记统计的问题；


#### 2015-12-04  ·  *< 2.2.0 >*  ·  *类结构规范化*

* 对整体的类结构做出了调整，类的继承结构规范化；



> ***以上为 HDC 分支的初始状态***

> ***创建自 master 分支 2015-12-04 的 `commit { 0dd0e561bf88035d1cfc557e2083dfa602ab9d43 }`***

> ***以下为 HDC 分支下的更改***



#### 2015-12-04  ·  *< 2.2.1 >*

* 更改工程名及工作路径名为`HDC`；
* 整理删除未使用的函数定义，包括 GA 算法相关的全局变量和处理函数；


#### 2015-12-09

* 修改`CData`和`CNode`类以适应 Epidemic 路由，其中 HOP 或 TTL 的选项通过赋给相应的最大值标明，在`CData`类的各种操作中可以自动判断；


#### 2015-12-10

* 继续修改`CData`和`CNode`类，`CNode`类中原来是针对全部 node 统一统计能耗的，现在改为对于每个节点单独统计能耗，全局的 node 能耗使用`SUM_ENERGY_CONSUMPTION`统计，但尚未添加相关操作；
* 在原来的能耗统计相关宏中添加`BYTE_PER_DATA`，`BYTE_PER_CTRL`，用于指示单个数据包和控制包的大小；
* 增加`CHDC`类、`Epidemic`类和`CMacProtocol`基类，原`HAR`类中有关热点选择的函数暂时放入`CHDC`类，有关路由的类暂时放入`Epidemic`类，其成员属性和函数仍待修改；


#### 2015-12-11  ·  *< 2.3.0 >*  ·  *Epidemic + HDC*

* 用`CNode::state`标识和计算节点的工作状态，取值范围在`[ - SLOT_TOTAL, + SLOT_LISTEN )`之间，值大于等于 0 即代表 Listen 状态；
* 按照通信成功率，多次往返通信时一旦有一次通信失败，之后的通信都不再成立；
* 节点的通信成功率和能量损耗，都已内置在`CNode`类内所有的数据收发操作中；
* 传感器节点的能量损耗来源包括：监听能耗、休眠能耗、数据发送能耗、数据接收能耗；
* 添加节点的总能量属性，以及相应的节点失效、网络运行终止的判定，默认为0即无限能量，可以通过命令行参数`-node-energy`赋值；
* 对于 HDC 中的热点选取操作：暂时只提供 HAR 和 IHAR 可选，参数可通过命令行参数修改；
* 基本修改完毕，各类参数待测试：Epidemic 路由的相关参数、能耗参数、数据生成速率及大小参数、时隙及占空比参数；
* MAC 协议的无关内容：网络初始假定所有节点时隙同步、忽略 Listen 时隙内的 Sync 时隙即全部视作 Data 时隙、忽略传输延迟（之后也可以加入计算）；


#### 2015-12-23  ·  *< 2.3.1 >*

* 将 HDC 和普通 DC 的对比改成：普通 DC 始终使用默认 DC 值（`DEFAULT_DUTY_CYCYLE`）；HDC 在非热点处使用更低（甚至为 0 的）默认 DC 值、热点处使用与普通 DC 中的默认 DC 值相等或更高的热点 DC 值（`HOTSPOT_DUTY_CYCYLE`）；


#### 2015-12-28  ·  *< 2.3.2 >*

* 将`README.txt`改为 MarkDown 文档`README.md`，并调整结构和格式；


#### 2016-01-22

* 将 sink 节点 ID 改为 0（从而在 map 中排在最前方便查找），`CData`、`CPosition`、`CNode`、`CHotspot`的 ID 都改为从 1 开始，实时值仍然等于当前实例总数；
* 添加`Prophet`类，主要包括`CNode::DeliveryPreds`及相关操作函数，Prophet 路由中使用的数据索引仍然使用 Epidemic 路由中的`CNode::SummaryVector`来实现，具体函数待测试；


#### 2016-02-09  ·  *< 2.4.0 >*  ·  *Prophet + HDC*

* 完成`Prophet`类相关的调试；


#### 2016-02-10  ·  *< 2.4.1 >*

* `debug.txt`中自动判断并输出文件头；
* 更新`cout`输出格式，将数据传输和热点选取过程中的输出信息改为改写当前行（即动态显示）的方式（仅更改了 HDC 中调用的部分，`Epidemic`和`HAR`等类中的相关输出暂未更新）；
* 程序运行出错和结束之后蜂鸣提醒（`BEEP 0x07 '\a'`）；


#### 2016-02-11

* 将`Epidemic`类和`Prophet`类中的共有函数整理到父类`CRoutingProtocol`中（包括时槽判断和统一的输出格式），并整理和规范这两个子类中的剩余函数；
* 将`CPreprocessor`类中的部分辅助函数整理到相关类中；


#### 2016-02-14  ·  *< 2.5.0 >*

* 引入 ReSharper 插件，按照 ReSharper 的建议优化代码细节（getter 函数赋 const，前缀操作符，auto 类型等）；
* 针对 Epidemic、Prophet、HAR 三种路由类及其父类的函数做出调整和统一，以及 HAR 和 HDC 中的热点选取函数的调整和统一；
* 整理所有的继承结构及构造函数（访问控制、参数统一、子类函数合并、父类虚析构函数等）；
* 为所有的输出文件保存文件头字符串（`string INFO_DEBUG`等）；
* 将一些选项保存到全局定义的选项类型中（`class SEND::LOOSE`，`class BUFFER::FIFO`），并作为函数参数标识具体操作；
* 将之前使用全局变量（`DO_IHAR`等）标识的 scheme 选项保存到新定义的枚举类型（`MacProtocol`，`RoutingProtocol`，`HotspotSelect`）的全局变量（`MAC_PROTOCOL`，`ROUTING_PROTOCOL`，`HOTSPOT_SELECT`），仍然通过命令行参数赋值；
