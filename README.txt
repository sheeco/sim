#### Created by Eco Sun on 2015-11-03


###################################################################################################################################################

#### HDC 分支的初始状态，创建自 master 分支 2015-12-04 的 commit { 0dd0e561bf88035d1cfc557e2083dfa602ab9d43 }

===================================================================================================================================================



### debug.txt

#Alpha            #Beta     #MinWaitingTime    ( #Lambda             #Lifetime )       ( #Merge        #Old              #Heat )

#DeliveryRatio    #Delay    #HotspotCostAvg    ( #MergePercentAvg    #OldPercentAvg )    #MACostAvg    #SimilarityAvg    #AtHotspotPercent    #Scheme



### 参数格式

                                        !!!!!! ALL CASE SENSITIVE !!!!!!
<mode>            -har;                  -ihar;                  -hdc;                    -hotspot-similarity;         -dynamic-node-number;
<time>            -time-data   [];       -time-run   [];
<energy>          -node-energy [];"
<har>             -alpha       [];       -beta       [];         -gama       [];          -heat   [] [];               -prob-trans [];
<ihar>            -lambda      [];       -lifetime   [];
<epidemic>        -hop         [];       -ttl        [];         -queue      [];          -spoken [];
<hdc>             -slot-total  [];       -default-dc [];         -hotspot-dc []; 



### 参数默认值

scheme                        HAR
hotspot_similarity            true
dynamic_node-number           false
min_wait                      0

alpha                         0.03
beta                          0.0025
gama                          0.5
heat                          1, 30
prob_trans                    1.0

(IHAR)
lambda                        0
lifetime                      3600

(mHAR)
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

============================

README.txt

GlobalParameters.h

main.cpp

============================

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

============================

GreedySelection.h & .cpp

PostSelector.h & .cpp

HAR.h & .cpp

============================

NodeRepair.h & .cpp



### 输出文件说明



### 关键类外部接口



### 此前的重要更改    [ 2.0.0 ]

修正旧版本的热点选取、后续选取中的错误（int除法等）；

所有调试用参数均改为命令行传入，具体参数格式可-help查看（但是没有实现足够的合法性检查，因此需要在输入时注意）；

归并HAR(mHAR)的引入，参数包含-merge, -old, -heat(-exp/-ln)，均由命令行传入；

为mHAR中的热度函数添加了指数增长、对数增长的选项（测试结果显示没有影响，原因尚未进行分析，之后的测试都应暂时使用flat选项）；

加入相邻两轮热点选取结果的相似度比较功能，从命令行传入-hotspot-similarity即可；

在所有的输出文件前打印时间戳和具体的scheme用于识别每一次运行，打印说明信息用于识别每一列信息，并统一内容和文件名的格式；

改正数据投递率计算的错误（当前值 instead of 增量）；

将调试用的重要参数格式化保存在debug.txt中，调试时直接复制到excel文档即可；

将数据投递分类为(a)MA在热点上得到投递和(b)MA在路径上得到投递，并统计两种投递计数的百分比；

并对每一个热点统计其投递计数，按照热点选取时槽输出，并伴随着atHotspot赋NULL时机的优化；

mHAR中由于所有选取ratio指数增长，在贪婪选取中可能出现position集合还没有得到全覆盖，但hotspotsAboveAverage为空的情况，此时直接选中unselectedHotspots中cover数最大的元素即可；



## 2015-11-13    [ 2.0.1 ]

加入最小等待时间(MIN_WAITING_TIME)，默认为0即不使用该修改，可以从命令行-min-wait赋值；

delivery-hotspot.txt中的信息改为降序排序之后再输出，便于分析热点投递计数的集中程度，减少数据后期处理的工作量；


## 2015-11-16    [ 2.0.2 ]

规范化命令行参数输入的关键字，将动态节点数的选项也加入命令行参数；


## 2015-11-20    [ 2.0.3 ]

将node的ID改为自动生成，并随之修改节点数动态变化时的node是否存在的判定，添加函数 HAR::ifNodeExists(int id) 代替 id >= m_nodes.size() 的判定方法（暂未测试）；


