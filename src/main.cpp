#pragma once

#include "RunHelper.h"


// TODO: move all func definition into cpp file except for inline func
// TODO: ����������ھ�̬���������� private / protected
// TODO: Ĭ�����ò�����Ϊ�� XML ��ȡ

// TODO: removc config '-node' (include all the files under given directory & read node identifiers from filename)
// TODO: separate configs into groups (global, har, ihar, prophet, epidemic, lstm ...)


int main(int argc, char* argv[])
{
	return CRunHelper::Run(argc, argv);
	//return CRunHelper::Debug();
}