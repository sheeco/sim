#include "Global.h"
#include "Configuration.h"
#include "PrintHelper.h"


namespace global
{

}

void global::Exit(int code)
{
	time_t seconds;  //秒时间  
	char temp_time[65];
	seconds = time(nullptr); //获取目前秒时间  
	strftime(temp_time, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));

	// Remove name prefix '.' in front of log folder

	if( code == EFINISH )
	{

		if( _access(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str(), 02) == 0
		   && ( getConfig<string>("log", "path_timestamp").find(".") != getConfig<string>("log", "path_timestamp").npos ) )   //if writeable & '.' found in filename
		{
			string newPathLog = getConfig<string>("log", "path_timestamp").substr(1, getConfig<string>("log", "path_timestamp").npos);
			if( _access(( getConfig<string>("log", "dir_log") + newPathLog ).c_str(), 00) != 0 )  //if no collision
			{
				rename(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str(), ( getConfig<string>("log", "dir_log") + newPathLog ).c_str());
				updateConfig<string>("log", "path_timestamp", newPathLog);
			}
		}

		// Unhide folder
		CFileHelper::UnsetHidden( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") );
	}

	// Alert & Pause
	if( code > EFINISH )
	{
		CPrintHelper::Alert();
	}

	_PAUSE_;
	exit(code);
}

void global::Exit(int code, string error)
{
	ofstream errorFile(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_error"), ios::app);
	errorFile << error << endl << endl;
	errorFile.close();

	Exit(code);
}