# 2015-11-24    [ 2.1.0 ]

动态节点个数测试时，在下一次热点选取之前(HAR::ChangeNodeNumber())删除被抛弃节点的位置点信息（注意：节点个数变化周期应该为热点选取周期的倍数）；
将所有的g_系列全局变量都改为相关类内的静态变量（GA等未采用的方法相关的变量未改动）；
增加CHotspot::ratio及相关方法和改动，用于测试新的ratio计算方法，将在贪婪选取和后续选取过程中用到，可使用命令行参数-balanced-ratio选项开启；
增加poor-hotspot.txt用于测试投递计数为0的热点信息；
*应该将动态节点个数测试时的增删节点过程改成随机选取节点；
*针对热点的排序应该改成从大到小的；


# 2015-11-25    [ 2.1.1 ]

对mergesort对应的comparison类的名字进行规范化；
按照投递计数降序输出热点cover的position数、node数和ratio到hotspot-rank.txt，用于统计和分析热点质量；
按照任期统计等待时间的累积值，同样输出到hotspot-rank.txt文件；


# 2015-11-27    [ 2.1.2 ]

为每个position定于初始值为1的权值，增加CPreprocessor::DecayPositionsWithoutDeliveryCount，按照投递计数为0的热点对其position执行衰减，可使用命令行参数-learn选项开启，-decay设置指数型衰减系数；
由于衰减后的position整体权值水平持续降低，后续选取过程中的固定alpha值将导致选取出的热点总数持续上升，在测试阶段暂时使用-max-hotspot来暂时稳定热点总数；
增加MIN_POSITION_WEIGHT，权值衰减之后如果低于此值将被直接删除，使用命令行参数-min-weight赋值（默认值为1即不删除）（未测试）；
修复CHotspot::calculateRatio中的int除法问题；
修复CHotspot::waitingTimes忘记统计的问题；


# 2015-12-04    [ 2.2.0 ]

对整体的类结构做出了调整，类的继承结构规范化；



===================================================================================================================================================

#### HDC 分支的初始状态

###################################################################################################################################################



# 2015-12-04    [2.2.1]

更改工程名及工作路径名为HDC；
整理删除未使用的函数定义，包括GA算法相关的全局变量和处理函数；


# 2015-12-09

修改CData和CNode类以适应Epidemic路由，其中HOP或TTL的选项通过赋给相应的最大值标明，在CData类的各种操作中可以自动判断；


# 2015-12-10

继续修改CData和CNode类，CNode类中原来是针对全部Node统一统计能耗的，现在改为对于每个节点单独统计能耗，全局的Node能耗使用SUM_ENERGY_CONSUMPTION统计，但尚未添加相关操作；
在原来的能耗统计相关宏中添加SIZE_DATA/CONTROL，用于指示单个数据包和控制包的大小；
增加CHDC类、Epidemic类和CMacProtocol基类，原HAR类中有关热点选择的函数暂时放入CHDC类，有关路由的类暂时放入Epidemic类，其成员属性和函数仍待修改；


# 2015-12-11

用CNode::state标识和计算节点的工作状态，取值范围在[ - SLOT_TOTAL, + SLOT_LISTEN )之间，值大于等于0即代表Listen状态；
按照通信成功率，多次往返通信时一旦有一次通信失败，之后的通信都不再成立；
节点的通信成功率和能量损耗，都已内置在CNode类内所有的数据收发操作中；
传感器节点的能量损耗来源包括：监听能耗、休眠能耗、数据发送能耗、数据接收能耗；
添加节点的总能量属性，以及相应的节点失效、网络运行终止的判定，默认为0即无限能量，可以通过命令行参数-node-energy赋值；
对于HDC中的热点选取操作：暂时只提供HAR和IHAR可选，参数可通过命令行参数修改；
基本修改完毕，各类参数待测试：Epidemic路由的相关参数、能耗参数、数据生成速率及大小参数、时隙及占空比参数；
MAC协议的无关内容：网络初始假定所有节点时隙同步、忽略Listen时隙内的Sync时隙即全部视作Data时隙、忽略传输延迟（之后也可以加入计算）；
