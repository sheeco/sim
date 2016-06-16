#include "Global.h"


namespace global
{
	_MAC_PROTOCOL MAC_PROTOCOL = _smac;
	_ROUTING_PROTOCOL ROUTING_PROTOCOL = _prophet;
	_HOTSPOT_SELECT HOTSPOT_SELECT = _none;

	int DATATIME = 0;
	int RUNTIME = 0;
	string DATASET;
	int SLOT = 0;
	int SLOT_LOG = 0;

	string TIMESTAMP;
	string PATH_TRACE = "../res/";
	string PATH_RUN = "";
	string PATH_LOG = "";  // " YY-MM-DD-HH-MM-SS/ "
	string PATH_ROOT = "../log/";

	string INFO_LOG;
	string FILE_DEFAULT_CONFIG = "default.config";
	string FILE_PARAMETES = "parameters.log";
	string FILE_HELP = "help.md";
	string FILE_VERION = "HDC.version";
	//string INFO_HELP;

	string FILE_ERROR = "error.log";
	string FILE_CONFIG = "config.log";
	string FILE_FINAL = "final.log";
	string INFO_FINAL = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#HOP/TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery%	#Delay/	#HOP/	#EnergyConsumption/	#TransmitSuccessful%	#EncounterActive%	(#EncounterAtHotspot%)	(#NetworkTime	#Node)	#Log \n" ;

	string FILE_ENCOUNTER = "encounter.log";
	string INFO_ENCOUNTER = "#Time	(#EncounterAtHotspot%	#EncounterAtHotspot)	#Encounter	 \n" ;
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
	string FILE_HOTSPOT_DETAILS = "hotspot-details.log";
	string INFO_HOTSPOT_DETAILS = "#Time 	#X	#Y \n";
	string FILE_HOTSPOT_SIMILARITY = "hotspot-similarity.log";
	string INFO_HOTSPOT_SIMILARITY = "#Time	#Overlap/Old	#Overlap/New	#OverlapArea	#OldArea	#NewArea \n";
	string FILE_VISIT = "visit.log";
	string INFO_VISIT = "#Time	#VisitAtHotspotPercent	#VisitAtHotspot	#VisitSum \n" ;
	string FILE_HOTSPOT_STATISTICS = "hotspot-statistics.log";
	string INFO_HOTSPOT_STATISTICS = "#Time	#CoverPercent	#HotspotCount	#AvgCover \n" ;
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

void global::Exit(int code)
{
	time_t seconds;  //秒时间  
	char temp_time[65];
	seconds = time(nullptr); //获取目前秒时间  
	strftime(temp_time, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));
	string finalTime(temp_time);

	//		// Remove entire folder if empty & exit directly
	//
	//		LPWSTR pathContent = CString( (PATH_ROOT + PATH_LOG + "*.*").c_str()).AllocSysString();
	//		LPWSTR pathFolder = CString( (PATH_ROOT + PATH_LOG).c_str()).AllocSysString();
	//		CFileFind tempFind;
	//		bool anyContentFound = (bool) tempFind.FindFile(pathContent);
	//		if( ! anyContentFound )
	//		{
	//			//去掉文件的系统和隐藏属性
	//			SetFileAttributes(pathFolder, FILE_ATTRIBUTE_NORMAL);
	//			remove( (PATH_ROOT + PATH_LOG).c_str() );
	//
	//			exit(code);
	//		}

	// Copy final file to father folder

	if( code == EFINISH )
	{
		ifstream finalInput(PATH_ROOT + PATH_LOG + FILE_FINAL, ios::in);
		if( finalInput.is_open()
		   && ( !finalInput.eof() ) )
		{

			ofstream copy(PATH_ROOT + FILE_FINAL, ios::app);
			char temp[310] = { '\0' };
			copy.seekp(0, ios::end);
			if( !copy.tellp() )
			{
				copy << INFO_FINAL;
			}
			finalInput.getline(temp, 300);  //skip head line
			finalInput.getline(temp, 300);
			string finalInfo(temp);
			copy << finalInfo << INFO_LOG << TAB << "@" << finalTime << endl;
			copy.close();
		}
	}

	// Print final time

	if( code >= EFINISH )
	{
		fstream final(PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app | ios::in);
		final << INFO_LOG << TAB << "@" << finalTime << endl;
		final.close();
	}

	// Remove name prefix '.' in front of log folder

	if( code == EFINISH )
	{

		if( _access(( PATH_ROOT + PATH_LOG ).c_str(), 02) == 0
		   && ( PATH_LOG.find(".") != PATH_LOG.npos ) )   //if writeable & '.' found in filename
		{
			string newPathLog = PATH_LOG.substr(1, PATH_LOG.npos);
			if( _access(( PATH_ROOT + newPathLog ).c_str(), 00) != 0 )  //if no collision
			{
				rename(( PATH_ROOT + PATH_LOG ).c_str(), ( PATH_ROOT + newPathLog ).c_str());
				PATH_LOG = newPathLog;
			}
		}

		// Unhide folder
		LPWSTR wstr = CString(( PATH_ROOT + PATH_LOG ).c_str()).AllocSysString();
		int attr = GetFileAttributes(wstr);
		if( ( attr & FILE_ATTRIBUTE_HIDDEN ) == FILE_ATTRIBUTE_HIDDEN )
		{
			SetFileAttributes(wstr, attr & ~FILE_ATTRIBUTE_HIDDEN);
		}
	}

	exit(code);
}

void global::Exit(int code, string error)
{
	ofstream errorFile(PATH_ROOT + PATH_LOG + FILE_ERROR, ios::app);
	errorFile << error << endl << endl;
	errorFile.close();

	Exit(code);
}
