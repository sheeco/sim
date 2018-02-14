#pragma once

#include "RunHelper.h"


// TODO: 检查所有类内静态变量，决定 private / protected


int main(int argc, char* argv[])
{
	CRunHelper::Debug();
	CRunHelper::Run(argc, argv);
}