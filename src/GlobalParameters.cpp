#include "GlobalParameters.h"


namespace global
{
	_MAC_PROTOCOL MAC_PROTOCOL = _smac;
	_ROUTING_PROTOCOL ROUTING_PROTOCOL = _prophet;
	_HOTSPOT_SELECT HOTSPOT_SELECT = _original;

	int DATATIME = 0;
	int RUNTIME = 0;
	string DATASET;
	int SLOT = 0;
	int SLOT_MOBILITYMODEL = 0;
	int SLOT_LOG = 0;

	string TIMESTAMP;
	string PATH_TRACE = "../res/";
	string PATH_LOG = "";  // " YY-MM-DD-HH-MM-SS/ "
	string PATH_ROOT = "../log/";

	string INFO_LOG;
	string FILE_PARAMETES = "parameters.log";
	string FILE_HELP = "help.txt";
	string INFO_HELP = 
		"\n                     !!!!!! ALL CASE SENSITIVE !!!!!! \n "
		
		"\n * 必选参数 \n "

		"\n <run>                       < 运行参数 > \n\n "

		" -time-run             []    网络运行时间（默认 15000）； \n "
		" -time-data            []    节点数据生成的截止时间（默认 15000）； \n "
		" -slot                 []    网络运行的时槽（默认 5）； \n "
		" -dataset              []    数据集名称（节点轨迹文件路径 ./res/%DATASET%/）； \n "
		" -log-path             []    日志文件路径（默认 ./log/）； \n "
		" -log-slot             []    日志输出时槽（默认 100）； \n "

		"\n <node>                      < 节点参数 > \n\n "

		" -sink               [][]    Sink 节点的坐标； \n "
		" -trans-prob           []    节点数据传输的成功率（默认 1.0）； \n "
		" -trans-range          []    节点数据传输半径（默认 100）； \n "
		" -node                 []    节点个数（默认 29）； \n "
		" -buffer               []    节点缓存容量（默认 500）； \n "
		" -energy               []    节点初始能量值（默认无限制）； \n "

		"\n <data>                      < 数据生成参数 > \n\n "

		" -data-rate            []    节点生成一个数据包需要的时间（默认 30）； \n "
		" -data-size            []    单个数据包的字节数（默认 250）； \n "

		"\n <mac>                       < MAC 层参数 > \n\n "

		" -hdc                        采用 HDC 的占空比协议； \n "
		" -cycle                []    工作周期（默认 300）； \n "
		" -slot-discover        []    每个工作周期中，开始邻居节点发现之前的监听时间（默认 10）； \n "
		" -dc-default           []    默认占空比，即监听时间占整个工作周期的比例（默认 1.0）； \n "
		" -dc-hotspot           []    *HDC 中热点区域使用的占空比； \n "
		" -random-state               采用随机的节点初始工作状态，即异步的占空比； \n "

		"\n <route>                     < 路由层参数 >  \n\n "

		" -epidemic                   采用 Epidemic 路由协议； \n "
		" -prophet                    采用 Prophet 路由协议（默认采用）； \n "
		" -har                        采用 HAR 路由协议； \n "
		" -hop                  []    数据包转发允许的最大跳数（默认无限制）； \n "
		" -ttl                  []    数据包的最大生存期（默认无限制）； \n "

		"\n <prophet>                   < Prophet 协议参数 > \n\n "

		" -spoken               []    Prophet 路由中节点对最近邻居节点直接跳过通信的计时（默认不使用）； \n "
		" -queue                []    Prophet 路由中节点允许存储的其他节点数据包的最大数目（默认无限制）； \n "
		" -pred-init            []    Prophet 路由中节点投递概率的初始值（默认 0.75）； \n "
		" -pred-decay           []    Prophet 路由中节点投递概率的衰减系数（默认 0.98）； \n "
		" -pred-tolerance       []    Prophet 路由中节点决定转发数据时的投递概率容差（默认不使用）； \n "

		"\n <hotspot>                   < 热点选取参数 > \n\n "

		" -ihs                        采用 IHAR 的热点选取算法； \n "
		" -mhs                        采用 mHAR 的热点选取算反； \n "
		" -alpha                []    热点选取中，后续选取过程中的阈值参数（默认 0.03）； \n "
		" -beta                 []    热点选取中，判定真热点的最低热度值（默认 0.0025）； \n "
		" -heat               [][]    热点选取中，热度值计算的系数（默认 1， 30）； \n "

		"\n <ihar>                      < IHAR 参数> \n\n "

		" -lambda               []    IHAR 中，疏漏热点修复算法的修复阈值（默认不使用）； \n "
		" -lifetime             []    IHAR 中，节点位置点信息的保存期（默认不使用）； \n "

		"\n <mhar>                      < mHAR 参数 > \n\n "

		" -merge                []    mHAR 中，归并热点的选取系数（默认 1.0）； \n "
		" -old                  []    mHAR 中，旧热点的选取系数（默认 1.0）； \n "
		" -balanced-ratio             采用综合的热点选取度量函数； \n " 

