#include "RunHelper.h"
#include "Configuration.h"
#include "Prophet.h"
#include "HAR.h"
#include "HDC.h"
#include "PrintHelper.h"
#include "ParseHelper.h"


void CRunHelper::InitLogPath()
{
	// Generate timestamp & output path

	// Create root path (../test/) if doesn't exist
	if( access(DIR_LOG.c_str(), 00) != 0 )
		_mkdir(DIR_LOG.c_str());

	time_t seconds;  //秒时间  
	char temp[65] = { '\0' };
	seconds = time(nullptr); //获取目前秒时间  
	strftime(temp, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));
	TIMESTAMP = string(temp);
	strftime(temp, 64, "%Y-%m-%d-%H-%M-%S", localtime(&seconds));
	string timestring;
	timestring = string(temp);
	INFO_LOG = "@" + TIMESTAMP + TAB;
	PATH_TIMESTAMP = "." + timestring + "/";

	// Create log path
	if( access(( DIR_LOG + PATH_TIMESTAMP ).c_str(), 00) != 0 )
		_mkdir(( DIR_LOG + PATH_TIMESTAMP ).c_str());

	// Hide folder
	LPWSTR wstr = CString(( DIR_LOG + PATH_TIMESTAMP ).c_str()).AllocSysString();
	int attr = GetFileAttributes(wstr);
	if( ( attr & FILE_ATTRIBUTE_HIDDEN ) == 0 )
	{
		SetFileAttributes(wstr, attr | FILE_ATTRIBUTE_HIDDEN);
	}

}

bool CRunHelper::Simulation(vector<string> args)
{
	PrepareSimulation(args);
	RunSimulation();

	return true;
}

bool CRunHelper::PrepareSimulation(vector<string> args)
{
	InitLogPath();

	CConfiguration::InitConfiguration();

	CConfiguration::ParseConfiguration(args);

	CConfiguration::ValidateConfiguration();

	CConfiguration::ApplyConfigurations();

	CConfiguration::PrintConfiguration();

	return true;
}

bool CRunHelper::RunSimulation()
{
	int currentTime = 0;
	bool dead = false;

	switch( ROUTING_PROTOCOL )
	{
		case _prophet:
			
			CProphet::Init();
			while( currentTime <= RUNTIME )
			{
				dead = !CProphet::Operate(currentTime);

				if( dead )
				{
					RUNTIME = currentTime;
					break;
				}
				currentTime += SLOT;
			}
			CProphet::PrintFinal(currentTime);

			break;

		//case _epidemic:
		//	CEpidemic::Init();
		//	while( currentTime <= RUNTIME )
		//	{
		//		dead = !CEpidemic::Operate(currentTime);

		//		if( dead )
		//		{
		//			RUNTIME = currentTime;
		//			break;
		//		}
		//		currentTime += SLOT;
		//	}
		//	CEpidemic::PrintFinal(currentTime);

			//break;

		case _xhar:

			HAR::Init();
			while( currentTime <= RUNTIME )
			{
				dead = !HAR::Operate(currentTime);

				if( dead )
				{
					RUNTIME = currentTime;
					break;
				}
				currentTime += SLOT;
			}
			HAR::PrintFinal(currentTime);

			break;

		default:
			break;
	}

	return true;
}

bool CRunHelper::Run(int argc, char* argv[])
{
	// TODO: release 版本中应改为 while(1) 循环

	srand(static_cast<unsigned>( time(nullptr) ));

	try
	{

		vector<string> args = CConfiguration::ConvertToConfiguration(argc - 1, ( argv + 1 ));

		Simulation(args);

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
	CFileHelper::test();
	CParseHelper::test();

	Exit(ESKIP);
	return true;
}

