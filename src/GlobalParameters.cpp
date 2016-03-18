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

	string INFO_LOG;
	string INFO_HELP = "\n                                 !!!!!! ALL CASE SENSITIVE !!!!!! \n"
					   "<node>      -sink           [][]  -range           []   -prob-trans  []   -energy      []   -time-data   []   -time-run    [] \n"
					   "<data>      -buffer           []  -data-rate       []   -data-size   []  \n"
					   "<mac>       -hdc                  -cycle           []   -dc-default  []   -dc-hotspot  [] \n"
					   "<route>     -epidemic             -prophet              -har              -hop         []   -ttl         []\n"
					   "<prophet>   -spoken           []  -queue           [] \n"
					   "<hs>        -hs                   -ihs                  -mhs              -alpha       []   -beta        []   -heat      [][] \n"
					   "<ihar>      -lambda           []  -lifetime        [] \n"
					   "<mhar>      -merge            []  -old             [] \n"
					   "<test>      -dynamic-node-number  -hotspot-similarity   -balanced-ratio \n\n" ;

	string FILE_DEBUG = "debug.txt";
	string INFO_DEBUG = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery%	#Delay	#EnergyConsumption/	#TransmitSuccessful%	#EncounterActive%	(#EncounterAtHotspot%)	(#NetworkTime #Node)	#Log \n" ;

	string INFO_ENCOUNTER = "#Time	(#EncounterAtHotspot%	#EncounterAtHotspot	#EncounterActive%	#EncounterActive)	#Encounter	 \n" ;
	string INFO_TRANSMIT = "#Time	(#TransmitSuccessful%	#TransmitSuccessful	#Transmit \n" ;
	string INFO_ENERGY_CONSUMPTION = "#Time	#AvgEC	(#SumEC	#NodeCount	#CurrentEnergy...) \n" ;
	string INFO_SINK = "#Time	#EncounterAtSink \n";  // TODO: unprinted

	string INFO_DELIVERY_RATIO = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n" ;
	string INFO_DELAY = "#Time	#AvgDelay \n" ;
	string INFO_BUFFER = "#Time	#BufferStateOfEachNode \n" ;
	string INFO_BUFFER_STATISTICS =  "#Time	#AvgBufferStateInHistoryOfEachNode \n" ;

	string INFO_HOTSPOT = "#Time	#HotspotCount \n" ;
	string INFO_AT_HOTSPOT = "#Time	#VisitAtHotspot	#VisitSum	#VisitAtHotspotPercent \n" ;
	string INFO_HOTSPOT_STATISTICS = "#Time	#CoverSum	#HotspotCount	#AvgCover \n" ;
	string INFO_MERGE = "#Time	#MergeHotspotCount	#MergeHotspotPercent	#OldHotspotCount	#OldHotspotPercent	#NewHotspotCount	#NewHotspotPercent \n" ;
	string INFO_MERGE_DETAILS = "#Time	#HotspotType/#MergeAge ... \n" ;
	string INFO_MA = "#Time	#MACount	#AvgMAWayPointCount \n" ;
	string INFO_BUFFER_MA = "#Time	#BufferStateOfEachMA \n" ;
	string INFO_ED = "#Time	#EstimatedDelay \n" ;

}
