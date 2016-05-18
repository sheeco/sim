// Dataset.cpp : 定义控制台应用程序的入口点。
//

#pragma once

#include "MotionRange.h"

namespace motionrange
{
	string POSTFIX = ".trace";
	string FILE_LOG = "./range.log";


	//如果文件不存在，返回值中的 node_id 为空
	range motionrange::calculateRangeForFile(string filename, string filelog)
	{
		ifstream input;
		input.open(filename, ios::in || ios::_Nocreate);
		range temp_range = { "", 0, 0, 0, 0 };

		if( !input.is_open() )
			return temp_range;

		temp_range.node_id = filename.substr( filename.find_last_of('/') + 1, filename.rfind('.') - 2 );
		cout << filename << " ..." << endl;

		while( !input.eof() )
		{
			double temp_time;
			double temp_x, temp_y;
			input >> temp_time;
			input >> temp_x;
			input >> temp_y;

#ifdef __MOTION_RANGE_DEBUG__
			cout << TAB << temp_time << TAB << temp_x << TAB << temp_y << endl;
#endif

			if( temp_x < temp_range.min_x )
				temp_range.min_x = temp_x;
			else if( temp_x > temp_range.max_x )
				temp_range.max_x = temp_x;

			if( temp_y < temp_range.min_y )
				temp_range.min_y = temp_y;
			else if( temp_y > temp_range.max_y )
				temp_range.max_y = temp_y;
		}

		// TODO: add assertion
		ofstream log(filelog, ios::app);
		log << temp_range.node_id << TAB;
		double range_x = temp_range.max_x - temp_range.min_x;
		double range_y = temp_range.max_y - temp_range.min_y;

		log << range_x << TAB;
		log << range_y << TAB;
		log << sqrt(range_x * range_x + range_y * range_y) << endl;
		log.close();

		return temp_range;
	}

	// TODO: untested
	//postfix e.g. '.trace'
	vector<string> motionrange::getFilesForPath(string path, string postfix)
	{
		vector<string> files;
		long   hFile = 0; //文件句柄    
		struct _finddata_t fileinfo; //文件信息

		string temp;

		if( ( hFile = _findfirst(temp.assign(path).append("/*").c_str(), &fileinfo) ) != -1 )
		{

			do
			{
				string foundpath = temp.assign(path).append("/").append(fileinfo.name);

				//如果是目录，迭代之  
				if( ( fileinfo.attrib   &   _A_SUBDIR ) )
				{
					if( strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0 )
					{
						vector<string> curr = getFilesForPath(foundpath, postfix);
						files.assign(curr.begin(), curr.end());
					}
				}
				//如果不是，检查后缀并加入列表  
				else
				{
					if( !postfix.empty()
					   && foundpath.rfind(postfix.c_str()) != postfix.npos )
						files.push_back(foundpath);
				}
			} while( _findnext(hFile, &fileinfo) == 0 );

			_findclose(hFile);
		}

		return files;
	}

	vector<range> motionrange::calculateRangeForAllFiles(string postfix, string filelog)
	{
		vector<range> ranges;
		vector<string> files = getFilesForPath(".", postfix);
		for( auto ifile = files.begin(); ifile != files.end(); ++ifile )
		{
			range ret = calculateRangeForFile(*ifile, filelog);
			if( !ret.node_id.empty() )
				ranges.push_back(ret);
		}

		return ranges;
	}

	int motionrange::MotionRange(int argc, char * argv[])
	{
		vector<range> ranges;

		int iField = 0;
		for( iField = 1; iField < argc; )
		{
			string field = argv[iField];

			//<mode> 不带数值的布尔型参数
			if( field == "-postfix" )
			{
				if( field[0] == '.' )
					POSTFIX = field;
				iField += 2;
			}
			else if( field == "-log" )
			{
				FILE_LOG = argv[iField + 1];
				iField += 2;
			}
		}

		calculateRangeForAllFiles(POSTFIX, FILE_LOG);

		cout << "Range info saved to " << FILE_LOG << endl;
		_PAUSE_;

		return 0;
	}

}