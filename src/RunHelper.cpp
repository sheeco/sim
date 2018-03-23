#include "RunHelper.h"
#include "Configuration.h"
#include "Prophet.h"
#include "HAR.h"
#include "HDC.h"
#include "PFerry.h"
#include "PrintHelper.h"
#include "FileHelper.h"
#include "ParseHelper.h"


void CRunHelper::InitLogPath()
{
	// Create root path (../test/) if doesn't exist
	if( access(getConfig<string>("log", "dir_log").c_str(), 00) != 0 )
		_mkdir(getConfig<string>("log", "dir_log").c_str());

	// Generate timestamp & output path
	time_t seconds;  //秒时间  
	char temp[65] = { '\0' };
	seconds = time(nullptr); //获取目前秒时间  
	strftime(temp, 64, "%Y-%m-%d-%H-%M-%S", localtime(&seconds));
	string timestring;
	timestring = string(temp);
	CConfiguration::updateConfiguration<string>("log", "timestamp", timestring, true);
	CConfiguration::updateConfiguration<string>("log", "info_log", string("@" + timestring + TAB), true);

	string tag = getConfig<string>("log", "tag");
	if(!tag.empty())
		tag + "[" + tag + "]";
	string logpath = "." + tag + timestring + "/";
	CConfiguration::updateConfiguration<string>("log", "path_timestamp", logpath, true);

	// Create log path
	if( access(( getConfig<string>("log", "dir_log") + logpath ).c_str(), 00) != 0 )
		_mkdir(( getConfig<string>("log", "dir_log") + logpath ).c_str());

	// Hide folder
	CFileHelper::SetHidden( getConfig<string>("log", "dir_log") + logpath);

}

bool CRunHelper::PrepareSimulation(int argc, char* argv[])
{
	CConfiguration::InitConfiguration();

	InitLogPath();

	/*********************************************  按照命令格式解析参数配置  *********************************************/

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

	CNode::Init(now);

	switch( getConfig<config::EnumRoutingProtocolScheme>("simulation", "routing_protocol") )
	{
		case config::_prophet:
			
			CProphet::Init(now);
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

			CPFerry::Init(now);
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

void CRunHelper::Exit(int code)
{
	time_t seconds;  //秒时间  
	char temp_time[65];
	seconds = time(nullptr); //获取目前秒时间  
	strftime(temp_time, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));

	// Remove name prefix '.' in front of log folder

	if( code == EFINISH )
	{
		string dir = getConfig<string>("log", "dir_log");
		string logpath = getConfig<string>("log", "path_timestamp");
		if(_access(( dir + logpath ).c_str(), 02) == 0
		   && ( logpath.find(".") != logpath.npos ) )   //if writeable & '.' found in filename
		{
			string newpath = logpath.substr(1, logpath.npos);
			if( _access(( dir + newpath ).c_str(), 00) != 0 )  //if no collision
			{
				rename(( dir + logpath ).c_str(), ( dir + newpath ).c_str());
				updateConfig<string>("log", "path_timestamp", newpath);
			}
		}

		// Unhide folder
		CFileHelper::UnsetHidden(dir + getConfig<string>("log", "path_timestamp"));
	}

	// Alert & Pause
	CPrintHelper::Alert();
	if( code > EFINISH )
		_PAUSE_;

	exit(code);
}

void CRunHelper::Exit(int code, string error)
{
	ofstream errorFile(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_error"), ios::app);
	errorFile << error << endl << endl;
	errorFile.close();

	Exit(code);
}

