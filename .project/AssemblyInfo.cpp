////#include "stdafx.h"
////#using <mscorlib.dll>


//
// 1. ʹ��`using namespace System;`Ҫ��������Ŀ - �������� - ���� - ������������ʱ֧�֣�ʹ�ù�����������ʱ֧��(/clr)
// 2. 1 ��Ҫ����Ŀ - �������� - C/C++ - �������� - ���п⣺���߳�DLL(\MD)
// 3. 2 ��Ҫ����Ŀ - �������� - C/C++ - MFC��ʹ�ã��ڹ��� DLL ��ʹ��MFC
//
using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;



//
// �йس��򼯵ĳ�����Ϣͨ���������Լ�
// ���ơ�������Щ����ֵ���޸�
// ����򼯹�������Ϣ��
//

//
// ����
// �������򼯵����ƣ��磺ĳĳ��˾ĳĳ��Ŀĳĳģ��ȡ�
// �����ƿ������κκϷ����ַ����������пո�
[assembly: AssemblyTitleAttribute("Hotspot-Based Duty Cycle")];
//
// ����
// ���򼯵ļ��������繦�ܡ����ԡ����ԡ�Լ����
[assembly: AssemblyDescriptionAttribute("xHAR: original HAR, iHAR, mHAR, HDC & latest try ...")];
//
// ��Ʒ����
[assembly: AssemblyProductAttribute("xHAR (Hotspot-Based Adaptive Routing)")];
//
// ��˾��Ϣ
[assembly: AssemblyCompanyAttribute("Tian-Si COMMUNICATION LAB @ B325")];
//
// �̱���Ϣ
[assembly: AssemblyTrademarkAttribute("(R) HAR")];
//
// ������Ϣ
// ö�ٵ��ֶα�������֧�ֵ������ԡ�
// ����Ҳ����ָ����������ԣ���������������Ĭ�������Ե���Դ��
// ���п⽫�κ�����������δ��Ϊ�յĳ��򼯰��������򼯴���
// ��������ܸ������򼯰󶨹���Լ������ϸ��Ϣ����μ����п���ζ�λ���򼯡�
[assembly: AssemblyCultureAttribute("")];
//
// ��Ȩ��Ϣ
[assembly: AssemblyCopyrightAttribute("Copyright (c) 2016")];
//
// �����ļ�
// ���򼯵�������Ϣ���磺���ۡ����������Եȣ�.NET����ʱû��ʹ�ô�����
[assembly: AssemblyConfigurationAttribute("alpha")];


//
// ���򼯵İ汾��Ϣ�������ĸ�ֵ���:
//
//       ���汾
//       �ΰ汾
//       �����
//       �޶���
//
// ��ǿ���Ƴ����� CLR ʹ�ô�ֵ���󶨲�����
// ����ʹ��ͨ��� * ������ڲ��汾���޸İ汾��VS ���Զ�Ϊ�����ɰ汾�š�
// �綨��Ϊ �� 1.0.*������ VS ���Զ����ɺ���Ĳ��֣�����汾������ 2000 �� 2 �� 1 �������������������汾��Ϊ����ҹ����������������� 2 ���õ�ֵ����
// ����趨Ϊ ��*������ÿ���޸ĳ����汾�Ŷ����Զ����������
// 
// ���汾�ź��Ӱ汾��Ӧ����д�ã���Ϊ�ⲿ���ǲ���Ƶ���䶯�ģ�������汾ʹ��ͨ��� * ���档
[assembly: AssemblyVersionAttribute("3.2.5928.23405")];
//
// �ļ��汾��Ϣ
// ���ļ��汾��Ϣע�͵������ļ��汾������򼯰汾����ͬ��
//[assembly: AssemblyFileVersion("1.0.0.0")]

//
// �Ƿ�ʹ���ӳ�ǩ��
[assembly: AssemblyDelaySign(false)]
//
[assembly: ComVisibleAttribute(true)];
//
[assembly: CLSCompliantAttribute(true)];
//
// ����������Կ�ԣ���Ϊ���������������ԵĹ��캯��������Կ������
[assembly: AssemblyKeyName("")]
//
// �����˹�Կ�����ʹ���ӳ�ǩ�������߼Ȱ�����ԿҲ����˽Կ���ļ�����
// �˹�Կ��˽Կ����Ϊ���������������ԵĹ��캯����
// �ļ�����������ļ�·����.exe �� .dll����أ���Դ�ļ�·���޹ء�
[assembly: AssemblyKeyFile("")] 
//
[assembly: SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];

//
// ��ȡ�ļ���Ϣ��
//
//
//object[] attr = assembly.GetCustomAttributes( typeof(AssemblyDescriptionAttribute), false);
//string title = ( ( AssemblyDescriptionAttribute )attr[0] ).Description;
//
// ���У�����ͨ���ı� typeof �еĲ������ʲ�ͬ�����ԣ�ע��Ҫ�� attr �����ͽ��ж�Ӧ������ת����
//