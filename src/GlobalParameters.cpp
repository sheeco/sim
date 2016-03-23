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
		
		"\n * ��ѡ���� \n "

		"\n <run>                       < ���в��� > \n\n "

		" -time-run             []    ��������ʱ�䣨Ĭ�� 15000���� \n "
		" -time-data            []    �ڵ��������ɵĽ�ֹʱ�䣨Ĭ�� 15000���� \n "
		" -slot                 []    �������е�ʱ�ۣ�Ĭ�� 5���� \n "
		" -dataset              []    ���ݼ����ƣ��ڵ�켣�ļ�·�� ./res/%DATASET%/���� \n "
		" -log-path             []    ��־�ļ�·����Ĭ�� ./log/���� \n "
		" -log-slot             []    ��־���ʱ�ۣ�Ĭ�� 100���� \n "

		"\n <node>                      < �ڵ���� > \n\n "

		" -sink               [][]    Sink �ڵ�����ꣻ \n "
		" -trans-prob           []    �ڵ����ݴ���ĳɹ��ʣ�Ĭ�� 1.0���� \n "
		" -trans-range          []    �ڵ����ݴ���뾶��Ĭ�� 100���� \n "
		" -buffer               []    �ڵ㻺��������Ĭ�� 500���� \n "
		" -energy               []    �ڵ��ʼ����ֵ��Ĭ�������ƣ��� \n "

		"\n <data>                      < �������ɲ��� > \n\n "

		" -data-rate            []    �ڵ�����һ�����ݰ���Ҫ��ʱ�䣨Ĭ�� 30���� \n "
		" -data-size            []    �������ݰ����ֽ�����Ĭ�� 250���� \n "

		"\n <mac>                       < MAC ����� > \n\n "

		" -hdc                        ���� HDC ��ռ�ձ�Э�飻 \n "
		" -cycle                []    �������ڣ�Ĭ�� 300���� \n "
		" -cycle-discover             []    ÿ�����������У���ʼ�ھӽڵ㷢��֮ǰ�ļ���ʱ�䣨Ĭ�� 10���� \n "
		" -dc-default           []    Ĭ��ռ�ձȣ�������ʱ��ռ�����������ڵı�����Ĭ�� 1.0���� \n "
		" -dc-hotspot           []    *HDC ���ȵ�����ʹ�õ�ռ�ձȣ� \n "
		" -random-state               ��������Ľڵ��ʼ����״̬�����첽��ռ�ձȣ� \n "

		"\n <route>                     < ·�ɲ���� >  \n\n "

		" -epidemic                   ���� Epidemic ·��Э�飻 \n "
		" -prophet                    ���� Prophet ·��Э�飨Ĭ�ϲ��ã��� \n "
		" -har                        ���� HAR ·��Э�飻 \n "
		" -hop                  []    ���ݰ�ת����������������Ĭ�������ƣ��� \n "
		" -ttl                  []    ���ݰ�����������ڣ�Ĭ�������ƣ��� \n "

		"\n <prophet>                   < Prophet Э����� > \n\n "

		" -spoken               []    Prophet ·���нڵ������ھӽڵ�ֱ������ͨ�ŵļ�ʱ��Ĭ�ϲ�ʹ�ã��� \n "
		" -queue                []    Prophet ·���нڵ�����洢�������ڵ����ݰ��������Ŀ��Ĭ�������ƣ��� \n "
		" -pred-tolerance       []    Prophet ·���нڵ����ת������ʱ��Ͷ�ݸ����ݲĬ�ϲ�ʹ�ã��� \n "

		"\n <hotspot>                   < �ȵ�ѡȡ���� > \n\n "

		" -ihs                        ���� IHAR ���ȵ�ѡȡ�㷨�� \n "
		" -mhs                        ���� mHAR ���ȵ�ѡȡ�㷴�� \n "
		" -alpha                []    �ȵ�ѡȡ�У�����ѡȡ�����е���ֵ������Ĭ�� 0.03���� \n "
		" -beta                 []    �ȵ�ѡȡ�У��ж����ȵ������ȶ�ֵ��Ĭ�� 0.0025���� \n "
		" -heat               [][]    �ȵ�ѡȡ�У��ȶ�ֵ�����ϵ����Ĭ�� 1�� 30���� \n "

		"\n <ihar>                      < IHAR ����> \n\n "

		" -lambda               []    IHAR �У���©�ȵ��޸��㷨���޸���ֵ��Ĭ�ϲ�ʹ�ã��� \n "
		" -lifetime             []    IHAR �У��ڵ�λ�õ���Ϣ�ı����ڣ�Ĭ�ϲ�ʹ�ã��� \n "

		"\n <mhar>                      < mHAR ���� > \n\n "

		" -merge                []    mHAR �У��鲢�ȵ��ѡȡϵ����Ĭ�� 1.0���� \n "
		" -old                  []    mHAR �У����ȵ��ѡȡϵ����Ĭ�� 1.0���� \n "
		" -balanced-ratio             �����ۺϵ��ȵ�ѡȡ���������� \n " 

		"\n <test>                      < ���Բ��� > \n\n "

		" -dynamic-node-number        �ڵ������̬�仯�Ĳ��ԣ� \n "
		" -hotspot-similarity         �����ȵ�ѡȡ������ǰ���ȵ�ѡȡ��������ƶȣ�Ĭ�ϲ��ã��� \n " ;


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
