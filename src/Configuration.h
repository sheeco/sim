/***********************************************************************************************************************************

顶级类 CConfiguration ： 所有参数配置类，应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"

class CConfiguration
{
	friend class CRunHelper;

	/******************************** Config Const ********************************/

	//template <class T>
	//struct Described
	//{
	//	T value;
	//	string description;

	//	Described(T value, string description) : value(value), description(description)
	//	{
	//	};
	//	Described(T value) : value(value), description("Unknown")
	//	{
	//	};
	//};
	//typedef struct Described Described;

public:

	typedef enum EnumMacProtocolScheme
	{
		_smac,
		_hdc
	} EnumMacProtocolScheme;

	typedef enum EnumRoutingProtocolScheme
	{
		_xhar,
		_prophet,
	} EnumRoutingProtocolScheme;
	
	typedef enum EnumHotspotSelectScheme
	{
		_skip,
		_original,
		_improved,
		_merge
	} EnumHotspotSelectScheme;


	typedef enum EnumForwardScheme
	{
		_copy,  //发送数据成功后，保留自身副本
		_dump   //发送数据成功后，删除自身副本
	} EnumForwardScheme;

	typedef enum EnumRelayScheme
	{
		_loose,   //MA buffer已满时，仍允许继续接收数据
		_selfish   //MA buffer已满时，不再从其他节点接收数据
	} EnumRelayScheme;

	typedef enum EnumQueueScheme
	{
		_fifo,   //可发送配额有限时，优先从头部发送
		_lifo   //可发送配额有限时，优先从尾部发送
	} EnumQueueScheme;


	typedef struct CSimulationConfig
	{
		static int DATATIME;
		static int RUNTIME;
		static int SLOT;
	} CSimulationConfig;
	
	typedef struct CLogConfig
	{
		static int SLOT_LOG;  //记录数据投递率和数据投递时延的slot

		static string DIR_ROOT;
		static string DIR_PROJECT;
		static string DIR_RUN;
		static string DIR_RESOURCE;
		static string DIR_LOG;
		static string TIMESTAMP;
		static string PATH_TIMESTAMP;
		
		static string INFO_LOG;
		static string FILE_DEFAULT_CONFIG;
		static string FILE_PARAMETES;
		static string FILE_HELP;
		static string FILE_VERSION;
		
		static string FILE_ERROR;
		static string FILE_CONFIG;
		static string FILE_FINAL;
		static string INFO_FINAL;
		
		static string FILE_NODE;
		static string INFO_NODE;
		static string FILE_DEATH;
		static string INFO_DEATH;
		static string FILE_ENCOUNTER;
		static string INFO_ENCOUNTER;
		static string FILE_TRANSMIT;
		static string INFO_TRANSMIT;
		static string FILE_ACTIVATION;
		static string INFO_ACTIVATION;
		static string FILE_ENERGY_CONSUMPTION;
		static string INFO_ENERGY_CONSUMPTION;
		static string FILE_SINK;
		static string INFO_SINK;
		
		static string FILE_DELIVERY_RATIO_900;
		static string INFO_DELIVERY_RATIO_900;
		static string FILE_DELIVERY_RATIO_100;
		static string INFO_DELIVERY_RATIO_100;
		static string FILE_DELAY;
		static string INFO_DELAY;
		static string FILE_HOP;
		static string INFO_HOP;
		static string FILE_BUFFER;
		static string INFO_BUFFER;
		static string FILE_BUFFER_STATISTICS;
		static string INFO_BUFFER_STATISTICS;
		
		static string FILE_HOTSPOT;
		static string INFO_HOTSPOT;
		static string FILE_HOTSPOT_DETAILS;
		static string INFO_HOTSPOT_DETAILS;
		static string FILE_HOTSPOT_SIMILARITY;
		static string INFO_HOTSPOT_SIMILARITY;
		static string FILE_VISIT;
		static string INFO_VISIT;
		static string FILE_HOTSPOT_STATISTICS;
		static string INFO_HOTSPOT_STATISTICS;
		static string FILE_DELIVERY_HOTSPOT;
		static string INFO_DELIVERY_HOTSPOT;
		static string FILE_DELIVERY_STATISTICS;
		static string INFO_DELIVERY_STATISTICS;
		static string FILE_MERGE;
		static string INFO_MERGE;
		static string FILE_MERGE_DETAILS;
		static string INFO_MERGE_DETAILS;
		static string FILE_MA;
		static string INFO_MA;
		static string FILE_MA_ROUTE;
		static string INFO_MA_ROUTE;
		static string FILE_BUFFER_MA;
		static string INFO_BUFFER_MA;
		static string FILE_ED;
		static string INFO_ED;

	} CLogConfig;

	typedef struct CDataConfig
	{
		static int SIZE_DATA;  //( Byte )
		static int SIZE_CTRL;
		static int SIZE_HEADER_MAC;  //Mac Header Size

		//跳数倒计数的初始值
		static int MAX_HOP;
	} CDataConfig;

	typedef struct CTransConfig
	{
		static int SPEED_TRANS;  // Byte / s
		static int RANGE_TRANS;  //transmission range
		static double PROB_TRANS;

		static double CONSUMPTION_WAKE;
		static double CONSUMPTION_SLEEP;
		static double CONSUMPTION_BYTE_SEND;
		static double CONSUMPTION_BYTE_RECEIVE;

		static int WINDOW_TRANS;
		static double CONSTANT_TRANS_DELAY;  // disabled if negative
	} CTransConfig;

	typedef struct CMacConfig
	{
		static int CYCLE_TOTAL;
		static double DUTY_RATE;  //不使用HDC，或者HDC中不在热点区域内时的占空比
		static int CYCLE_CARRIER_SENSE;  //发送RTS之前，载波侦听的时间

		static bool SYNC_CYCLE;  //是否同步所有节点的工作状态
	} CMacConfig;

	typedef struct CHDCConfig
	{
		static double HOTSPOT_DUTY_RATE;  //HDC中热点区域内的占空比
	} CHDCConfig;

	typedef struct CSinkConfig
	{
		static double X;
		static double Y;
		static int SINK_ID;
		static int CAPACITY_BUFFER;
	} CSinkConfig;

	typedef struct CNodeConfig
	{
		static double DEFAULT_DATA_RATE;  //( Byte / s )

		static int CAPACITY_BUFFER;
		static int CAPACITY_ENERGY;
		static int LIFETIME_SPOKEN_CACHE;  //在这个时间内交换过数据的节点暂时不再交换数据
		static EnumRelayScheme SCHEME_RELAY;
		static EnumForwardScheme SCHEME_FORWARD;
		static EnumQueueScheme SCHEME_QUEUE;
	} CNodeConfig;

	typedef struct CMANodeConfig
	{
		static int SPEED;
		static int CAPACITY_BUFFER;
		static EnumRelayScheme SCHEME_RELAY;
		static int START_COUNT_ID;
	} CMANodeConfig;

	typedef struct CTraceConfig
	{
		static bool CONTINUOUS_TRACE;  //是否将从轨迹文件中得到的散点模拟成为连续的折线
		static string EXTENSION_TRACE;
		static string PATH_TRACE;
		static int SLOT_TRACE;  //移动模型中的 slot，由数据文件中得来（eg. NCSU模型中为30）；值为 0 时，表示未赋值；值为 -1 时，表示无固定的时槽；
	} CTraceConfig;

	typedef struct CHotspotSelectConfig
	{
		static int SLOT_POSITION_UPDATE;  //地理信息收集的slot
		static int SLOT_HOTSPOT_UPDATE;	 //更新热点和分类的slot
		static int STARTTIME_HOSPOT_SELECT;  //no MA node at first
		static bool TEST_HOTSPOT_SIMILARITY;

		static double ALPHA;  //ratio for post selection

	} CHotspotSelectConfig;

	typedef struct CImprovedHotspotSelectConfig
	{
		static double LAMBDA;
		static int LIFETIME_POSITION;
	} CImprovedHotspotSelectConfig;

	typedef struct CMergedHotspotSelectConfig
	{
		static double RATIO_MERGE_HOTSPOT;
		static double RATIO_NEW_HOTSPOT;
		static double RATIO_OLD_HOTSPOT;
		static bool TEST_BALANCED_RATIO;
	} CMergedHotspotSelectConfig;

	typedef struct CHARConfig
	{
		static double BETA;  //ratio for true hotspot
		static double CO_HOTSPOT_HEAT_A1;
		static double CO_HOTSPOT_HEAT_A2;

		static int MIN_WAITING_TIME;  //add minimum waiting time to each hotspot
	} CHARConfig;

	typedef struct CProphetConfig
	{
		static double INIT_PRED;
		static double RATIO_PRED_DECAY;
		static double RATIO_PRED_TRANS;
		static bool TRANS_STRICT_BY_PRED;  //指示是否严格按照投递概率真大于，来决定是否转发
	} CProphetConfig;

	// TODO: change to map<group, map<keyword, {description, type, value*}>> ?
	typedef struct CConfig
	{
		static EnumMacProtocolScheme MAC_PROTOCOL;
		static EnumRoutingProtocolScheme ROUTING_PROTOCOL;
		static EnumHotspotSelectScheme HOTSPOT_SELECT;
		
		static CSimulationConfig simulation;
		static CLogConfig log;
		static CMacConfig mac;
		static CHDCConfig hdc;
		static CDataConfig data;
		static CTransConfig trans;
		static CSinkConfig sink;
		static CNodeConfig node;
		static CMANodeConfig ma;
		static CTraceConfig trace;
		
		static CHotspotSelectConfig hs;
		static CImprovedHotspotSelectConfig ihs;
		static CMergedHotspotSelectConfig mhs;
		static CHARConfig har;
		static CProphetConfig prophet;

	} CConfig;

	//static CConfig config;


