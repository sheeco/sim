////#include "stdafx.h"
////#using <mscorlib.dll>


//
// 1. 使用`using namespace System;`要求开启：项目 - 配置属性 - 常规 - 公共语言运行时支持：使用公共语言运行时支持(/clr)
// 2. 1 将要求：项目 - 配置属性 - C/C++ - 代码生成 - 运行库：多线程DLL(\MD)
// 3. 2 将要求：项目 - 配置属性 - C/C++ - MFC的使用：在共享 DLL 中使用MFC
//
using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;



//
// 有关程序集的常规信息通过下列特性集
// 控制。更改这些特性值可修改
// 与程序集关联的信息。
//

//
// 标题
// 描述程序集的名称，如：某某公司某某项目某某模块等。
// 此名称可以是任何合法的字符串，可以有空格。
[assembly: AssemblyTitleAttribute("Hotspot-Based Duty Cycle")];
//
// 描述
// 程序集的简单描述，如功能、语言、特性、约束等
[assembly: AssemblyDescriptionAttribute("xHAR: original HAR, iHAR, mHAR, HDC & latest try ...")];
//
// 产品名称
[assembly: AssemblyProductAttribute("xHAR (Hotspot-Based Adaptive Routing)")];
//
// 公司信息
[assembly: AssemblyCompanyAttribute("Tian-Si COMMUNICATION LAB @ B325")];
//
// 商标信息
[assembly: AssemblyTrademarkAttribute("(R) HAR")];
//
// 区域信息
// 枚举的字段表明程序集支持的区域性。
// 程序集也可以指定区域独立性，表明它包含用于默认区域性的资源。
// 运行库将任何区域性属性未设为空的程序集按附属程序集处理。
// 此类程序集受附属程序集绑定规则约束。详细信息，请参见运行库如何定位程序集。
[assembly: AssemblyCultureAttribute("")];
//
// 版权信息
[assembly: AssemblyCopyrightAttribute("Copyright (c) 2016")];
//
// 配置文件
// 程序集的配置信息，如：零售、发布、调试等，.NET运行时没有使用此属性
[assembly: AssemblyConfigurationAttribute("alpha")];


//
// 程序集的版本信息由下面四个值组成:
//
//       主版本
//       次版本
//       编译号
//       修订号
//
// 在强名称程序集中 CLR 使用此值来绑定操作。
// 可以使用通配符 * 来替代内部版本和修改版本，VS 将自动为其生成版本号。
// 如定义为 “ 1.0.*”，则 VS 会自动生成后面的部分（编译版本号是自 2000 年 2 月 1 日以来的天数；修正版本号为自午夜零点以来的秒数除以 2 所得的值）。
// 如果设定为 “*”，则每次修改程序后版本号都会自动发生变更。
// 
// 主版本号和子版本号应当填写好，因为这部分是不会频繁变动的，而编译版本使用通配符 * 代替。
[assembly: AssemblyVersionAttribute("3.1.5925.19658")];
//
// 文件版本信息
// 将文件版本信息注释掉，则文件版本号与程序集版本号相同。
//[assembly: AssemblyFileVersion("1.0.0.0")]

//
// 是否使用延迟签名
[assembly: AssemblyDelaySign(false)]
//
[assembly: ComVisibleAttribute(true)];
//
[assembly: CLSCompliantAttribute(true)];
//
// 表明包含密钥对（作为参数传递至此属性的构造函数）的密钥容器。
[assembly: AssemblyKeyName("")]
//
// 包含了公钥（如果使用延迟签名）或者既包含公钥也包含私钥的文件名。
// 此公钥和私钥将作为参数传递至此属性的构造函数。
// 文件名称与输出文件路径（.exe 或 .dll）相关，与源文件路径无关。
[assembly: AssemblyKeyFile("")] 
//
[assembly: SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];

//
// 读取文件信息：
//
//
//object[] attr = assembly.GetCustomAttributes( typeof(AssemblyDescriptionAttribute), false);
//string title = ( ( AssemblyDescriptionAttribute )attr[0] ).Description;
//
// 其中，可以通过改变 typeof 中的参数访问不同的属性，注意要将 attr 的类型进行对应的类型转换。
//