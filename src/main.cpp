#pragma once

#include "RunHelper.h"


// TODO: ����������ھ�̬���������� private / protected
// TODO: add static attribute for configs of protocols & read from getConfig() in Init()
// TODO: replace auto-generated ID for CNode with read identifier ?

int main(int argc, char* argv[])
{
	CRunHelper::Debug();
	CRunHelper::Run(argc, argv);
}