#pragma once

#include "RunHelper.h"


// TODO: 检查所有类内静态变量，决定 private / protected
// TODO: add static attribute for configs of protocols & read from getConfig() in Init()

int main(int argc, char* argv[])
{
	CRunHelper::Debug();
	CRunHelper::Run(argc, argv);
}