		"\n <test>                      < 测试参数 > \n\n "

		" -dynamic-node-number        节点个数动态变化的测试； \n "
		" -hotspot-similarity         对于热点选取，测试前后热点选取结果的相似度（默认采用）； \n " ;


	string FILE_ERROR = "error.log";
	string FILE_FINAL = "final.log";
	string INFO_FINAL = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#HOP/TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery%	#Delay/	#HOP/	#EnergyConsumption/	#TransmitSuccessful%	#EncounterActive%	(#EncounterAtHotspot%)	(#NetworkTime	#Node)	#Log \n" ;

	string FILE_ENCOUNTER = "encounter.log";
	string INFO_ENCOUNTER = "#Time	(#EncounterActiveAtHotspot%	#EncounterActiveAtHotspot	#EncounterAtHotspot%	#EncounterAtHotspot)	#EncounterActive%	#EncounterActive	#Encounter	 \n" ;
	string FILE_TRANSMIT = "transmit.log";
	string INFO_TRANSMIT = "#Time	(#TransmitSuccessful%	#TransmitSuccessful	#Transmit \n" ;
	string FILE_ENERGY_CONSUMPTION = "energy-consumption.log";
	string INFO_ENERGY_CONSUMPTION = "#Time	#AvgEC	(#SumEC	#NodeCount	#CurrentEnergy...) \n" ;
	string FILE_SINK = "sink.log";
	string INFO_SINK = "#Time	#EncounterAtSink \n";  // TODO: unprinted

	string FILE_DELIVERY_RATIO_900 = "delivery-ratio.log";
	string INFO_DELIVERY_RATIO_900 = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n" ;
	string FILE_DELIVERY_RATIO_100 = "delivery-ratio-100.log";
	string INFO_DELIVERY_RATIO_100 = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n" ;
	string FILE_DELAY = "delay.log";
	string INFO_DELAY	 = "#Time	#AvgDelay \n" ;
	string FILE_HOP = "hop.log";
	string INFO_HOP	 = "#Time	#AvgHOP \n" ;
	string FILE_BUFFER = "buffer.log";
	string INFO_BUFFER = "#Time	#BufferStateOfEachNode \n" ;
	// TODO: all 0 in output
	string FILE_BUFFER_STATISTICS = "buffer-statistics.log";
	string INFO_BUFFER_STATISTICS =  "#Time	#AvgBufferStateInHistoryOfEachNode \n" ;

	string FILE_HOTSPOT = "hotspot.log";
	string INFO_HOTSPOT = "#Time	#HotspotCount \n" ;
	string FILE_HOTSPOT_SIMILARITY = "hotspot-similarity.log";
	string INFO_HOTSPOT_SIMILARITY = "#Time	#Overlap/Old	#Overlap/New	#OverlapArea	#OldArea	#NewArea \n";
	string FILE_AT_HOTSPOT = "at-hotspot.log";
	string INFO_AT_HOTSPOT = "#Time	#VisitAtHotspot	#VisitSum	#VisitAtHotspotPercent \n" ;
	string FILE_HOTSPOT_STATISTICS = "hotspot-statistics.log";
	string INFO_HOTSPOT_STATISTICS = "#Time	#CoverSum	#HotspotCount	#AvgCover \n" ;
	string FILE_HOTSPOT_RANK = "hotspot-rank.log";
	string INFO_HOTSPOT_RANK =  "#WorkTime	#ID	#Location	#nPosition, nNode	#Ratio	#Tw	#DeliveryCount \n" ;
	string FILE_DELIVERY_HOTSPOT = "delivery-hotspot.log";
	string INFO_DELIVERY_HOTSPOT = "#Time	#DeliveryCountForSingleHotspotInThisSlot ... \n";
	string FILE_DELIVERY_STATISTICS = "delivery-statistics.log";
	string INFO_DELIVERY_STATISTICS = "#Time	#DeliveryAtHotspotCount	#DeliveryTotalCount	#DeliveryAtHotspotPercent \n";
	string FILE_MERGE = "merge.log";
	string INFO_MERGE = "#Time	#MergeHotspotCount	#MergeHotspotPercent	#OldHotspotCount	#OldHotspotPercent	#NewHotspotCount	#NewHotspotPercent \n" ;
	string FILE_MERGE_DETAILS = "merge-details.log";
	string INFO_MERGE_DETAILS = "#Time	#HotspotType/#MergeAge ... \n" ;
	string FILE_MA = "ma.log";
	string INFO_MA = "#Time	#MACount	#AvgMAWayPointCount \n" ;
	string FILE_BUFFER_MA = "buffer-ma.log";
	string INFO_BUFFER_MA = "#Time	#BufferStateOfEachMA \n" ;
	string FILE_ED = "ed.log";
	string INFO_ED = "#Time	#EstimatedDelay \n" ;

}
