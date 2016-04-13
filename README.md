
>***Created by Eco Sun on 2015-11-03***



### Output

	final.log
	parameters.log

	delivery-ratio.log
	delivery-ratio-100.log
	delay.log
	energy-consumption.log
	buffer.log
	buffer-statistics.log
	encounter.log
	transmit.log
	hop.log

	hotspot.log
	at-hotspot.log

	hotspot-similarity.log
	hotspot-rank.log
	delivery-hotspot.log
	delivery-statistics.log
	merge.log
	merge-details.log
	ma.log
	buffer-ma.log
	ed.log



### Usage

`HDC -help` / [./bin/help.md](./bin/help.md)

> 
> **!!! ALL CASE SENSITIVE !!!** 
>  
>                             |    < 运行参数 > 
>  -------------------------- | ---------------------------- 
>   -time-run             []  |  网络运行时间（默认 15000）； 
>   -time-data            []  |  节点数据生成的截止时间（默认 15000）； 
>   -slot                 []  |  网络运行的时槽（默认 1）； 
>   -dataset              []  |  数据集名称（节点轨迹文件路径 ./res/%DATASET%/）； 
>   -log-path             []  |  日志文件路径（默认 ./log/）； 
>   -log-slot             []  |  日志输出时槽（默认 100）； 
>  
>                             |    < 节点参数 > 
>  -------------------------- | ---------------------------- 
>   -sink               [][]  |  Sink 节点的坐标； 
>   -trans-prob           []  |  节点数据传输的成功率（默认 1.0）； 
>   -trans-range          []  |  节点数据传输半径（默认 100）； 
>   -node                 []  |  节点个数（默认 29）； 
>   -buffer               []  |  *节点缓存容量（默认 500）； 
>   -energy               []  |  节点初始能量值（默认无限制）； 
>  
>                             |    < 数据生成参数 > 
>  -------------------------- | ---------------------------- 
>   -data-rate            []  |  节点生成一个数据包需要的时间（默认 30）； 
>   -data-size            []  |  单个数据包的字节数（默认 250）； 
>  
>                             |    < MAC 层参数 > 
>  -------------------------- | ---------------------------- 
>   -hdc                      |  采用 HDC 的占空比协议； 
>   -cycle                []  |  工作周期（默认 300）； 
>   -slot-discover        []  |  每个工作周期中，开始邻居节点发现之前的监听时间（默认 10）； 
>   -dc-default           []  |  默认占空比，即监听时间占整个工作周期的比例（默认 1.0）； 
>   -dc-hotspot           []  |  *HDC 中热点区域使用的占空比； 
>   -random-state             |  采用随机的节点初始工作状态，即异步的占空比； 
> 
>                             |    < 路由层参数 >  
>  -------------------------- | ---------------------------- 
>   -prophet                  |  采用 Prophet 路由协议（默认采用）； 
>   -har                      |  采用 HAR 路由协议； 
>   -hop                  []  |  数据包转发允许的最大跳数（默认无限制）； 
> 
>                             |    < Prophet 协议参数 > 
>  -------------------------- | ---------------------------- 
>   -spoken               []  |  Prophet 路由中节点对最近邻居节点直接跳过通信的计时（默认不使用）； 
>   -pred-init            []  |  Prophet 路由中节点投递概率的初始值（默认 0.75）； 
>   -pred-decay           []  |  Prophet 路由中节点投递概率的衰减系数（默认 0.98）； 
>   -capacity-forward     []  |  Prophet 路由中节点单次转发数据的最大数目（默认无限制）； 
> 
>                             |    < 热点选取参数 > 
>  -------------------------- | ---------------------------- 
>   -ihs                      |  采用 IHAR 的热点选取算法； 
>   -mhs                      |  采用 mHAR 的热点选取算反； 
>   -alpha                []  |  热点选取中，后续选取过程中的阈值参数（默认 0.03）； 
>   -beta                 []  |  热点选取中，判定真热点的最低热度值（默认 0.0025）； 
>   -heat               [][]  |  热点选取中，热度值计算的系数（默认 1， 30）； 
> 
>                             |    < IHAR 参数 > 
>  -------------------------- | ---------------------------- 
>   -buffer-ma            []  |  MA 节点缓存容量（默认 100）； 
>   -lambda               []  |  IHAR 中，疏漏热点修复算法的修复阈值（默认不使用）； 
>   -lifetime             []  |  IHAR 中，节点位置点信息的保存期（默认不使用）； 
> 
>                             |    < mHAR 参数 > 
>  -------------------------- | ---------------------------- 
>   -merge                []  |  mHAR 中，归并热点的选取系数（默认 1.0）； 
>   -old                  []  |  mHAR 中，旧热点的选取系数（默认 1.0）； 
>   -balanced-ratio           |  采用综合的热点选取度量函数； 
> 
>                             |    < 测试参数 > 
>  -------------------------- | ---------------------------- 
>   -dynamic-node-number      |  节点个数动态变化的测试； 
>   -hotspot-similarity       |  对于热点选取，测试前后热点选取结果的相似度（默认采用）； 
> 



### Dependency

