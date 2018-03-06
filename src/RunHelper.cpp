#include "RunHelper.h"
#include "Configuration.h"
#include "Prophet.h"
#include "HAR.h"
#include "HDC.h"
#include "PFerry.h"
#include "PrintHelper.h"
#include "ParseHelper.h"


void CRunHelper::InitLogPath()
{
	// Create root path (../test/) if doesn't exist
	if( access(getConfig<string>("log", "dir_log").c_str(), 00) != 0 )
		_mkdir(getConfig<string>("log", "dir_log").c_str());

	// Generate timestamp & output path
	time_t seconds;  //��ʱ��  
	char temp[65] = { '\0' };
	seconds = time(nullptr); //��ȡĿǰ��ʱ��  
	strftime(temp, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));
	CConfiguration::updateConfiguration<string>("log", "timestamp", string(temp), true);
	strftime(temp, 64, "%Y-%m-%d-%H-%M-%S", localtime(&seconds));
	string timestring;
	timestring = string(temp);
	CConfiguration::updateConfiguration<string>("log", "info_log", string("@" + getConfig<string>("log", "timestamp") + TAB), true);
	CConfiguration::updateConfiguration<string>("log", "path_timestamp", string("." + timestring + "/"), true);

	// Create log path
	if( access(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str(), 00) != 0 )
		_mkdir(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str());

	// Hide folder
	LPWSTR wstr = CString(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str()).AllocSysString();
	int attr = GetFileAttributes(wstr);
	if( ( attr & FILE_ATTRIBUTE_HIDDEN ) == 0 )
	{
		SetFileAttributes(wstr, attr | FILE_ATTRIBUTE_HIDDEN);
	}

}

bool CRunHelper::PrepareSimulation(int argc, char* argv[])
{
	CConfiguration::InitConfiguration();

	InitLogPath();

	/*********************************************  ���������ʽ������������  *********************************************/

	CConfiguration::ParseConfiguration(getConfig<string>("log", "dir_run") + getConfig<string>("log", "file_default_config"));

	vector<string> args = CConfiguration::ConvertToConfiguration(argc - 1, ( argv + 1 ));
	CConfiguration::ParseConfiguration(args, "Command Line Args");

	CConfiguration::ValidateConfiguration();


	CConfiguration::PrintConfiguration();

	CPrintHelper::PrintNewLine();
	return true;
}

bool CRunHelper::RunSimulation()
{
	int now = 0;
	bool dead = false;

	switch( getConfig<CConfiguration::EnumRoutingProtocolScheme>("simulation", "routing_protocol") )
	{
		case config::_prophet:
			
			CProphet::Init();
			while( now <= getConfig<int>("simulation", "runtime") )
			{
				dead = !CProphet::Operate(now);

				if( dead )
				{
					updateConfig<int>("simulation", "runtime", now);
					break;
				}
				now += getConfig<int>("simulation", "slot");
			}
			CProphet::PrintFinal(now);

			break;

		case config::_xhar:

			HAR::Init(now);
			while( now <= getConfig<int>("simulation", "runtime") )
			{
				dead = !HAR::Operate(now);

				if( dead )
				{
					updateConfig<int>("simulation", "runtime", now);
					break;
				}
				now += getConfig<int>("simulation", "slot");
			}
			HAR::PrintFinal(now);

			break;

		case config::_pferry:

			while( now <= getConfig<int>("simulation", "runtime") )
			{
				dead = !CPFerry::Operate(now);

				if( dead )
				{
					updateConfig<int>("simulation", "runtime", now);
					break;
				}
				now += getConfig<int>("simulation", "slot");
			}
			CPFerry::PrintFinal(now);

			break;

		default:
			break;
	}

	return true;
}

bool CRunHelper::Run(int argc, char* argv[])
{
	srand(static_cast<unsigned>( time(nullptr) ));

	try
	{
		PrepareSimulation(argc, argv);
		RunSimulation();
	}
	catch(string error)
	{
		CPrintHelper::PrintError(error);
		Exit(EERROR, error);
	}
	catch(pair<int, string> &pairError)
	{
		CPrintHelper::PrintError(pairError);
		Exit(pairError.first, pairError.second);
	}
	catch(exception ex)
	{
		CPrintHelper::PrintError(ex);
		Exit(EERROR, ex.what());
	}

	Exit(EFINISH);

	return true;
}

bool CRunHelper::Debug()
{
	return false;

	CFileHelper::test();
	CParseHelper::test();

	CConfiguration::test();

	Exit(ESKIP);
	return true;
}

