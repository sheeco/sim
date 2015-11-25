#### Created by Eco Sun on 2015-11-03


### debug.txt

#Alpha            #Beta     #MinWaitingTime    ( #Lambda             #Lifetime )       ( #Merge        #Old              #Heat )

#DeliveryRatio    #Delay    #HotspotCostAvg    ( #MergePercentAvg    #OldPercentAvg )    #MACostAvg    #SimilarityAvg    #AtHotspotPercent



### 参数格式

                                                  !!!!!! ALL CASE SENSITIVE !!!!!!

<mode>            -har;                -ihar;                -mhar;               -hotspot-similarity;        -dynamic-node-number;        -balanced-ratio;
<time>            -time-data [];       -time-run [];
<parameter>       -alpha     [];       -beta     [];         -gama     [];        -heat [] [];                -prob-trans [];
<ihar>            -lambda    [];       -lifetime [];
<mhar>            -merge     [];       -old      [];         -min-wait [];        -heat-exp;                  -heat-ln;



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

README.txt

GlobalParameters.h

main.cpp

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

GreedySelection.h & .cpp

PostSelector.h & .cpp

HAR.h & .cpp

GASolution.h & .cpp

GA.h & .cpp

NodeRepair.h & .cpp



### 输出文件说明



### 关键类外部接口



### 此前的重要更改：

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



## 2015-11-13

加入最小等待时间(MIN_WAITING_TIME)，默认为0即不使用该修改，可以从命令行-min-wait赋值；

delivery-hotspot.txt中的信息改为降序排序之后再输出，便于分析热点投递计数的集中程度，减少数据后期处理的工作量；


## 2015-11-16

规范化命令行参数输入的关键字，将动态节点数的选项也加入命令行参数；


## 2015-11-20

将node的ID改为自动生成，并随之修改节点数动态变化时的node是否存在的判定，添加函数 HAR::ifNodeExists(int id) 代替 id >= m_nodes.size() 的判定方法（暂未测试）；


# 2015-11-24

动态节点个数测试时，在下一次热点选取之前(HAR::ChangeNodeNumber())删除被抛弃节点的位置点信息（注意：节点个数变化周期应该为热点选取周期的倍数）；
将所有的g_系列全局变量都改为相关类内的静态变量（GA等未采用的方法相关的变量未改动）；
增加CHotspot::ratio及相关方法和改动，用于测试新的ratio计算方法，将在贪婪选取和后续选取过程中用到，可使用命令行参数-balanced-ratio选项开启；
增加poor-hotspot.txt用于测试投递计数为0的热点信息；
*应该将动态节点个数测试时的增删节点过程改成随机选取节点；
*针对热点的排序应该改成从大到小的；


# 2015-11-25

对mergesort对应的comparison类的名字进行规范化；
按照投递计数降序输出热点cover的position数、node数和ratio到hotspot-rank.txt，用于统计和分析热点质量；
按照任期统计等待时间的累积值，同样输出到hotspot-rank.txt文件；
*整理没有用到的函数定义，包括GA算法相关的全局变量；