private:

	typedef enum EnumArgumentType
	{
		_none,  //该 keyword 不带任何参数时，向 values 中压入一个 config::_none 型的 _TYPE_VALUE，并将 defaultValue 的值赋给 dstAttr
		_bool,  //按照布尔型解析，关键字由 KEYWORD_TRUE, KEYWORD_FALSE 指定
		_int,  //按照整型格式解析
		_double,  //按照浮点型格式解析
		_string  //按照字符串解析
	} EnumArgumentType;


	// TODO: define all the configs here as static attribute


	typedef	struct CConfigArgument
	{
		EnumArgumentType type;
		void * dstAttr;  //解析结果将赋值到该地址指向的变量
		void * value;  //value of this configuration
		bool state;  //仅用于 _none 时，记录该选项的开/关
	} CConfigArgument;

	//string keyword;  //关键字：带参数的关键字如 "-sink"，不带参数的关键字如 "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //该关键字后附带的值域

	static map<string, pair<string, vector<CConfigArgument> > > configurations;
	static string KEYWORD_HELP;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};


	//检测该关键字是否存在
	static bool ifExists(string keyword);
	static pair<string, vector<CConfigArgument>> getConfiguration(string keyword);
	//最多允许带 2 个参数
	// TODO: remove description ?
	// TODO: remove defaultValue ?
	static void AddConfiguration(string keyword, string description, vector<CConfigArgument> fields);
	static bool AddConfiguration(string keyword, void *OnValue, bool defaultState, void *dstAttr, string description);
	static bool AddConfiguration(string keyword, EnumArgumentType type, void *defaultValue, void * dstAttr, string description);
	static bool AddConfiguration(string keyword, EnumArgumentType type_1, void *defaultValue_1, void * dstAttr_1, 
								 EnumArgumentType type_2, void *defaultValue_2, void * dstAttr_2, string description);
	

protected:

	static void UpdateConfiguration(string keyword, vector<string> argvs);

	static vector<string> ConvertToConfiguration(int argc, char * argv[]);
	static bool ParseConfiguration(vector<string> args);
	static bool ParseConfiguration(string filename);

	static void InitConfiguration();
	static void ValidateConfiguration();
	static bool ApplyConfigurations();
	static void PrintConfiguration();

	static void Help();

};

#endif // __CONFIGURATION_H__

using config = CConfiguration;

extern CConfiguration::CConfig configs;
