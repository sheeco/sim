#pragma once

#include "RunHelper.h"


// TODO: move all func definition into cpp file except for inline func
// TODO: 检查所有类内静态变量，决定 private / protected
// TODO: CConfiguration / CConfigureHelper ?
// TODO: 默认配置参数改为从 XML 读取
// TODO: MulitCast ?



int main(int argc, char* argv[])
{
	return CRunHelper::Run(argc, argv);
}