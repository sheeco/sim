/***********************************************************************************************************************************

������ CConfiguration �� ���в��������࣬Ӧ�ü̳��������

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
		_copy,  //�������ݳɹ��󣬱���������
		_dump   //�������ݳɹ���ɾ��������
	} EnumForwardScheme;

	typedef enum EnumRelayScheme
	{
		_loose,   //MA buffer����ʱ�������������������
		_selfish   //MA buffer����ʱ�����ٴ������ڵ��������
	} EnumRelayScheme;

	typedef enum EnumQueueScheme
	{
		_fifo,   //�ɷ����������ʱ�����ȴ�ͷ������
		_lifo   //�ɷ����������ʱ�����ȴ�β������
	} EnumQueueScheme;


	typedef struct CSimulationConfig
	{
		static int DATATIME;
		static int RUNTIME;
		static int SLOT;
	} CSimulationConfig;
	
	typedef struct CLogConfig
	{
		static int SLOT_LOG;  //��¼����Ͷ���ʺ�����Ͷ��ʱ�ӵ�slot

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

		//�����������ĳ�ʼֵ
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
		static double DUTY_RATE;  //��ʹ��HDC������HDC�в����ȵ�������ʱ��ռ�ձ�
		static int CYCLE_CARRIER_SENSE;  //����RTS֮ǰ���ز�������ʱ��

		static bool SYNC_CYCLE;  //�Ƿ�ͬ�����нڵ�Ĺ���״̬
	} CMacConfig;

	typedef struct CHDCConfig
	{
		static double HOTSPOT_DUTY_RATE;  //HDC���ȵ������ڵ�ռ�ձ�
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
		static int LIFETIME_SPOKEN_CACHE;  //�����ʱ���ڽ��������ݵĽڵ���ʱ���ٽ�������
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
		static bool CONTINUOUS_TRACE;  //�Ƿ񽫴ӹ켣�ļ��еõ���ɢ��ģ���Ϊ����������
		static string EXTENSION_TRACE;
		static string PATH_TRACE;
		static int SLOT_TRACE;  //�ƶ�ģ���е� slot���������ļ��е�����eg. NCSUģ����Ϊ30����ֵΪ 0 ʱ����ʾδ��ֵ��ֵΪ -1 ʱ����ʾ�޹̶���ʱ�ۣ�
	} CTraceConfig;

	typedef struct CHotspotSelectConfig
	{
		static int SLOT_POSITION_UPDATE;  //������Ϣ�ռ���slot
		static int SLOT_HOTSPOT_UPDATE;	 //�����ȵ�ͷ����slot
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
		static bool TRANS_STRICT_BY_PRED;  //ָʾ�Ƿ��ϸ���Ͷ�ݸ�������ڣ��������Ƿ�ת��
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
		_none,  //�� keyword �����κβ���ʱ���� values ��ѹ��һ�� config::_none �͵� _TYPE_VALUE������ defaultValue ��ֵ���� dstAttr
		_bool,  //���ղ����ͽ������ؼ����� KEYWORD_TRUE, KEYWORD_FALSE ָ��
		_int,  //�������͸�ʽ����
		_double,  //���ո����͸�ʽ����
		_string  //�����ַ�������
	} EnumArgumentType;


	// TODO: define all the configs here as static attribute


	typedef	struct CConfigArgument
	{
		EnumArgumentType type;
		void * dstAttr;  //�����������ֵ���õ�ַָ��ı���
		void * value;  //value of this configuration
		bool state;  //������ _none ʱ����¼��ѡ��Ŀ�/��
	} CConfigArgument;

	//string keyword;  //�ؼ��֣��������Ĺؼ����� "-sink"�����������Ĺؼ����� "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //�ùؼ��ֺ󸽴���ֵ��

	static map<string, pair<string, vector<CConfigArgument> > > configurations;
	static string KEYWORD_HELP;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};


	//���ùؼ����Ƿ����
	static bool ifExists(string keyword);
	static pair<string, vector<CConfigArgument>> getConfiguration(string keyword);
	//�������� 2 ������
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