- [TinyXML2 3.0.0](http://www.grinninglizard.com/tinyxml/)


### Environment

- Windows 10
- Visual Studio Enterprise 2015 Update 2 ( 14.0.25123.00 )
- Microsoft Visual C++ 2015 ( 00322-90150-00888-AA891 )


### Files

    README.md
    
    version.h & HDC.rc

    Global.h & .cpp
    
    main.cpp
    
    ---
    
    Entity.h
    
	|-- Package.h & .cpp

    |-- BasicEntity.h & .cpp

		|-- GeneralData.h & .cpp

			|-- Ctrl.h & .cpp

			|-- Data.h & .cpp

		|-- GeneralNode.h & .cpp

			|-- Sink.h & .cpp
    
			|-- Node.h & .cpp
    
			|-- MANode.h & .cpp

    |-- GeoEntity.h & .cpp
    
		|-- Coordinate.h & .cpp
    
		|-- CTrace.h & .cpp
    
		|-- Position.h & .cpp
    
		|-- Hotspot.h & .cpp

		|-- Route.h & .cpp
    
    Process.h

    |-- Algorithm.h
    
		|-- Protocol.h & .cpp

			|-- RoutingProtocol.h & .cpp

				|-- Prophet.h & .cpp
	
				|-- HAR.h & .cpp

				|-- Epidemic.h & .cpp
	    
			|-- MacProtocol.h & .cpp
	
				|-- SMac.h & .cpp
	
				|-- HDC.h & .cpp

		|-- HotspotSelect.h & .cpp
    
		|-- PostSelect.h & .cpp
    
		|-- NodeRepair.h & .cpp

    |-- Helper.h & .cpp
    
		|-- FileHelper.h & .cpp
    
		|-- SortHelper.h & .cpp
    
	Configuration.h
        


### Interface

	...


---


#### Log Mark

*在 Git Commit Comment 中使用快速标签：*

　　 `+` 添加、`-` 删除、`~` 修改、`#` 优化、`?` 尝试、`!` 错误、`$` 修复、`%` 测试；

*在 `README.md` 的更新日志中使用分类标记标签：*

　　 `ADD` 添加新功能、`REM` 删除现有功能、`MOD` 修改现有实现、`OPT` 功能优化、`TRY` 不确定尝试；

　　 `BUG` 待修复错误、`FIX` 修复错误、`TEST` 功能测试；

　　 `RFCT` 代码重构、`MNT` 版控维护、`NOTE` 附加说明、`TODO` 计划任务；



# Update LOG    * 2015-11-13 -- *


## 2.* : Previous Modifications


###### 2015-09-01 -- 2015-11-13  ·  *< 2.0.0 >*

- 修正旧版本的热点选取、后续选取中的错误（int 除法等）；

- 所有调试用参数均改为命令行传入，具体参数格式可 `-help` 查看（但是没有实现足够的合法性检查，因此需要在输入时注意）；

- 归并 HAR (mHAR) 的引入，参数包含 `-merge`, `-old`, `-heat[-exp/-ln]`，均由命令行传入；

- 为 mHAR 中的热度函数添加了指数增长、对数增长的选项（测试结果显示没有影响，原因尚未进行分析，之后的测试都应暂时使用 flat 选项）；

- 加入相邻两轮热点选取结果的相似度比较功能，从命令行传入 `-hotspot-similarity` 即可；

- 在所有的输出文件前打印时间戳和具体的 scheme 用于识别每一次运行，打印说明信息用于识别每一列信息，并统一内容和文件名的格式；

- 改正数据投递率计算的错误（当前值 instead of 增量）；

- 将调试用的重要参数格式化保存在 `debug.txt` 中，调试时直接复制到 xls 文档即可；

- 将数据投递分类为 (a) MA 在热点上得到投递和 (b) MA 在路径上得到投递，并统计两种投递计数的百分比；

- 并对每一个热点统计其投递计数，按照热点选取时槽输出，并伴随着 `atHotspot` 赋 `nullptr` 时机的优化；

- mHAR 中由于所有选取 ratio 指数增长，在贪婪选取中可能出现 position 集合还没有得到全覆盖，`hotspotsAboveAverage` 为空的情况，此时直接选中 `unselectedHotspots` 中 cover 数最大的元素即可；


###### 2015-11-13  ·  *< 2.0.1 >*

- ADD：加入最小等待时间(`MIN_WAITING_TIME`)，默认为 0 即不使用该修改，可以从命令行 `-min-wait` 赋值；

- ADD：`delivery-hotspot.txt` 中的信息改为降序排序之后再输出，便于分析热点投递计数的集中程度，减少数据后期处理的工作量；


###### 2015-11-16  ·  *< 2.0.2 >*

- ADD：规范化命令行参数输入的关键字，将动态节点数的选项也加入命令行参数；


###### 2015-11-20  ·  *< 2.0.3 >*

- ADD：将 node 的 ID 改为自动生成，并随之修改节点数动态变化时的 node 是否存在的判定，添加函数 `HAR::ifNodeExists(int id)` 代替 `id >= m_nodes.size()` 的判定方法（暂未测试）；


### 2.1.*


###### 2015-11-24  ·  *< 2.1.0 >*

- FIX：<动态节点个数测试> 在下一次热点选取之前 `HAR::ChangeNodeNumber()` 删除被抛弃节点的位置点信息（注意：节点个数变化周期应该为热点选取周期的倍数）；
- RFCT：将所有的 `g_` 系列全局变量都改为相关类内的静态变量（GA 等未采用的方法相关的变量未改动）；
- ADD：增加 `CHotspot::ratio` 及相关方法和改动，用于测试新的 ratio 计算方法，将在贪婪选取和后续选取过程中用到，可使用命令行参数 `-balanced-ratio` 选项开启；
- ADD：增加 `poor-hotspot.txt` 用于测试投递计数为 0 的热点信息；
- [ ] ADD：应该将动态节点个数测试时的增删节点过程改成随机选取节点；
- [ ] MOD：针对热点的排序应该改成从大到小的；


###### 2015-11-25  ·  *< 2.1.1 >*

- RFCT：对 `mergesort` 对应的 comp 类的名字进行规范化；
- ADD：按照投递计数降序输出热点 cover 的 position 数、node 数和 ratio 到 `hotspot-rank.txt`，用于统计和分析热点质量；
- ADD：按照任期统计等待时间的累积值，同样输出到 `hotspot-rank.txt` 文件；


###### 2015-11-27  ·  *< 2.1.2 >*

- TRY：为每个 position 定于初始值为 1 的权值，增加 `CSortHelper::DecayPositionsWithoutDeliveryCount`，按照投递计数为 0 的热点对其 position 执行衰减，可使用命令行参数 `-learn` 选项开启，`-decay` 设置指数型衰减系数；
- NOTE：由于衰减后的 position 整体权值水平持续降低，后续选取过程中的固定 alpha 值将导致选取出的热点总数持续上升，在测试阶段暂时使用 `-max-hotspot` 来暂时稳定热点总数；
- TRY：增加 `MIN_POSITION_WEIGHT`，权值衰减之后如果低于此值将被直接删除，使用命令行参数 `-min-weight` 赋值（默认值为 1 即不删除）（未测试）；
- FIX：修复 `CHotspot::calculateRatio` 中的 int 除法问题；
- FIX：修复 `CHotspot::waitingTimes` 忘记统计的问题；


### 2.2.* : 类重构


###### 2015-12-04  ·  *< 2.2.0 >*  ·  *类重构*

- RFCT：对整体的类结构做出了调整，类的继承结构规范化；



> *** 以上为 HDC 分支的初始状态 ***

> *** 创建自 [ 2015-12-04 的 master 分支 ]( 0dd0e561bf88035d1cfc557e2083dfa602ab9d43 ) ***

> *** 以下为 HDC 分支下的更改 ***


　
###### 2015-12-04  ·  *< 2.2.1 >*

- MOD：更改工程名及工作路径名为 `HDC`；
- OPT：整理删除未使用的函数定义，包括 GA 算法相关的全局变量和处理函数；


###### 2015-12-09

- MOD：修改 `CData` 和 `CNode` 类以适应 Epidemic 路由，其中 HOP 或 TTL 的选项通过赋给相应的最大值标明，在 `CData` 类的各种操作中可以自动判断；


###### 2015-12-10

- MOD：继续修改 `CData` 和 `CNode` 类，`CNode` 类中原来是针对全部 node 统一统计能耗的，现在改为对于每个节点单独统计能耗，全局的 node 能耗改为调用时统计；
- ADD：在原来的能耗统计相关宏中添加 `SIZE_DATA`，`SIZE_CTRL`，用于指示单个数据包和控制包的大小；
- ADD：增加 `CHDC` 类、`Epidemic` 类和 `CMacProtocol` 基类，原 `HAR` 类中有关热点选择的函数暂时放入 `CHDC` 类，有关路由的类暂时放入 `Epidemic` 类，其成员属性和函数仍待修改；


### 2.3.* : Epidemic + HDC


###### 2015-12-11  ·  *< 2.3.0 >*  ·  *Epidemic + HDC*

- ADD：用 `CNode::state` 标识和计算节点的工作状态，取值范围在 `[ - SLOT_TOTAL, + SLOT_LISTEN )` 之间，值大于等于 0 即代表 Listen 状态；
- ADD：按照通信成功率，多次往返通信时一旦有一次通信失败，之后的通信都不再成立；
- ADD：节点的通信成功率和能量损耗，都已内置在 `CNode` 类内所有的数据收发操作中；
- ADD：传感器节点的能量损耗来源包括：监听能耗、休眠能耗、数据发送能耗、数据接收能耗；
- ADD：添加节点的总能量属性，以及相应的节点失效、网络运行终止的判定，默认为 0 即无限能量，可以通过命令行参数 `-node-energy` 赋值；
- ADD：对于 HDC 中的热点选取操作：暂时只提供 HAR 和 IHAR 可选，参数可通过命令行参数修改；
- NOTE：基本修改完毕，各类参数待测试：Epidemic 路由的相关参数、能耗参数、数据生成速率及大小参数、时隙及占空比参数；
- NOTE：MAC 协议的无关内容：网络初始假定所有节点时隙同步、忽略 Listen 时隙内的 Sync 时隙即全部视作 Data 时隙、忽略传输延迟（之后也可以加入计算）；


###### 2015-12-23  ·  *< 2.3.1 >*

- MOD：将 HDC 和普通 DC 的对比改成：普通 DC 始终使用默认 DC 值（`DEFAULT_DUTY_CYCYLE`）；
- ----- HDC 在非热点处使用更低（甚至为 0 的）默认 DC 值、热点处使用与普通 DC 中的默认 DC 值相等或更高的热点 DC 值（`HOTSPOT_DUTY_CYCYLE`）；


###### 2015-12-28  ·  *< 2.3.2 >*

- MNT：将 `README.txt` 改为 MarkDown 文档 `README.md`，并调整结构和格式；


###### 2016-01-22

- OPT：将 sink 节点 ID 改为 0（从而在 map 中排在最前方便查找），`CData`、`CPosition`、`CNode`、`CHotspot` 的 ID 都改为从 1 开始，实时值仍然等于当前实例总数；
- ADD：添加 `Prophet` 类，主要包括 `CNode::DeliveryPreds` 及相关操作函数，Prophet 路由中使用的数据索引仍然使用 Epidemic 路由中的 `CNode::SummaryVector` 来实现，具体函数待测试；


### 2.4.* : Prophet + HDC


###### 2016-02-09  ·  *< 2.4.0 >*  ·  *Prophet + HDC*

- FIX：完成 `Prophet` 类相关的调试；
- OPT：轨迹文件 .newlocation 的序号改为从 1 开始，序号对应 `CNode::ID`，增删节点时仍保持对应关系；
- ADD：在解决方案中新增 file-formatter 项目，用于自动修改轨迹文件序号；


###### 2016-02-10  ·  *< 2.4.1 >*

- OPT：`debug.txt` 中自动判断并输出文件头；
- OPT：更新 `cout` 输出格式，将数据传输和热点选取过程中的输出信息改为改写当前行（即动态显示）的方式（仅更改了 HDC 中调用的部分，`Epidemic` 和 `HAR` 等类中的相关输出暂未更新）；
- OPT：程序运行出错和结束之后蜂鸣提醒（`BEEP 0x07 '\a'`）；


###### 2016-02-11  ·  *< 2.4.2 >*

- RFCT：将 `Epidemic` 类和 `Prophet` 类中的共有函数整理到父类 `CRoutingProtocol` 中（包括时槽判断和统一的输出格式），并整理和规范这两个子类中的剩余函数；
- RFCT：将 `CSortHelper` 类中的部分辅助函数整理到相关类中；


### 2.5.* : ReSharper


###### 2016-02-14  ·  *< 2.5.0 >*  ·  *ReSharper*

- RFCT：引入 ReSharper 插件，按照 ReSharper 的建议优化代码细节（getter 函数赋 const，前缀操作符，auto 类型等）；
- RFCT：针对 Epidemic、Prophet、HAR 三种路由类及其父类的函数做出调整和统一，以及 HAR 和 HDC 中的热点选取函数的调整和统一；
- RFCT：整理所有的继承结构及构造函数（访问控制、参数统一、子类函数合并、父类虚析构函数等）；
- OPT：为所有的输出文件保存文件头字符串（`string INFO_DEBUG` 等）；
- RFCT：将一些选项保存到全局定义的配置类型中（`SEND::COPY/DUMP`、`RECEIVE::LOOSE/SELFISH`、`QUEUE::FIFO/LIFO`），并作为函数参数标识具体操作；
- RFCT：将之前使用全局变量（`DO_IHAR` 等）标识的 scheme 选项保存到新定义的枚举类型（`_MacProtocol`，`_RoutingProtocol`，`_HotspotSelect`）的全局变量（`MAC_PROTOCOL`，`ROUTING_PROTOCOL`，`HOTSPOT_SELECT`），仍然通过命令行参数赋值；


###### 2016-02-15  ·  *< 2.5.1 > < 2.5.2 >*


###### 2016-02-16  ·  *< 2.5.3 >*


###### 2016-02-18  ·  *< 2.5.4 >*


###### 2016-02-20  ·  *< 2.5.5 >*

- RFCT：原宏定义 `DATA_GENERATE_RATE`、`BYTE_PER_CTRL` 和 `BYTE_PER_DATA` 移入 `CNode` 中的静态成员变量 `CNode::DEFAULT_DATA_RATE`、`CNode::SIZE_DATA` 和 `CGeneralNode::SIZE_CTRL`；
- NOTE：`CNode::SIZE_DATA` 和 `CGeneralNode::SIZE_CTRL` 的比例太小时，HDC 的平均能耗将比普通 DC 更大；比例越大，改善效果越明显；
- NOTE：`CNode::CAPACITY_BUFFER` 和 `CNode::DEFAULT_DATA_RATE` 的比例减小，投递率整体下降；比例越大，投递率整体上升；


###### 2016-02-21  ·  *< 2.5.6 >*

- FIX：`energy-consumption.txt` 中输出错误（为按照 x 坐标排序），修复 `CNode::getAllNodes()` 中使用到的节点排序函数；


###### 2016-02-23  ·  *< 2.5.7 >*

- OPT：<有限能量的测试> 部分节点能量耗尽而死亡时，不应该以全部节点生存期为指标，应以总投递计数为指标；


###### 2016-02-23  ·  *< 2.5.8 >*

- FIX：<有限能量的测试> 输出平均能耗时，应将死亡节点的能耗也计入；


###### 2016-02-23  ·  *< 2.5.9 >*

- OPT：<有限能量的测试>不论运行时间是否超出轨迹文件范围，始终以总投递计数为指标；
- RFCT：将保留小数点后 n 位的操作独立成 `NDigitFloat`，放入 `Global.h`；
- OPT：<有限能量的测试> 针对部分节点死亡的情况，修改 `buffer-node.txt` 和 `buffer-node-statistics.txt` 文件中的输出，保证节点信息的前后对应，并使用 `-` 标出死亡节点；


###### 2016-03-01  ·  *< 2.5.10 >*

- RFCT：类名和文件名
	* `CProcessor` 改为 `CHelper`；
	* 将所有辅助函数分类整理到全局（`Global.h`）、`CFileHelper`、`CSortHelper` 三处；
	* `CRoute` 类继承自 `CGeoEntity`；


###### 2016-03-02  ·  *< 2.5.11 >*

- RFCT：部分类名；
- RFCT：删除所有 `using namespace std;` 的使用，改为 `using cin/ cout/ endl/ string/ ...;`；
- RFCT：修改部分函数参数类型为基类的引用来实现多态，以避免对象分割；
- MNT：将 Resharper 的配置导出到文件 `HDC.sln.DotSettings`，并添加到追踪列表；


###### 2016-03-03

- MNT：在 `README.md` 中的更新日志中加上分类标签，参见 *[Log Mark](#log-mark)* ；
- MNT：在 `README.md` 中添加 Environment 说明；
- MNT：将类图文件 `ClassDiagram.cd` 添加到追踪列表；


###### 2016-03-04

- MNT：更新 `README.md` 文档格式；


### 2.6.* : 类重构


###### 2016-03-07  ·  *< 2.6.0 >*  ·  *类重构*

- RFCT：将 `Global.h` 中的全局变量尽量地移入类内静态变量；
- RFCT：对于发送、接收、队列选项的定义和使用，将 `Mode`、`SEND`、`RECEIVE`、`QUEUE` 等类定义改为类内的枚举类定义 `CGeneralNode::_Send`、`CGeneralNode::_Receive`、`CGeneralNode::_Queue`；
- RFCT：对于热点类型的定义和使用，将 `TYPE_MERGE_HOTSPOT`、`TYPE_NEW_HOTSPOT`、`TYPE_OLD_HOTSPOT` 等宏定义改为类内的枚举类定义 `CHotspot::_TypeHotspot`；
- RFCT：新增 `CConfiguration` 顶级类，为之后添加从文件中读取参数配置的功能做准备；
- RFCT：将主函数中的运行部分移入单独的 `Run()` 函数；


###### 2016-03-08  ·  *< 2.6.1 >*

- MNT：自定义 VS 和 Resharper 快捷键，并保存在文件 `HDC.vssettings` 中，加入项目追踪列表；
- RFCT：调整项目的代码目录结构；


###### 2016-03-09  ·  *< 2.6.2 >*

- RFCT：将枚举类名格式改为 `_ALL_UPPERCASE`，系统行为宏定义格式改为 `_ALL_UPPERCASE_`；
- RFCT：调整项目的代码目录结构，将 VS 工程和设置文件等移入 `/.project/` 文件夹下（Resharper 的配置导出到文件 `/.project/resharper.DotSettings`，VS 的配置导出到文件 `/.project/vs.vssettings`）；


###### 2016-03-09  ·  *< 2.6.3 >*

- RFCT：将轨迹文件从 `.newlocation` 改为 `.trace` 文件，并放入 `/res/$DATASET$/` 文件夹中；
- ADD：数据集名字 `DATASET` 为可定制参数（尚未添加读取代码）；
- FIX：之前使用 `nodes.size()` 替换 `NUM_NODE` 时造成的错误；
- MNT：在 git commit message 中使用快速标签来概括更改，参见 *[Log Mark](#log-mark)* ；


###### 2016-03-10  ·  *< 2.6.4 >*

- ADD：不同节点的生存时间最大值可以不同，由 trace 文件决定；
- MOD：将把死亡节点清除到 `CNode::deadNodes` 中的操作独立成一个函数 `CNode::ClearDeadNodes()`；
- MOD：由于 `CRoutingProtocol::UpdateNodeStatus()` 中也可能发生节点删除，将所有的路由协议中的 `CNode::hasNodes()` 判断移到此步骤之后；
- FIX：在热点选取之前，丢弃死亡节点的 position 记录（`CHotspotSelect::CollectNewPositions()`）；
- FIX：热点区域相遇百分比的统计存在的关键性错误（`CProphet::SendData()`）；
- ADD：`CNode::encounterActive` 用于统计有效的节点相遇计数；


###### 2016-03-11  ·  *< 2.6.5 >*

- MOD：平均能耗的统计，从以 byte 为单位改为以 package 为单位；
- MOD：将增删节点和更新节点位置、工作状态等操作，整理到 MAC 协议层中（暂时全部放入 `CHDC`）；
- RFCT：将输出调试信息的操作分别整理到 MAC 层和路由层的父类和子类中，以实现尽可能多的重用；
- ADD：增加全局函数 `Exit()`，并将输出时间戳的操作统一放入其中；


###### 2016-03-16

- ADD：添加类 `CSMac`，并将 MAC 层的基本操作从 `CHDC` 中移入新增类 `CSMac` 中；


###### 2016-03-16  ·  *< 2.6.6 >*

- FIX：整理路由类和 Mac 类的 `PrintInfo()` 和 `PrintFinal` 输出函数；
- ADD：将 `x` 和 `y` 两个变量包装成 `CCoordinate` 类；
- RFCT：暂时将各个类内静态变量的访问权限全设为 `public`，并将初始化操作都移入 `initConfiguration()`；
- FIX：根据 ref，修改 `SIZE_DATA` 最大值为 250 字节，`SIZE_CTRL` 参考值为 10 字节，将原 `BEACON_SIZE` 改为 `SIZE_HEADER_MAC`，参考值为 8 字节；


###### 2016-03-17  ·  *< 2.6.7 >*

- ADD：将所有全局变量和辅助函数，包装到命名空间 `namespace global`；


###### 2016-03-17

- ADD：添加类 `CGeneralData`、`CCtrl`、`CPackage`，和函数 `CNode::broadcastPackage()`、`::transmitPackage()`、`::sendRTS()`、`::receiveRTS()`、`::sendCTS()`、`::receivePackage()`；


###### 2016-03-18  ·  *< 2.6.8 >*

- ADD：将数据通信的操作整合到 `CMacProtocol::broadcastPackage()`、`::transmitPackage()`；
- ADD：将数据发送的响应流程整合到 `CMacProtocol::receivePackage()`，涉及到路由层的操作提交给相应路由类处理（暂时只实现了 Prophet 路由，`CProphet::bufferData()` 和 `::selectDataByIndex`）；


## 3.* : 重写 MAC 层


###### [ 2016-03-18  ·  *< 3.0.0 >*  ·  *重写 MAC 层* ]( b50107fb04bd0976fc54aa0a81e47b2f6d619e1b )

- ADD：完成新定义的函数 `CMacProtocol::broadcastPackage()`、`::transmitPackage()`、`::receivePackage()`、`CProphet::bufferData()`、`::selectDataByIndex`；
- MOD：删除原有的 `CRoutingProtocol::SendData()` 系列实现；
- NOTE：暂时只实现了 Prophet 路由，未测试；未重写 Epidemic 路由和 HAR 路由；
        

### 3.1.* : 重写 MAC 层


###### [ 2016-03-20  ·  *< 3.1.0 >* ]( 684137e5b575ebf6f1bffb06f44f48e9fa2d1db7 )

- RFCT：添加 `FILE_xxx` 和 `PATH_xxx` 系列全局变量，易于管理和更改；
- ADD：将输出文件的后缀改为 `.log`，不同运行结果将放入标注时间戳的不同文件夹下（如 `/log/.xxxx-xx-xx xx:xx:xx`），运行完成之后去除 `.` 前缀；
- MOD：最终的输出文件由 `debug.txt` 更名为 `final.log`，并拷贝到父文件夹中（仅当正常运行完成退出时）；
- ADD：对所有的退出标记不同的代码，代码 0 为正常运行完成退出，小于 0 为未开始运行的直接退出，大于 0 为错误代码；
- FIX：初步测试新的 MAC 层函数，修复一些小错误；
- OPT：替换掉所有不必要的前置递加操作符 `++var`；


###### [ 2016-03-21  ·  *< 3.1.5923.38461 >* ]( 04ed66ee1958827e8aa75e74a98444c8224e65fc )

- ADD：添加生成版本号配置文件 `AssemblyInfo.cpp` 和 `HDC.rc`，并修改相关的项目属性为“使用公共语言运行时支持(/clr)”、“多线程 DLL(\MD)”、“在共享 DLL 中使用 MFC”；
- MNT：使用 VS 插件 Versioning Controlled Build 进行版本号管理；
- MNT：改用 MS 默认的版本号规则，编译号和修订号由生成日期和时间决定，同时定义对可执行文件写入的属性信息；
- ADD：使用 `ERRONO` 标记程序的返回值，并自定义扩展错误代码；
- FIX：`final.txt` 拷贝到父文件夹出错；
- FIX：输出文件夹重命名失败；


###### [ 2016-03-21  ·  *< 3.1.5924.1169 >* ]( 2ad5fe57cc9d76fddfc53bf1a0894519cf3d6906 )

- ADD：重新添加所有的控制台输出；
- FIX：`CPackage` 类构造函数的错误；
- FIX：禁用 `CData::ID_MASK`，修复因此产生的错误；


###### [ 2016-03-21  ·  *< 3.1.5924.21445 >* ]( cbd00e950480529860836007f447394a1ef32295 )

- TRY：为投递概率设置容差 `TOLERANCE_PRED`，使用宏定义 `USE_PRED_TOLERANCE` 启用（测试：会破坏梯度，最终删除）；
- MNT：调整和统一 `README.md` 的格式；
- TEST：投递概率容差；


###### [ 2016-03-22  ·  *< 3.1.5925.19658 >* ]( 7de8dc84a46985d1fd04ec8201f5ca4ef9d143d9 )

- ADD：添加命令行参数 `-pred-tolerance`、`-log-path`、`-log-slot`；
- FIX：模板类函数 `GetItemsByID()` 可能不能成功调用？
- FIX：`CProphet::selectDataByIndex()` 的错误；
- FIX：`CMacProtocol::receivePackage()` 的迭代错误；


###### [ 2016-03-22  ·  *< 3.1.5925.29760 >* ]( c95ff6cce1eefcd8145a1272ba578d4ad41c4c6e )

- OPT：运行发生异常退出时，输出错误信息到文件 `error.log`；
- OPT：优化命令行参数的使用帮助信息；


### 3.2.* : RTS 捎带 DP


###### [ 2016-03-23  ·  *< 3.2.5926.22497 >* ]( af30b1aed1c80c2f909c74a2add441cc26089bb6 )

- RFCT：规范化变量命名和 getter 函数名；
- FIX：更改节点是否将进行邻居节点发现的方法， 添加成员变量 `CNode::discovering`，以修复 `CMacProtocol::broadcastPackage()` 中节点相遇和数据传输计数重复计算的问题；
- FIX：更改数据传输计数的计算方法，单方节点收到一组数据就认为是一次数据传输成功；
- ADD：添加成员变量 `CNode::encounterActiveAtHotspot`，更改相遇计数的计算方法；
- ADD：添加节点工作状态随机初始化的可选功能，由变量 `CMacProtocol::RANDOM_STATE_INIT` 标记，命令行参数 `-random-state`；


###### [ 2016-03-23  ·  *< 3.2.5926.22866 >* ]( bbdcaf90421a36b0b3ff97a9b8cd2c6982d41acb )


###### [ 2016-03-23  ·  *< 3.2.5926.30658 >* ]( 3ee22ba30ad1cc21b07f785f013339f7368d49f4 )

- FIX：取消 `USE_TOLERANCE_PRED`；
- FIX：修复遗漏引用 `CNode::updateDeliveryPredsWithSink()` 的重要错误；


###### [ 2016-03-23  ·  *< 3.2.5926.31165 >* ]( 57912b77acaf0d9ce19d470ba6c106acaf92d294 )


###### [ 2016-03-24  ·  *< 3.2.5926.41581 >* ]( 6dab843a5108b472b1f642e88797d1253aa40ed8 )

- FIX：修改 `CMacProtocol::receivePackage` 中的数据传输过程，发送 RTS 时直接捎带投递概率；
- FIX：修改 `CMacProtocol::receivePackage` 中的数据传输计数方法，对于每个节点对只计入一次数据传输，以收到 ACK 为准则；


###### [ 2016-03-24  ·  *< 3.2.5927.16957 >* ]( dc6a2d7dcfde527f1c85a7201a5c2ff41e2f3440 )

- FIX：修改项目属性，添加链接器命令行参数 `/NODEFAULTLIB:MSVCRT`，以解决 `Warning LNK4098`；
- ADD：宏定义 `_WIN32_WINNT`，用于为编译器指示系统版本；
- FIX：修复 `CMacProtocol::receivePackage` 中的响应逻辑的重要错误；
- NOTE：只要路由协议允许转发数据，就将发送 DATA 或 NODATA 包，收到二者中的任何一个，都应回复 ACK；
- ADD：添加成员变量 `CProphet::TRANS_STRICT_BY_PRED`，指示是否严格以投递概率**真**大于本节点为判断准则；
- ----- （默认）值 `true` 时，对于相等投递概率的节点也允许发送（但仍保持单向）；值为 `true` 时，对于相等投递概率的节点，将以 0.5 的概率决定发送；
- ADD：添加命令行参数 `-node` 用于定义节点个数；
- RFCT：添加函数 `CBasicEntity::withinRange()` 替换多数 `CBasicEntity::getDistance()` 的引用；


###### [ 2016-03-24  ·  *< 3.2.5927.20040 >* ]( fde3f0dc45bc1ca95317f803663aacc46b9bce92 )

- RFCT：将数据包响应逻辑从 `CMacProtocol::receivePackage()` 中尽量分离到 `CProphet::receiveContents()` 中；


###### [ 2016-03-25  ·  *< 3.2.5928.23405 >* ]( 7fa4d90b398c905de325e4ab0e20e3515e50ab21 )

- RFCT：将 `CProphet::receiveContents()` 中的响应操作按照源-目的节点对的类型，分类整合到三个重载的同名函数中；
- FIX：函数 `CMacProtocol::CommunicateWithNeighbor()` 更名为 `CMacProtocol::CommunicateWithNeighbor()`，并修复函数调用错误；
- FIX：修复 `CProphet::receiveContents()` 中的错误；
- OPT：将 `CData::MAX_HOP` 和 `CData::MAX_TTL` 的冲突，改为在 `ParseArguments()` 中检查，如有冲突将报错退出；
- BUG：数据传输计数存在差错，应检查 `CMacProtocol::transmitTry()` 和 `CMacProtocol::transmitSucceed()` 的调用；


###### [ 2016-03-28  ·  *< 3.2.5931.21338 >* ]( dac9dd3379f583fc8ef510c6471ceeccdceaa514 )

- BUG：数据传输计数的差错；
- RFCT：将热点选取的相关输出分离移入到 `CHotspotSelect` 类中；
- ADD：将版本信息复制到日志文件夹中（`HDC.version`）；
- ADD：添加 XML 解析库 TinyXML2 的相关文件 `/lib/xml/tinyxml2.h & .cpp` 及 `CXmlHelper` 类；


###### [ 2016-03-29  ·  *< 3.2.5932.17811 >* ]( eecbc4b00b64c8bc1dee0d062a53541dd30826ab )

- ADD：添加 `_HOTSPOT_SELECT::_none` 用于独立开启热点选取操作的功能，但暂时没有添加独立的节点是否位于热点中的检测操作；
- ADD：在参数解析中添加异常捕获，如果命令行参数存在错误，则禁止运行；
- ADD：添加命令行参数 `-capacity-forward`，赋值 Prophet 路由中单次数据转发的最大数据包数目；
- ADD：重写 xHAR 路由的数据通信流程，函数 `HAR::receiveContents()` 待完成；
- BUG：Prophet 路由中单次数据转发的最大数据包数目，对运行结果不造成影响；


### 3.3.* : 重写 xHAR


###### [ 2016-03-30  ·  *< 3.3.5933.21567 >* ]( e1d4990c1d264bf4ac82eb1db3b135019e15ba32 )

- ADD：完成函数 `HAR::receiveContents()`（待测试）；
- RFCT：将 `CHDC::UpdateDutyCycleForNodes()` 中更新节点所在热点区域的操作提取到 `CHotspot::UpdateAtHotspotForNodes()`；
- ADD：在 `CSmac::Operate()` 中添加独立开启热点选取的操作，添加独立的节点是否位于热点区域的检测操作；
- REM：暂时删除所有 Epidemic 路由的相关定义、操作和引用，包括 `-ttl` 和 `-queue` 的使用；
- ADD：对于 MA 节点，如果路线过期或缓存已满（_selfish 模式下），立即返回 sink；
- ADD：添加 Prophet 和 HAR 路由中对 _selfish 模式的支持；
- ADD：添加命令行参数 `-buffer-ma`，并将 `-buffer` 默认值改为 0，即必选参数；


###### [ 2016-03-31  ·  *< 3.3.5934.19858 >* ]( 7dd414d67b6620cec73fea917d15b7a6a58a62b2 )

- ADD：添加 `sink.log` 的输出；
- RFCT：将节点相遇计数及热点区域相遇计数的统计移入 `CHotspot::UpdateAtHotspotForNodes()` 中，统计时槽将独立于运行粒度，仅由移动模型粒度决定，；


###### [ 2016-04-04  ·  *< 3.3.3 >* ]( c8d9fd949928f8ac63a335f9db8bd8f101e3f174 )

- MNT：更新到 Visual Studio 2015，项目文件和设置文件相应更新；
- MNT：停止使用 Resharper，暂时保留设置文件；
- MNT：停止使用 Versioning Controlled Build，暂时移除 `AssembllyInfo.cpp` 和 `HDC.version`；
- MNT：改用 `version.h` 和 `HDC.rc` 实现版本号控制，从基于系统时间的子版本号改为递增子版本号，需要每次手动修改版本号变量（`version.h` 中）；


###### [ 2016-04-06  ·  *< 3.3.4 >* ]( b88c685d4f91ada83d5f2f677af0d7d6fd0e361b )

- FIX：指针重复释放的错误及输出错误；


###### [ 2016-04-08  ·  *< 3.3.5 >* ]( 1d48ec73413cc02785ab79edcca4354b920c028a )

- OPT：将传输计数的统计及所有相关定义移入 `CMacProtocol` 类内，仅当发生有效数据交付时才认为传输成功；
- OPT：优化 `final.log` 文件中的参数输出；
- REM：删除投递概率容差的所有定义和引用；
- MNT：优化项目属性中，生成文件及中间文件目录的设置；
- [ ] MOD：数据生成率的定义应从“个数 / 秒”改为“字节 / 秒”，相应地，该定义的引用也需要更改；


### 3.4.* : 模拟连续轨迹


###### [ 2016-04-12  ·  *< 3.4.1 >* ]( 4ba7d15999a41b34c0ef83a429d5f768888e00a8 )

- ADD：类 `CCTrace`（`CTrace` 为保留字），以及模拟连续轨迹的功能，由 `CCTrace::CONTINUOUS_TRACE` 指示；
- RFCT：将原 `CFileHelper` 类中的轨迹文件读取操作，移入 `CCTrace` 类中；
- ADD：重载类 `CCoordinate` 的算术操作符；
- FIX：热点选取中，位置点信息收集的调用时槽错误；
- [ ] ADD：如果轨迹文件中的时槽为动态变化的，需要在 `CCTrace::getLocation()` 中增加相应的读取代码；
- [ ] OPT：对所有操作符重载进行补全和标准化；
- [ ] RFCT：使用条件编译包裹不同协议相关的类内成员和函数的定义和引用；
- [ ] RFCT：将文件夹操作提取到 `CFileHelper` 类；


###### [ 2016-04-13  ·  *< 3.4.2 >* ]( 6bb139e98ddd1aa949b4aa0a550008897153d96e )

- MNT：优化 `README.md` 和 `help.md` 的输出格式；
- MNT：将命令行参数使用说明提取到文件 `help.md`，更新版本信息文件 `HDC.version` 的输出；


###### 2016-04-14  ·  *< 3.4.3 >*

- RFCT：整理路由类和 MAC 类协议的操作函数及调用逻辑，增加 `CMacProtocol::Prepare()` 函数；
- FIX：`CCTrace::addLocation()` 中的整型除法错误；
- ADD：`-continuous-trance` 选项；
- TEST：热点选取中，位置点信息收集的调用时槽错误的修复；
- TEST：`CCTrace` 类中的轨迹文件读取操作，以及 `CHotspotSelect::CollectNewPositions()` 中的位置点信息收集操作；
- [ ] OPT：`hotspot-statistics.log` 中的输出，改为针对最终选择的热点集合输出信息，添加覆盖比例；
- [ ] FIX：`parameters.log` 中的输出；
- [ ] FIX：STL 容器的调试器可视化；



- [ ] FIX：测试 Prophet 路由中单次数据转发的最大数据包数目；
- [ ] TEST：测试函数 `HAR::receiveContents()`；
- [ ] ADD：添加传输延迟的耗时计算；
- [ ] ADD：将节点间的时间同步改用线程编程实现；
- [ ] ADD：使用网格和指针结合存储，以减少二阶遍历；
- [ ] ADD：重写 Epidemic 路由的数据通信流程；
- [ ] ADD：为 Release 版本添加尽可能多的合法性检查、异常捕获和输出；
- [ ] ADD：添加将热点选取操作独立开启，而不是必须和 HAR 或者 HDC 绑定的功能；
- [ ] RFCT：对于不确定尝试和新增的可选功能，尽量使用条件编译和宏定义包裹变量和成员函数定义；
- [ ] TRY：当缓存已满时，即使对投递概率更低的节点，也发送数据 ?
- [ ] RFCT：将 `Global.h` 和 `main.cpp` 中的全局辅助函数放入 `CRunHelper` ?
- [ ] RFCT：将配置参数相关的定义和操作整理到单独的 `CConfiguration` 类；
- [ ] RFCT：将配置参数的默认值读取改用 XML 实现；
- [ ] MNT：在 `README.md` 中，添加 trace 文件的格式和目录位置要求的说明；
- [ ] MNT：重构完成后，将代码合并到 master 分支；
- [ ] RFCT：尝试生成 dll；
