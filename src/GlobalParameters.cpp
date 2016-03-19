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
	int SLOT_RECORD_INFO = 0;

	string TIMESTAMP;
	string PATH_TRACE = "../res/";
	string PATH_LOG = "../test/";
	string PATH_FINAL = "../test/";

	string INFO_LOG;
	string FILE_PARAMETES = "parameters.log";
	string FILE_HELP = "help.txt";
	string INFO_HELP = 
		"\n                                 !!!!!! ALL CASE SENSITIVE !!!!!! \n"
		" <run>       -time-data        []   -time-run        []   -slot        []   -dataset     [] \n"
		" <node>      -sink           [][]   -range           []   -prob-trans  []   -energy      []   \n"
		" <data>      -buffer           []   -data-rate       []   -data-size   []   -dataset     []   \n"
		" <mac>       -hdc                   -cycle           []   -dc-default  []   -dc-hotspot  []   \n"
		" <route>     -epidemic              -prophet              -har              -hop         []   -ttl         []   \n"
		" <prophet>   -spoken           []   -queue           []   \n"
		" <hs>        -ihs                   -mhs                  -alpha       []   -beta        []   -heat      [][] \n"
		" <ihar>      -lambda           []   -lifetime        []   \n"
		" <mhar>      -merge            []   -old             []   \n"
		" <test>      -dynamic-node-number   -hotspot-similarity   -balanced-ratio \n\n" ;

	string FILE_FINAL = "final.log";
	string INFO_FINAL = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery%	#Delay	#EnergyConsumption/	#TransmitSuccessful%	#EncounterActive%	(#EncounterAtHotspot%)	(#NetworkTime #Node)	#Log \n" ;

	string FILE_ENCOUNTER = "encounter.log";
	string INFO_ENCOUNTER = "#Time	(#EncounterAtHotspot%	#EncounterAtHotspot	#EncounterActive%	#EncounterActive)	#Encounter	 \n" ;
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
