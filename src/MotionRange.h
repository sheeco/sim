// Dataset.cpp : 定义控制台应用程序的入口点。
//

#pragma once

#ifndef __MOTION_RANGE_H__
#define __MOTION_RANGE_H__

#include "Global.h"

#define __MOTION_RANGE_DEBUG__

namespace motionrange
{
	typedef struct range
	{
		string node_id;
		double min_x;
		double min_y;
		double max_x;
		double max_y;
	} range;

	extern string POSTFIX;
	extern string FILE_LOG;

	//如果文件不存在，返回值中的 node_id 为空
	range calculateRangeForFile(string filename, string filelog);

	// TODO: untested
	//postfix e.g. '.trace'
	vector<string> getFilesForPath(string path, string postfix);

	vector<range> calculateRangeForAllFiles(string postfix, string filelog);

	int MotionRange(int argc, char* argv[]);

}

#endif // __MOTION_RANGE_H__