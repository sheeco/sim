
>***Created by Eco Sun on 2015-11-03***


#### Log Mark

*�� Git Commit Comment ��ʹ�ÿ��ٱ�ǩ��*

���� `+` ��ӡ�`-` ɾ����`~` �޸ġ�`#` �Ż���`?` ���ԡ�`!` ����`$` �޸���`%` ���ԣ�

*�� `CHANGE.md` �ĸ�����־��ʹ�÷����Ǳ�ǩ��*

���� `ADD` ����¹��ܡ�`REM` ɾ�����й��ܡ�`MOD` �޸�����ʵ�֡�`OPT` �����Ż���`TRY` ��ȷ�����ԣ�

���� `BUG` ���޸�����`FIX` �޸�����`TEST` ���ܲ��ԣ�

���� `RFCT` �����ع���`MNT` ���ά����`NOTE` ����˵����`TODO` �ƻ�����



# Change Log


## 2.* : Previous Modifications


###### 2015-09-01 -- 2015-11-13  ��  *< 2.0.0 >*

- �����ɰ汾���ȵ�ѡȡ������ѡȡ�еĴ���int �����ȣ���

- ���е����ò�������Ϊ�����д��룬���������ʽ�� `-help` �鿴������û��ʵ���㹻�ĺϷ��Լ�飬�����Ҫ������ʱע�⣩��

- �鲢 HAR (mHAR) �����룬�������� `-merge`, `-old`, `-heat[-exp/-ln]`�����������д��룻

- Ϊ mHAR �е��ȶȺ��������ָ������������������ѡ����Խ����ʾû��Ӱ�죬ԭ����δ���з�����֮��Ĳ��Զ�Ӧ��ʱʹ�� flat ѡ���

- �������������ȵ�ѡȡ��������ƶȱȽϹ��ܣ��������д��� `-hotspot-similarity` ���ɣ�

- �����е�����ļ�ǰ��ӡʱ����;���� scheme ����ʶ��ÿһ�����У���ӡ˵����Ϣ����ʶ��ÿһ����Ϣ����ͳһ���ݺ��ļ����ĸ�ʽ��

- ��������Ͷ���ʼ���Ĵ��󣨵�ǰֵ instead of ��������

- �������õ���Ҫ������ʽ�������� `debug.txt` �У�����ʱֱ�Ӹ��Ƶ� xls �ĵ����ɣ�

- ������Ͷ�ݷ���Ϊ (a) MA ���ȵ��ϵõ�Ͷ�ݺ� (b) MA ��·���ϵõ�Ͷ�ݣ���ͳ������Ͷ�ݼ����İٷֱȣ�

- ����ÿһ���ȵ�ͳ����Ͷ�ݼ����������ȵ�ѡȡʱ��������������� `atHotspot` �� `nullptr` ʱ�����Ż���

- mHAR ����������ѡȡ ratio ָ����������̰��ѡȡ�п��ܳ��� position ���ϻ�û�еõ�ȫ���ǣ�`hotspotsAboveAverage` Ϊ�յ��������ʱֱ��ѡ�� `unselectedHotspots` �� cover ������Ԫ�ؼ��ɣ�


###### 2015-11-13  ��  *< 2.0.1 >*

- ADD��������С�ȴ�ʱ��(`MIN_WAITING_TIME`)��Ĭ��Ϊ 0 ����ʹ�ø��޸ģ����Դ������� `-min-wait` ��ֵ��

- ADD��`delivery-hotspot.txt` �е���Ϣ��Ϊ��������֮������������ڷ����ȵ�Ͷ�ݼ����ļ��г̶ȣ��������ݺ��ڴ���Ĺ�������


###### 2015-11-16  ��  *< 2.0.2 >*

- ADD���淶�������в�������Ĺؼ��֣�����̬�ڵ�����ѡ��Ҳ���������в�����


###### 2015-11-20  ��  *< 2.0.3 >*

- ADD���� node �� ID ��Ϊ�Զ����ɣ�����֮�޸Ľڵ�����̬�仯ʱ�� node �Ƿ���ڵ��ж�����Ӻ��� `HAR::ifNodeExists(int id)` ���� `id >= m_nodes.size()` ���ж���������δ���ԣ���


### 2.1.*


###### 2015-11-24  ��  *< 2.1.0 >*

- FIX��<��̬�ڵ��������> ����һ���ȵ�ѡȡ֮ǰ `HAR::ChangeNodeNumber()` ɾ���������ڵ��λ�õ���Ϣ��ע�⣺�ڵ�����仯����Ӧ��Ϊ�ȵ�ѡȡ���ڵı�������
- RFCT�������е� `g_` ϵ��ȫ�ֱ�������Ϊ������ڵľ�̬������GA ��δ���õķ�����صı���δ�Ķ�����
- ADD������ `CHotspot::ratio` ����ط����͸Ķ������ڲ����µ� ratio ���㷽��������̰��ѡȡ�ͺ���ѡȡ�������õ�����ʹ�������в��� `-balanced-ratio` ѡ�����
- ADD������ `poor-hotspot.txt` ���ڲ���Ͷ�ݼ���Ϊ 0 ���ȵ���Ϣ��
- [ ] ADD��Ӧ�ý���̬�ڵ��������ʱ����ɾ�ڵ���̸ĳ����ѡȡ�ڵ㣻
- [ ] MOD������ȵ������Ӧ�øĳɴӴ�С�ģ�


###### 2015-11-25  ��  *< 2.1.1 >*

- RFCT���� `mergesort` ��Ӧ�� comp ������ֽ��й淶����
- ADD������Ͷ�ݼ�����������ȵ� cover �� position ����node ���� ratio �� `hotspot-rank.txt`������ͳ�ƺͷ����ȵ�������
- ADD����������ͳ�Ƶȴ�ʱ����ۻ�ֵ��ͬ������� `hotspot-rank.txt` �ļ���


###### 2015-11-27  ��  *< 2.1.2 >*

- TRY��Ϊÿ�� position ���ڳ�ʼֵΪ 1 ��Ȩֵ������ `CSortHelper::DecayPositionsWithoutDeliveryCount`������Ͷ�ݼ���Ϊ 0 ���ȵ���� position ִ��˥������ʹ�������в��� `-learn` ѡ�����`-decay` ����ָ����˥��ϵ����
- NOTE������˥����� position ����Ȩֵˮƽ�������ͣ�����ѡȡ�����еĹ̶� alpha ֵ������ѡȡ�����ȵ����������������ڲ��Խ׶���ʱʹ�� `-max-hotspot` ����ʱ�ȶ��ȵ�������
- TRY������ `MIN_POSITION_WEIGHT`��Ȩֵ˥��֮��������ڴ�ֵ����ֱ��ɾ����ʹ�������в��� `-min-weight` ��ֵ��Ĭ��ֵΪ 1 ����ɾ������δ���ԣ���
- FIX���޸� `CHotspot::calculateRatio` �е� int �������⣻
- FIX���޸� `CHotspot::waitingTimes` ����ͳ�Ƶ����⣻


### 2.2.* : ���ع�


###### 2015-12-04  ��  *< 2.2.0 >*  ��  *���ع�*

- RFCT�����������ṹ�����˵�������ļ̳нṹ�淶����



> *** ����Ϊ HDC ��֧�ĳ�ʼ״̬ ***

> *** ������ [ 2015-12-04 �� master ��֧ ]( 0dd0e561bf88035d1cfc557e2083dfa602ab9d43 ) ***

> *** ����Ϊ HDC ��֧�µĸ��� ***


��
###### 2015-12-04  ��  *< 2.2.1 >*

- MOD�����Ĺ�����������·����Ϊ `HDC`��
- OPT������ɾ��δʹ�õĺ������壬���� GA �㷨��ص�ȫ�ֱ����ʹ�������


###### 2015-12-09

- MOD���޸� `CData` �� `CNode` ������Ӧ Epidemic ·�ɣ����� HOP �� TTL ��ѡ��ͨ��������Ӧ�����ֵ�������� `CData` ��ĸ��ֲ����п����Զ��жϣ�


###### 2015-12-10

- MOD�������޸� `CData` �� `CNode` �࣬`CNode` ����ԭ�������ȫ�� node ͳһͳ���ܺĵģ����ڸ�Ϊ����ÿ���ڵ㵥��ͳ���ܺģ�ȫ�ֵ� node �ܺĸ�Ϊ����ʱͳ�ƣ�
- ADD����ԭ�����ܺ�ͳ����غ������ `SIZE_DATA`��`SIZE_CTRL`������ָʾ�������ݰ��Ϳ��ư��Ĵ�С��
- ADD������ `CHDC` �ࡢ`Epidemic` ��� `CMacProtocol` ���࣬ԭ `HAR` �����й��ȵ�ѡ��ĺ�����ʱ���� `CHDC` �࣬�й�·�ɵ�����ʱ���� `Epidemic` �࣬���Ա���Ժͺ����Դ��޸ģ�


### 2.3.* : Epidemic + HDC


###### 2015-12-11  ��  *< 2.3.0 >*  ��  *Epidemic + HDC*

- ADD���� `CNode::state` ��ʶ�ͼ���ڵ�Ĺ���״̬��ȡֵ��Χ�� `[ - SLOT_TOTAL, + SLOT_WAKE )` ֮�䣬ֵ���ڵ��� 0 ������ WAKE ״̬��
- ADD������ͨ�ųɹ��ʣ��������ͨ��ʱһ����һ��ͨ��ʧ�ܣ�֮���ͨ�Ŷ����ٳ�����
- ADD���ڵ��ͨ�ųɹ��ʺ�������ģ����������� `CNode` �������е������շ������У�
- ADD���������ڵ�����������Դ�����������ܺġ������ܺġ����ݷ����ܺġ����ݽ����ܺģ�
- ADD����ӽڵ�����������ԣ��Լ���Ӧ�Ľڵ�ʧЧ������������ֹ���ж���Ĭ��Ϊ 0 ����������������ͨ�������в��� `-node-energy` ��ֵ��
- ADD������ HDC �е��ȵ�ѡȡ��������ʱֻ�ṩ HAR �� IHAR ��ѡ��������ͨ�������в����޸ģ�
- NOTE�������޸���ϣ�������������ԣ�Epidemic ·�ɵ���ز������ܺĲ����������������ʼ���С������ʱ϶��ռ�ձȲ�����
- NOTE��MAC Э����޹����ݣ����Դ����ӳ٣�֮��Ҳ���Լ�����㣩��


###### 2015-12-23  ��  *< 2.3.1 >*

- MOD���� HDC ����ͨ DC �ĶԱȸĳɣ���ͨ DC ʼ��ʹ��Ĭ�� DC ֵ��`DEFAULT_DUTY_CYCYLE`����
- ----- HDC �ڷ��ȵ㴦ʹ�ø��ͣ�����Ϊ 0 �ģ�Ĭ�� DC ֵ���ȵ㴦ʹ������ͨ DC �е�Ĭ�� DC ֵ��Ȼ���ߵ��ȵ� DC ֵ��`HOTSPOT_DUTY_CYCYLE`����


###### 2015-12-28  ��  *< 2.3.2 >*

- MNT���� `README.txt` ��Ϊ MarkDown �ĵ� `README.md`���������ṹ�͸�ʽ��


###### 2016-01-22

- OPT���� sink �ڵ� ID ��Ϊ 0���Ӷ��� map ��������ǰ������ң���`CData`��`CPosition`��`CNode`��`CHotspot` �� ID ����Ϊ�� 1 ��ʼ��ʵʱֵ��Ȼ���ڵ�ǰʵ��������
- ADD����� `Prophet` �࣬��Ҫ���� `CNode::DeliveryPreds` ����ز���������Prophet ·����ʹ�õ�����������Ȼʹ�� Epidemic ·���е� `CNode::SummaryVector` ��ʵ�֣����庯�������ԣ�


### 2.4.* : Prophet + HDC


###### 2016-02-09  ��  *< 2.4.0 >*  ��  *Prophet + HDC*

- FIX����� `Prophet` ����صĵ��ԣ�
- OPT���켣�ļ� .newlocation ����Ÿ�Ϊ�� 1 ��ʼ����Ŷ�Ӧ `CNode::ID`����ɾ�ڵ�ʱ�Ա��ֶ�Ӧ��ϵ��
- ADD���ڽ������������ file-formatter ��Ŀ�������Զ��޸Ĺ켣�ļ���ţ�


###### 2016-02-10  ��  *< 2.4.1 >*

- OPT��`debug.txt` ���Զ��жϲ�����ļ�ͷ��
- OPT������ `cout` �����ʽ�������ݴ�����ȵ�ѡȡ�����е������Ϣ��Ϊ��д��ǰ�У�����̬��ʾ���ķ�ʽ���������� HDC �е��õĲ��֣�`Epidemic` �� `HAR` �����е���������δ���£���
- OPT���������г���ͽ���֮��������ѣ�`BEEP 0x07 '\a'`����


###### 2016-02-11  ��  *< 2.4.2 >*

- RFCT���� `Epidemic` ��� `Prophet` ���еĹ��к����������� `CRoutingProtocol` �У�����ʱ���жϺ�ͳһ�������ʽ����������͹淶�����������е�ʣ�ຯ����
- RFCT���� `CSortHelper` ���еĲ��ָ�����������������У�


### 2.5.* : ReSharper


###### 2016-02-14  ��  *< 2.5.0 >*  ��  *ReSharper*

- RFCT������ ReSharper ��������� ReSharper �Ľ����Ż�����ϸ�ڣ�getter ������ const��ǰ׺��������auto ���͵ȣ���
- RFCT����� Epidemic��Prophet��HAR ����·���༰�丸��ĺ�������������ͳһ���Լ� HAR �� HDC �е��ȵ�ѡȡ�����ĵ�����ͳһ��
- RFCT���������еļ̳нṹ�����캯�������ʿ��ơ�����ͳһ�����ຯ���ϲ������������������ȣ���
- OPT��Ϊ���е�����ļ������ļ�ͷ�ַ�����`string INFO_DEBUG` �ȣ���
- RFCT����һЩѡ��浽ȫ�ֶ�������������У�`SEND::COPY/DUMP`��`RECEIVE::LOOSE/SELFISH`��`QUEUE::FIFO/LIFO`��������Ϊ����������ʶ���������
- RFCT����֮ǰʹ��ȫ�ֱ�����`DO_IHAR` �ȣ���ʶ�� scheme ѡ��浽�¶����ö�����ͣ�`_MacProtocol`��`_RoutingProtocol`��`_HotspotSelect`����ȫ�ֱ�����`MAC_PROTOCOL`��`ROUTING_PROTOCOL`��`HOTSPOT_SELECT`������Ȼͨ�������в�����ֵ��


###### 2016-02-15  ��  *< 2.5.1 > < 2.5.2 >*


###### 2016-02-16  ��  *< 2.5.3 >*


###### 2016-02-18  ��  *< 2.5.4 >*


###### 2016-02-20  ��  *< 2.5.5 >*

- RFCT��ԭ�궨�� `DATA_GENERATE_RATE`��`BYTE_PER_CTRL` �� `BYTE_PER_DATA` ���� `CNode` �еľ�̬��Ա���� `CNode::DEFAULT_DATA_RATE`��`CNode::SIZE_DATA` �� `CGeneralNode::SIZE_CTRL`��
- NOTE��`CNode::SIZE_DATA` �� `CGeneralNode::SIZE_CTRL` �ı���̫Сʱ��HDC ��ƽ���ܺĽ�����ͨ DC ���󣻱���Խ�󣬸���Ч��Խ���ԣ�
- NOTE��`CNode::CAPACITY_BUFFER` �� `CNode::DEFAULT_DATA_RATE` �ı�����С��Ͷ���������½�������Խ��Ͷ��������������


###### 2016-02-21  ��  *< 2.5.6 >*

- FIX��`energy-consumption.txt` ���������Ϊ���� x �������򣩣��޸� `CNode::getAllNodes()` ��ʹ�õ��Ľڵ���������


###### 2016-02-23  ��  *< 2.5.7 >*

- OPT��<���������Ĳ���> ���ֽڵ������ľ�������ʱ����Ӧ����ȫ���ڵ�������Ϊָ�꣬Ӧ����Ͷ�ݼ���Ϊָ�ꣻ


###### 2016-02-23  ��  *< 2.5.8 >*

- FIX��<���������Ĳ���> ���ƽ���ܺ�ʱ��Ӧ�������ڵ���ܺ�Ҳ���룻


###### 2016-02-23  ��  *< 2.5.9 >*

- OPT��<���������Ĳ���>��������ʱ���Ƿ񳬳��켣�ļ���Χ��ʼ������Ͷ�ݼ���Ϊָ�ꣻ
- RFCT��������С����� n λ�Ĳ��������� `NDigitFloat`������ `Global.h`��
- OPT��<���������Ĳ���> ��Բ��ֽڵ�������������޸� `buffer-node.txt` �� `buffer-node-statistics.txt` �ļ��е��������֤�ڵ���Ϣ��ǰ���Ӧ����ʹ�� `-` ��������ڵ㣻


###### 2016-03-01  ��  *< 2.5.10 >*

- RFCT���������ļ���
	* `CProcessor` ��Ϊ `CHelper`��
	* �����и���������������ȫ�֣�`Global.h`����`CFileHelper`��`CSortHelper` ������
	* `CRoute` ��̳��� `CGeoEntity`��


###### 2016-03-02  ��  *< 2.5.11 >*

- RFCT������������
- RFCT��ɾ������ `using namespace std;` ��ʹ�ã���Ϊ `using cin/ cout/ endl/ string/ ...;`��
- RFCT���޸Ĳ��ֺ�����������Ϊ�����������ʵ�ֶ�̬���Ա������ָ
- MNT���� Resharper �����õ������ļ� `HDC.sln.DotSettings`������ӵ�׷���б�


###### 2016-03-03

- MNT���� `README.md` �еĸ�����־�м��Ϸ����ǩ���μ� *[Log Mark](#log-mark)* ��
- MNT���� `README.md` ����� Environment ˵����
- MNT������ͼ�ļ� `ClassDiagram.cd` ��ӵ�׷���б�


###### 2016-03-04

- MNT������ `README.md` �ĵ���ʽ��


### 2.6.* : ���ع�


###### 2016-03-07  ��  *< 2.6.0 >*  ��  *���ع�*

- RFCT���� `Global.h` �е�ȫ�ֱ����������������ھ�̬������
- RFCT�����ڷ��͡����ա�����ѡ��Ķ����ʹ�ã��� `Mode`��`SEND`��`RECEIVE`��`QUEUE` ���ඨ���Ϊ���ڵ�ö���ඨ�� `CGeneralNode::_Send`��`CGeneralNode::_Receive`��`CGeneralNode::_Queue`��
- RFCT�������ȵ����͵Ķ����ʹ�ã��� `TYPE_MERGE_HOTSPOT`��`TYPE_NEW_HOTSPOT`��`TYPE_OLD_HOTSPOT` �Ⱥ궨���Ϊ���ڵ�ö���ඨ�� `CHotspot::_TypeHotspot`��
- RFCT������ `CConfiguration` �����࣬Ϊ֮����Ӵ��ļ��ж�ȡ�������õĹ�����׼����
- RFCT�����������е����в������뵥���� `Run()` ������


###### 2016-03-08  ��  *< 2.6.1 >*

- MNT���Զ��� VS �� Resharper ��ݼ������������ļ� `HDC.vssettings` �У�������Ŀ׷���б�
- RFCT��������Ŀ�Ĵ���Ŀ¼�ṹ��


###### 2016-03-09  ��  *< 2.6.2 >*

- RFCT����ö��������ʽ��Ϊ `_ALL_UPPERCASE`��ϵͳ��Ϊ�궨���ʽ��Ϊ `_ALL_UPPERCASE_`��
- RFCT��������Ŀ�Ĵ���Ŀ¼�ṹ���� VS ���̺������ļ������� `/.project/` �ļ����£�Resharper �����õ������ļ� `/.project/resharper.DotSettings`��VS �����õ������ļ� `/.project/vs.vssettings`����


###### 2016-03-09  ��  *< 2.6.3 >*

- RFCT�����켣�ļ��� `.newlocation` ��Ϊ `.trace` �ļ��������� `/res/$DATASET$/` �ļ����У�
- ADD�����ݼ����� `DATASET` Ϊ�ɶ��Ʋ�������δ��Ӷ�ȡ���룩��
- FIX��֮ǰʹ�� `nodes.size()` �滻 `NUM_NODE` ʱ��ɵĴ���
- MNT���� git commit message ��ʹ�ÿ��ٱ�ǩ���������ģ��μ� *[Log Mark](#log-mark)* ��


###### 2016-03-10  ��  *< 2.6.4 >*

- ADD����ͬ�ڵ������ʱ�����ֵ���Բ�ͬ���� trace �ļ�������
- MOD�����������ڵ������ `CNode::deadNodes` �еĲ���������һ������ `CNode::ClearDeadNodes()`��
- MOD������ `CRoutingProtocol::UpdateNodeStatus()` ��Ҳ���ܷ����ڵ�ɾ���������е�·��Э���е� `CNode::hasNodes()` �ж��Ƶ��˲���֮��
- FIX�����ȵ�ѡȡ֮ǰ�����������ڵ�� position ��¼��`CHotspotSelect::CollectNewPositions()`����
- FIX���ȵ����������ٷֱȵ�ͳ�ƴ��ڵĹؼ��Դ���`CProphet::SendData()`����
- ADD��`CNode::encounterActive` ����ͳ����Ч�Ľڵ�����������


###### 2016-03-11  ��  *< 2.6.5 >*

- MOD��ƽ���ܺĵ�ͳ�ƣ����� byte Ϊ��λ��Ϊ�� frame Ϊ��λ��
- MOD������ɾ�ڵ�͸��½ڵ�λ�á�����״̬�Ȳ��������� MAC Э����У���ʱȫ������ `CHDC`����
- RFCT�������������Ϣ�Ĳ����ֱ����� MAC ���·�ɲ�ĸ���������У���ʵ�־����ܶ�����ã�
- ADD������ȫ�ֺ��� `Exit()`���������ʱ����Ĳ���ͳһ�������У�


###### 2016-03-16

- ADD������� `CSMac`������ MAC ��Ļ��������� `CHDC` ������������ `CSMac` �У�


###### 2016-03-16  ��  *< 2.6.6 >*

- FIX������·����� Mac ��� `PrintInfo()` �� `PrintFinal` ���������
- ADD���� `x` �� `y` ����������װ�� `CCoordinate` �ࣻ
- RFCT����ʱ���������ھ�̬�����ķ���Ȩ��ȫ��Ϊ `public`��������ʼ������������ `initConfiguration()`��
- FIX������ ref���޸� `SIZE_DATA` ���ֵΪ 250 �ֽڣ�`SIZE_CTRL` �ο�ֵΪ 10 �ֽڣ���ԭ `BEACON_SIZE` ��Ϊ `SIZE_HEADER_MAC`���ο�ֵΪ 8 �ֽڣ�


###### 2016-03-17  ��  *< 2.6.7 >*

- ADD��������ȫ�ֱ����͸�����������װ�������ռ� `namespace global`��


###### 2016-03-17

- ADD������� `CPacket`��`CCtrl`��`CFrame`���ͺ��� `CNode::broadcastFrame()`��`::transmitFrame()`��`::sendRTS()`��`::receiveRTS()`��`::sendCTS()`��`::receiveFrame()`��


###### 2016-03-18  ��  *< 2.6.8 >*

- ADD��������ͨ�ŵĲ������ϵ� `CMacProtocol::broadcastFrame()`��`::transmitFrame()`��
- ADD�������ݷ��͵���Ӧ�������ϵ� `CMacProtocol::receiveFrame()`���漰��·�ɲ�Ĳ����ύ����Ӧ·���ദ����ʱֻʵ���� Prophet ·�ɣ�`CProphet::bufferData()` �� `::selectDataByIndex`����


## 3.* : ��д MAC ��


###### [ 2016-03-18  ��  *< 3.0.0 >*  ��  *��д MAC ��* ]( b50107fb04bd0976fc54aa0a81e47b2f6d619e1b )

- ADD������¶���ĺ��� `CMacProtocol::broadcastFrame()`��`::transmitFrame()`��`::receiveFrame()`��`CProphet::bufferData()`��`::selectDataByIndex`��
- MOD��ɾ��ԭ�е� `CRoutingProtocol::SendData()` ϵ��ʵ�֣�
- NOTE����ʱֻʵ���� Prophet ·�ɣ�δ���ԣ�δ��д Epidemic ·�ɺ� HAR ·�ɣ�
        

### 3.1.* : ��д MAC ��


###### [ 2016-03-20  ��  *< 3.1.0 >* ]( 684137e5b575ebf6f1bffb06f44f48e9fa2d1db7 )

- RFCT����� `FILE_xxx` �� `PATH_xxx` ϵ��ȫ�ֱ��������ڹ���͸��ģ�
- ADD��������ļ��ĺ�׺��Ϊ `.log`����ͬ���н���������עʱ����Ĳ�ͬ�ļ����£��� `/log/.xxxx-xx-xx xx:xx:xx`�����������֮��ȥ�� `.` ǰ׺��
- MOD�����յ�����ļ��� `debug.txt` ����Ϊ `final.log`�������������ļ����У�����������������˳�ʱ����
- ADD�������е��˳���ǲ�ͬ�Ĵ��룬���� 0 Ϊ������������˳���С�� 0 Ϊδ��ʼ���е�ֱ���˳������� 0 Ϊ������룻
- FIX�����������µ� MAC �㺯�����޸�һЩС����
- OPT���滻�����в���Ҫ��ǰ�õݼӲ����� `++var`��


###### [ 2016-03-21  ��  *< 3.1.5923.38461 >* ]( 04ed66ee1958827e8aa75e74a98444c8224e65fc )

- ADD��������ɰ汾�������ļ� `AssemblyInfo.cpp` �� `HDC.rc`�����޸���ص���Ŀ����Ϊ��ʹ�ù�����������ʱ֧��(/clr)���������߳� DLL(\MD)�������ڹ��� DLL ��ʹ�� MFC����
- MNT��ʹ�� VS ��� Versioning Controlled Build ���а汾�Ź���
- MNT������ MS Ĭ�ϵİ汾�Ź��򣬱���ź��޶������������ں�ʱ�������ͬʱ����Կ�ִ���ļ�д���������Ϣ��
- ADD��ʹ�� `ERRONO` ��ǳ���ķ���ֵ�����Զ�����չ������룻
- FIX��`final.txt` ���������ļ��г���
- FIX������ļ���������ʧ�ܣ�


###### [ 2016-03-21  ��  *< 3.1.5924.1169 >* ]( 2ad5fe57cc9d76fddfc53bf1a0894519cf3d6906 )

- ADD������������еĿ���̨�����
- FIX��`CFrame` �๹�캯���Ĵ���
- FIX������ `CData::ID_MASK`���޸���˲����Ĵ���


###### [ 2016-03-21  ��  *< 3.1.5924.21445 >* ]( cbd00e950480529860836007f447394a1ef32295 )

- TRY��ΪͶ�ݸ��������ݲ� `TOLERANCE_PRED`��ʹ�ú궨�� `USE_PRED_TOLERANCE` ���ã����ԣ����ƻ��ݶȣ�����ɾ������
- MNT��������ͳһ `README.md` �ĸ�ʽ��
- TEST��Ͷ�ݸ����ݲ


###### [ 2016-03-22  ��  *< 3.1.5925.19658 >* ]( 7de8dc84a46985d1fd04ec8201f5ca4ef9d143d9 )

- ADD����������в��� `-pred-tolerance`��`-log-path`��`-log-slot`��
- FIX��ģ���ຯ�� `GetItemsByID()` ���ܲ��ܳɹ����ã�
- FIX��`CProphet::selectDataByIndex()` �Ĵ���
- FIX��`CMacProtocol::receiveFrame()` �ĵ�������


###### [ 2016-03-22  ��  *< 3.1.5925.29760 >* ]( c95ff6cce1eefcd8145a1272ba578d4ad41c4c6e )

- OPT�����з����쳣�˳�ʱ�����������Ϣ���ļ� `error.log`��
- OPT���Ż������в�����ʹ�ð�����Ϣ��


### 3.2.* : RTS �Ӵ� DP


###### [ 2016-03-23  ��  *< 3.2.5926.22497 >* ]( af30b1aed1c80c2f909c74a2add441cc26089bb6 )

- RFCT���淶������������ getter ��������
- FIX�����Ľڵ��Ƿ񽫽����ھӽڵ㷢�ֵķ����� ��ӳ�Ա���� `CNode::discovering`�����޸� `CMacProtocol::broadcastFrame()` �нڵ����������ݴ�������ظ���������⣻
- FIX���������ݴ�������ļ��㷽���������ڵ��յ�һ�����ݾ���Ϊ��һ�����ݴ���ɹ���
- ADD����ӳ�Ա���� `CNode::encounterActiveAtHotspot`���������������ļ��㷽����
- ADD����ӽڵ㹤��״̬�����ʼ���Ŀ�ѡ���ܣ��ɱ��� `CMacProtocol::DC_SYNC` ��ǣ������в��� `-dc-sync`��


###### [ 2016-03-23  ��  *< 3.2.5926.22866 >* ]( bbdcaf90421a36b0b3ff97a9b8cd2c6982d41acb )


###### [ 2016-03-23  ��  *< 3.2.5926.30658 >* ]( 3ee22ba30ad1cc21b07f785f013339f7368d49f4 )

- FIX��ȡ�� `USE_TOLERANCE_PRED`��
- FIX���޸���©���� `CNode::updateDeliveryPredsWithSink()` ����Ҫ����


###### [ 2016-03-23  ��  *< 3.2.5926.31165 >* ]( 57912b77acaf0d9ce19d470ba6c106acaf92d294 )


###### [ 2016-03-24  ��  *< 3.2.5926.41581 >* ]( 6dab843a5108b472b1f642e88797d1253aa40ed8 )

- FIX���޸� `CMacProtocol::receiveFrame` �е����ݴ�����̣����� RTS ʱֱ���Ӵ�Ͷ�ݸ��ʣ�
- FIX���޸� `CMacProtocol::receiveFrame` �е����ݴ����������������ÿ���ڵ��ֻ����һ�����ݴ��䣬���յ� ACK Ϊ׼��


###### [ 2016-03-24  ��  *< 3.2.5927.16957 >* ]( dc6a2d7dcfde527f1c85a7201a5c2ff41e2f3440 )

- FIX���޸���Ŀ���ԣ���������������в��� `/NODEFAULTLIB:MSVCRT`���Խ�� `Warning LNK4098`��
- ADD���궨�� `_WIN32_WINNT`������Ϊ������ָʾϵͳ�汾��
- FIX���޸� `CMacProtocol::receiveFrame` �е���Ӧ�߼�����Ҫ����
- NOTE��ֻҪ·��Э������ת�����ݣ��ͽ����� DATA �� NODATA �����յ������е��κ�һ������Ӧ�ظ� ACK��
- ADD����ӳ�Ա���� `CProphet::TRANS_STRICT_BY_PRED`��ָʾ�Ƿ��ϸ���Ͷ�ݸ���**��**���ڱ��ڵ�Ϊ�ж�׼��
- ----- ��Ĭ�ϣ�ֵ `true` ʱ���������Ͷ�ݸ��ʵĽڵ�Ҳ�����ͣ����Ա��ֵ��򣩣�ֵΪ `true` ʱ���������Ͷ�ݸ��ʵĽڵ㣬���� 0.5 �ĸ��ʾ������ͣ�
- ADD����������в��� `-node` ���ڶ���ڵ������
- RFCT����Ӻ��� `CBasicEntity::withinRange()` �滻���� `CBasicEntity::getDistance()` �����ã�


###### [ 2016-03-24  ��  *< 3.2.5927.20040 >* ]( fde3f0dc45bc1ca95317f803663aacc46b9bce92 )

- RFCT�������ݰ���Ӧ�߼��� `CMacProtocol::receiveFrame()` �о������뵽 `CProphet::receivePackets()` �У�


###### [ 2016-03-25  ��  *< 3.2.5928.23405 >* ]( 7fa4d90b398c905de325e4ab0e20e3515e50ab21 )

- RFCT���� `CProphet::receivePackets()` �е���Ӧ��������Դ-Ŀ�Ľڵ�Ե����ͣ��������ϵ��������ص�ͬ�������У�
- FIX������ `CMacProtocol::CommunicateWithNeighbor()` ����Ϊ `CMacProtocol::CommunicateWithNeighbor()`�����޸��������ô���
- FIX���޸� `CProphet::receivePackets()` �еĴ���
- OPT���� `CData::MAX_HOP` �� `CData::MAX_TTL` �ĳ�ͻ����Ϊ�� `ParseConfiguration()` �м�飬���г�ͻ�������˳���
- BUG�����ݴ���������ڲ��Ӧ��� `CMacProtocol::transmitTry()` �� `CMacProtocol::transmitSucceed()` �ĵ��ã�


###### [ 2016-03-28  ��  *< 3.2.5931.21338 >* ]( dac9dd3379f583fc8ef510c6471ceeccdceaa514 )

- BUG�����ݴ�������Ĳ��
- RFCT�����ȵ�ѡȡ���������������뵽 `CHotspotSelect` ���У�
- ADD�����汾��Ϣ���Ƶ���־�ļ����У�`HDC.version`����
- ADD����� XML ������ TinyXML2 ������ļ� `/lib/xml/tinyxml2.h & .cpp` �� `CXmlHelper` �ࣻ


###### [ 2016-03-29  ��  *< 3.2.5932.17811 >* ]( eecbc4b00b64c8bc1dee0d062a53541dd30826ab )

- ADD����� `_HOTSPOT_SELECT::_none` ���ڶ��������ȵ�ѡȡ�����Ĺ��ܣ�����ʱû����Ӷ����Ľڵ��Ƿ�λ���ȵ��еļ�������
- ADD���ڲ�������������쳣������������в������ڴ������ֹ���У�
- ADD����������в��� `-window-trans`����ֵ Prophet ·���е�������ת����������ݰ���Ŀ��
- ADD����д xHAR ·�ɵ�����ͨ�����̣����� `HAR::receivePackets()` ����ɣ�
- BUG��Prophet ·���е�������ת����������ݰ���Ŀ�������н�������Ӱ�죻


### 3.3.* : ��д xHAR


###### [ 2016-03-30  ��  *< 3.3.5933.21567 >* ]( e1d4990c1d264bf4ac82eb1db3b135019e15ba32 )

- ADD����ɺ��� `HAR::receivePackets()`�������ԣ���
- RFCT���� `CHDC::UpdateDutyCycleForNodes()` �и��½ڵ������ȵ�����Ĳ�����ȡ�� `CHotspot::UpdateAtHotspotForNodes()`��
- ADD���� `CSmac::Operate()` ����Ӷ��������ȵ�ѡȡ�Ĳ�������Ӷ����Ľڵ��Ƿ�λ���ȵ�����ļ�������
- REM����ʱɾ������ Epidemic ·�ɵ���ض��塢���������ã����� `-ttl` �� `-queue` ��ʹ�ã�
- ADD������ MA �ڵ㣬���·�߹��ڻ򻺴�������_selfish ģʽ�£����������� sink��
- ADD����� Prophet �� HAR ·���ж� _selfish ģʽ��֧�֣�
- ADD����������в��� `-buffer-ma`������ `-buffer` Ĭ��ֵ��Ϊ 0������ѡ������


###### [ 2016-03-31  ��  *< 3.3.5934.19858 >* ]( 7dd414d67b6620cec73fea917d15b7a6a58a62b2 )

- ADD����� `sink.log` �������
- RFCT�����ڵ������������ȵ���������������ͳ������ `CHotspot::UpdateAtHotspotForNodes()` �У�ͳ��ʱ�۽��������������ȣ������ƶ�ģ�����Ⱦ�������


###### [ 2016-04-04  ��  *< 3.3.3 >* ]( c8d9fd949928f8ac63a335f9db8bd8f101e3f174 )

- MNT�����µ� Visual Studio 2015����Ŀ�ļ��������ļ���Ӧ���£�
- MNT��ֹͣʹ�� Resharper����ʱ���������ļ���
- MNT��ֹͣʹ�� Versioning Controlled Build����ʱ�Ƴ� `AssembllyInfo.cpp` �� `HDC.version`��
- MNT������ `version.h` �� `HDC.rc` ʵ�ְ汾�ſ��ƣ��ӻ���ϵͳʱ����Ӱ汾�Ÿ�Ϊ�����Ӱ汾�ţ���Ҫÿ���ֶ��޸İ汾�ű�����`version.h` �У���


###### [ 2016-04-06  ��  *< 3.3.4 >* ]( b88c685d4f91ada83d5f2f677af0d7d6fd0e361b )

- FIX��ָ���ظ��ͷŵĴ����������


###### [ 2016-04-08  ��  *< 3.3.5 >* ]( 1d48ec73413cc02785ab79edcca4354b920c028a )

- OPT�������������ͳ�Ƽ�������ض������� `CMacProtocol` ���ڣ�����������Ч���ݽ���ʱ����Ϊ����ɹ���
- OPT���Ż� `final.log` �ļ��еĲ��������
- REM��ɾ��Ͷ�ݸ����ݲ�����ж�������ã�
- MNT���Ż���Ŀ�����У������ļ����м��ļ�Ŀ¼�����ã�


### 3.4.* : ģ�������켣


###### [ 2016-04-12  ��  *< 3.4.1 >* ]( 4ba7d15999a41b34c0ef83a429d5f768888e00a8 )

- ADD���� `CCTrace`��`CTrace` Ϊ�����֣����Լ�ģ�������켣�Ĺ��ܣ��� `CCTrace::CONTINUOUS_TRACE` ָʾ��
- RFCT����ԭ `CFileHelper` ���еĹ켣�ļ���ȡ���������� `CCTrace` ���У�
- ADD�������� `CCoordinate` ��������������
- FIX���ȵ�ѡȡ�У�λ�õ���Ϣ�ռ��ĵ���ʱ�۴���


###### [ 2016-04-13  ��  *< 3.4.2 >* ]( 6bb139e98ddd1aa949b4aa0a550008897153d96e )

- MNT���Ż� `README.md` �� `HELP.md` �������ʽ��
- MNT���������в���ʹ��˵����ȡ���ļ� `help.md`�����°汾��Ϣ�ļ� `HDC.version` �������


###### [ 2016-04-14  ��  *< 3.4.3 >* ]( 3e63540c509120214faebe500e21ef2dd5ee5743 )

- RFCT������·����� MAC ��Э��Ĳ��������������߼������� `CMacProtocol::Prepare()` ������
- FIX��`CCTrace::addLocation()` �е����ͳ�������
- ADD��`-continuous-trance` ѡ�
- TEST���ȵ�ѡȡ�У�λ�õ���Ϣ�ռ��ĵ���ʱ�۴�����޸���
- TEST��`CCTrace` ���еĹ켣�ļ���ȡ�������Լ� `CHotspotSelect::CollectNewPositions()` �е�λ�õ���Ϣ�ռ�������


###### [ 2016-04-15  ��  *< 3.4.4 >* ]( 7241339d229cc7643ddb4c3998cb4206c30f6222 )

- FIX��VS 2015 �ĵ��������ӻ���������ԭ��������ʱ���� VS 2012 ��ƽ̨���߼��������ɺ͵��ԣ�
- MNT���� `HELP.md` ��ӵ�׷���б�


###### [ 2016-04-15  ��  *< 3.4.5 >* ]( c4335ddba90278c908b2629c014dde90f6c7db16 )

- OPT�������в��������ؽ��в�ȫ�ͱ�׼����
- RFCT������ `global` �����ռ��µĶ����ʵ�֣�
- FIX���޸�`hotspot-statistics.log` �е����������Ӹ��Ǳ����������
- RFCT���ȵ�ѡȡ�����У��� `CHotspot` ���ڱ����Ķ�д���Լ���غ���������
- FIX��`parameters.log` �е������
- FIX��ʹ�� HAR ·��Э��ʱ��log �ļ�ͷ�������©��
- FIX��`error.log` �ļ����ܴ�������� `/log/` Ŀ¼�£��� log ·����ʼ���Ĳ�����ȡ������ `InitLogPath()`��
- ADD����ʹ�õ������в�������� `command.log`��



> *** ����Ϊ HDC ��֧������״̬ ***

> *** �ϲ��� [ 2015-12-04 �� master ��֧ ]( 0dd0e561bf88035d1cfc557e2083dfa602ab9d43 ) ***

> *** ����Ϊ master ��֧�µĸ��� ***



### 3.5.* : �޸��ȵ�ѡȡ

��
###### [ 2016-05-18  ��  *< 3.5.1 >* ]( 169dcb4d55408936ef0f88e45e03f931bf81dbd0 )

- MNT���� HDC ��֧�����´���ϲ��� master ��֧��
- MNT�����������������������ڵ��ļ� `main.cpp` ������Ϊ `Run.cpp`��
- MNT������ `Global.h`��
- RFCT������������ `#ifndef __XXX_H__` ��������ͷ�ļ����壻 
- MNT�����Ŀ¼ `/demo/`����� demo ���뼰��Ԫ���Ժ�����
- ADD����� WGS84 ����� UTM �����ת�����ܣ�
- ADD����Ӷ�ȡ�켣�ļ���ͳ�ƽڵ��˶���Χ�Ĺ��ܣ�


###### [ 2016-05-26  ��  *< 3.5.3 >* ]( 8c2c036518fea21caa5172ab9891187b08eb5576 )

- FIX���޸� `CNode::updateStatus()` ���ھӽڵ���ʱ�۵Ĵ��󣬲�ʹ��ռ�ձȹ���ʱ����`CNode::discovering` ʼ�����棬���� `CNode::DEFAULT_DISCOVER_CYCLE` Ĭ��ֵΪ 0��
- ADD�������־�ļ� `hotspot-details.log` ����ѡ���ȵ��λ����Ϣ��


###### [ 2016-06-08  ��  *< 3.5.4 >* ]( d82ef12b88e183e65d02f341aad33ba7e3b1713d )

- OPT��`DEFAULT_SLOT_DISCOVER` �ȱ�������Ϊ `DEFAULT_SLOT_CARRIER_SENSE`��ָʾ�ڿ�ʼ�ھӽڵ㷢��֮ǰ�ز�������ʱ�䣻
- NOTE����ʹ��ռ�ձȹ���ʱ��`-cycle` ������ָʾ�ھӽڵ㷢�ֵ����ڣ�
- OPT����д `CNode::updateStatus()` �й���״̬�ĸ��£�
- MOD��Ĭ��ʹ������ĳ�ʼ����״̬���ڵ㹤�����ڵ�Ĭ��ֵ��Ϊ 30��
- FIX���޸� `CProphet::CAPACITY_FORWARD` Ĭ��ֵΪ 20�� ���޸��ڵ��� sink Ͷ��ʱ���ܸ�ֵ���ƵĴ���


###### [ 2016-06-08  ��  *< 3.5.5 >* ]( 93e7633d595d1d3f4f196a18922729398e84f524 )

- FIX���޸� `CNode::updateStatus()` �У������ʼ�޷���ȷ��ȡ�ڵ�λ�õĴ���
- FIX��`CBasicEntity::time` ��ʼֵ�� 0 ��Ϊ -1��
- FIX���޸� `CPostSelect::PostSelect` �е��ȵ�ѡ��˳�����
- FIX���޸� `CMANode::updateStatus()` �еĴ���
- FIX��������� `typeid` �� `static_cast` ʹ�ø�Ϊ `dynamic_cast`������ `CBasicEntity` ��Ϊ����࣬��������ֱ�ӻ��ӵ�������ļ̳�������


###### [ 2016-06-15  ��  *< 3.5.6 >* ]( dc6407c76a5b3fe7c8ce9e9b5613c7ddb390e322 )

- ADD����Ӵ� `./default.config` ����Ĭ�ϲ����Ĺ��ܣ����������в���һ�𱣴浽 `config.log`��
- ADD��Ϊ `bool` �Ͳ�����������ӿ����͹رյĹ��ܣ�ʹ�� on / off ָʾ��
- MNT���Ƴ� TinyXML2 �⼰��ص� Helper �ࣻ


###### [ 2016-06-16  ��  *< 3.5.7 >* ]( 03472a7e891917adfae693ab78ef11e7f734e908 )

- FIX���޸� `config.log` �Ϳ���̨����еĴ���͸�ʽ���⣻
- OPT������ `ParseConfiguration()` �е��쳣����


###### [ 2016-06-17  ��  *< 3.5.8 >* ]( cd6bbef119fed4346a2dfc89b89bfe1a406080ed )

- RFCT���� Prophet ·����صĲ����ͺ����Ķ���� `CNode` ���� `CProphet` �У�
- RFCT�������ֶ� `CSink::getSink()` ��ֱ�ӵ��ø�Ϊʹ�ò������룬��׼���Զ� sink ��������䣻


### 3.6.* : ��д Duty Cycle

��
###### [ 2016-06-18  ��  *< 3.6.1 >* ]( 3847980b9e8418ffd494371839a95e00bcbc57d3 )

- ADD����� `timerSleep/Wake/CarrierSense` ����������ʱ���� `_STATE` ö���࣬������ʵ�� `CNode::updateStatus()`��
- ADD����� `CSorHelper::insertIntoSortedList()` �� `CNode::removeDataByCapacity()` �����������ݵ����򣬴�ȡ������ʱ��Ϊѹ������ʱ��
- ADD����� `CNode::SPEED_TRANS` �� `CRoutingProtocol::TIME_WINDOW_TRANS, getTimeWindowTrans()`��
- MOD�����ز�����ʱ���Ϊ�� `CRoutingProtocol::TIME_WINDOW_TRANS` ��ȡ���ֵ��

��
###### [ 2016-06-18  ��  *< 3.6.2 >* ]( 5fd3d9eee4e67cac6af7994a2008794f0bbf5e18 )

- ADD����ӻ���ʱ���ͳ�ƣ������ `activation.log`��
- RFCT������������Ķ��弰�̳нṹ���õ� `CGeneralData`��`CFrame`��`CPacket`��`CData`��`CCtrl`��
- RFCT��Ϊ���в�����ʵ�����ĳ�������Ӵ��������������������м̳й�ϵ���Ϊ `virtual`��
- FIX��ʹ�� `strtol()` �� `strtod()` ���� `atoi()` �� `atof()`, ���޸���©���ַ��������쳣����

��
###### [ 2016-06-20  ��  *< 3.6.3 >* ]( 428975682d95aac39fa558eeba51b96702a91a5e )

- ADD�����㲥�����������ϲ�������� `CGeneralNode::Overhear(), Occupy()`����ģ������ͳ�ͻ����δʹ�ã���
- ADD����� `CNode::calTimeForTrans()` �� `CMacProtocol::transmitFrame()` �еľ�̬���� `frameArrivals`����ģ��ʱ�ӣ���δʹ�ã���
- ADD����� `CNode::delayDiscovering(), startDiscoering()` ��Ϊ��Ӧ��������δʹ�ã���
- ADD��`node.log` �� `death.log`��
- RFCT���Ż�·���� `receivePackets()` �����ķ�����
- NOTE���������������Դ����ԣ���������������ע�͵����ύ��Դ����汾���ڲ��ԣ�


###### [ 2016-06-21  ��  *< 3.6.4 >* ]( ed6a20f1e5dcf7d7582639c4dd1e44d75b156f81 )

- ADD�����չ켣�ļ�ʱ��ͳ�ƽڵ����������������Ż������
- OPT������������־�ļ��ļ�¼ʱ�ۣ�


###### [ 2016-06-23  ��  *< 3.6.5 >* ]( 3d0571d020f95ea034b3af9ccec850095bde165a )

- RFCT����� `CRunHelper` �࣬�� `Run.cpp` �е�ȫ�ֺ����������У�
- RFCT�����ַ��������Ĳ�����ȡ�� `CRunHelper::ParseInt, Double, Bool, Token()` �������������쳣�׳���
- RFCT���� `CConfiguration` ������� `_TYPE_VALUE, _VALUE_CONFIGURATION` �Ͳ��ֳ�Ա�����Ķ��壻
- MNT���ļ� `Run.cpp` �������Ļ� `main.cpp`��


###### [ 2016-06-23  ��  *< 3.6.6 >* ]( 1de0e0a1e534164313f6319509ebef709abc33e8 )

- FIX���޸� `CNode::updateStatus()` �� `CMacProtocol::receiveFrame()` �еĴ���
- RFCT�����ַ��������Ĳ�����ȡ���µ� `CParseHelper` �ࣻ
- RFCT����� `CConfiguration::AddConfiguration()`������������������ã��������� `CRunHelper` ���ʣ�


###### [ 2016-06-26  ��  *< 3.6.7 >* ]( 25640030afbfc783c9da9bc8224a7134f19462a7 )

- RFCT����д `CConfiguration::ParseConfiguration()` ������
- RFCT����ȫ `CConfiguration::AddConfiguration()` �����е��ã�����ʵ��ԭ�е��������������ø�ʽ��
- MOD�������������� `CNode::DEFAULT_DATA_RATE` �Ķ���ӡ�����/�롱��Ϊ���ֽ�/�롱���������ñ������������ã��������µ����ø�ʽ��


###### [ 2016-06-29  ��  *< 3.6.8 >* ]( c5a214bcb4bb1e104b8e9064069d5c58b61fca2c )

- MNT��������Ŀ��Ϊ sim (simulation)��


###### [ 2016-06-29  ��  *< 3.6.9 >* ]( b5c87e97e9cc4f5b0d8029114f0d2b8ddd1755d7 )

- RFCT�������еĴ�����ʾ���� `Exit()` ��ֱ�ӵ��ø�Ϊʹ�� `throw` �׳��쳣������ `CRunHelper::Run()` ����Ӷ������쳣����
- RFCT���������в�������ת���Ĳ�����ȡ�ɺ��� `CConfiguration::getConfiguration()`��


###### [ 2016-07-08  ��  *< 3.6.10 >* ]( dd4dfaf5388abb429ada815f5a62e0db71856e78 )

- OPT���쳣���񼰴�����룻


###### [ 2016-12-23  ��  *< 3.6.11 >* ]( 45969492d599ff2f543ec78abcd1657059b262ba )

- FIX��`CParseHelper` �е��쳣�׳����룻
- FIX�������������� `CNode::DEFAULT_DATA_RATE` �Ķ���ӡ�����/�롱��Ϊ���ֽ�/�롱 �����Ĵ���
- FIX���� `CRunHelper::InitConfiguration` �в��������ϣ����޸���Ӷ������ȵ�ѡȡʱ�� `HOTSPOT_SELECT` Ĭ��ֵ��Ϊ `_none` ֮�󣬵������� HAR �� HDC ʱ�ȵ�ѡȡȱʧ�����⣻
- FIX��������Ϊ���ݵ��� MA �ڵ㼴���ڵ��� sink��


###### [ 2016-12-29 }( 19cc2c9d12a4f35410638a236b22a9db4ba72636 )

- MNT����������־���ִ� `README.md` �з��뵽�������ļ� `CHANGE.md`��


###### [ 2016-12-29 ]( 918bce08512d6d9d51f1162a1452f5fad3ded96f )

- ADD����� MA �ڵ�·����Ϣ�� `ma-route.log`��
- ADD��`CCoordinate::toString` �� `CRoute::toString`��


###### [ 2017-01-24 ]( 8d8a1f2c8ed83974e37a1defaf2a6b0740af511b )

- FIX: Wrong use of `CBasicEntity::moveTo()` ret value in `CMANode::updateStatus`, which leads to false arrival to waypoints;
- FIX: Bug in `CCTrace::getTraceFromFile()`, which leads to early death of nodes due to lack of trace entry;

###### [ 2017-01-24 ]( 82b2c533f2c4f268542d6c144568452cfcc351d3 )

- ADD: Delivery count & waiting time stat info printed to `hotspot-statistics.log` by `HAR::PrintInfo()`, which was once abandoned after [48b5f2](48b5f2fe380ebfa510da3f2e578984bd353894b6);
- MOD: Redefine `CHotspot::countsDelivery` & `CHotspot::waitingTimes` as `map<int, int>`;
- NOTE: Bugs may be introduced with merged hotspots in mHAR. Need to check if a merged hotspot would overwrite the old one;
- RFCT: Redefine `CHotspot::oldSelectedHotspots` as `map<int, vector<CHotspot*>>` to store all the previous hotspot selection, rather than just last one;
- RFCT: Extract access to `CHotspot::selectedHotspots, oldSelectedHotspots` into `CHotspot::getSelectedHotspots()`, store & clean before new selection into `CHotspotSelect::SaveOldSelectedHotspots()`;

###### [ 2017-01-24 ]( d442f8466ea046fa72062ddd73422f1085573677 )

- OPT: Minor improvement for some printings & removal of obsolete TODOs;

###### [ 2017-01-26 ]( b04463874887150c377fe83f718b42d48e225fdd )

- OPT: Minor improvement for path & filename string definitions in `global`;

###### [ 2017-01-26 �� *< 3.6.6234.36098 >* ]( 127ce48e7a110837e898f575c672952cc6b70246 )

- [x] MNT: Use VCB command line util `AUTOVER` for version control. & Allow switching back to manual versioning by undefining `__VERSIONING_CONTOLLED_BUILD__`;

###### [ 2017-01-26 �� *< 3.6.6235.24502 >* ]( 45ec30d8e864d3cc15562e38e4702ab2bfe058f0 )

- [x] FIX: Only 1 waypoint for each MA, which is caused by definition change of `CNode::DEFAULT_DATA_RATE`;

###### [ 2017-01-26 �� *< 3.6.6235.28166 >* ]( c24bad16eb016ceff17e3c3c91ad4507fdf9a8dd )

- FIX: And some opt for data communication between MA & sink in HAR;

###### [ 2017-01-26 �� *< 3.6.6235.29389 >* ]( f5b4ee1a57eedb9365677850a8f27d135b3fbee4 )

- ADD: Length limit to data transmission in HAR according to `CRoutingProtocol::WINDOW_TRANS`;

###### [ 2017-01-30 �� *< 3.6.6239.28044 >* ]( 624aaa84a9effa5e0b839526f2fcbc71ac8a6249 )

- [x] RFCT: Extract printing operations into `CPrintHelper`;

###### [ 2017-01-31 �� *< 3.6.6239.29279 >* ]( 2bc8ccd32cd2f91dfd3c4bd969114fb751757715 )

- FIX: Minor bug with hotspot statistics;

###### [ 2017-02-07 �� *< 3.6.6246.31032 >* �� *< v0.1.1 >* ]( df7497ae40d9c932d3fe5ded76f1fc452d8aea14 )

- MNT: Fix the bug with version control. Version No. in `sim.version` had always been 1 commit behind between [*<3.6.6234.36098>*](127ce48e7a110837e898f575c672952cc6b70246) ~ [*<3.6.6239.29279>*](2bc8ccd32cd2f91dfd3c4bd969114fb751757715);
- MNT: Product Version No. changed to `0.1.1`;

###### [ 2017-03-26 �� *< 3.6.6294.23218 >* ]( 8645d174020e323ad0022a2e3a50f9994a160fb9 )

- ADD: Implementation of **Trace Filter** based on the Rectangle Model in [On the Levy-walk Nature of Human Mobility](http://ieeexplore.ieee.org/document/4509740/).
  Original snippets written by �����, incompatible with the project for now;

###### 2017-03-26 �� *< 3.6.6294.29782 >*

- OPT: Rewrite `CTraceFilter` to be compatible with the project. Untested;


- [ ] ADD����Ӿ��溯�� `CRunHelper::Warn()`��
- [ ] ADD����� `timerTransmission` ��ָʾ�������ӵĿ�ʼ���Ͽ�����ʱ��
- [ ] ADD��������������ӶϿ������¿�ʼ�ھӽڵ㷢�ֵĲ�����
- [ ] ADD��������Ѱ�������ھӽڵ�ԵĲ�����ȡ�ɺ�����
- [ ] ADD����ӽڵ���ŵ���æ�ļ�⼰��Ӧ��
- [ ] ADD����Ӷ��ŵ���ͻ�ļ�⼰��Ӧ������ʧ�ܣ���
- [ ] ADD����Ӵ����ӳٵ�ģ�⣻
- [ ] RFCT�������й�����ز�����ȡ���ࣻ
- [ ] RFCT�����������ò����Ķ������ `CConfiguration` �ಢ���浽һ�����飬�Լ� `ParseConfiguration()` �еĲ�����
- [ ] ADD���Զ� sink ��������䣻
- [ ] FIX����� `CBasicEntity::time` ��ʼֵ�� 0 ��Ϊ -1 ���ܵ��µ��������⣻
- [ ] FIX��������� `typeid` ��ʹ�ã�
- [ ] FIX���Ա� [�����汾](48b5f2fe380ebfa510da3f2e578984bd353894b6) �ҳ� HAR ·�� MA �ͽڵ�ͨ�������д��ڵ����⣻
- [ ] RFCT�����ļ� `GPS.h` �� `MotionRange.h` �еĶ������� Helper �� ?
- [ ] RFCT�����ļ����뵽��������µĲ�ͬ����Ŀ ?
- [ ] RFCT������һ�������в������ڷ�����������Ƕ�������������� ?
- [ ] RFCT������ `UTMCoor` �ṹ��� `CCoordinate` ��Ķ�������ã�
- [ ] RFCT���� `MotionRange.h` �л�ȡָ��·�����ļ��б�Ĳ�����ȡ�� `CFileHelper` ���У������·��������ڣ�
- [ ] ADD�������ʼ���ڵ�Ĺ���״̬����ģ���첽��ռ�ձȣ������в��ԣ�
- [ ] ADD������켣�ļ��е�ʱ��Ϊ��̬�仯�ģ���Ҫ�� `CCTrace::getLocation()` ��������Ӧ�Ķ�ȡ���룻
- [ ] RFCT��ʹ���������������ͬЭ����ص����ڳ�Ա�ͺ����Ķ�������ã�
- [ ] RFCT�����ļ��в�����ȡ�� `CFileHelper` �ࣻ
- [ ] ADD����Ӵ����ӳٵĺ�ʱ���㣻
- [ ] ADD����ÿ���ڵ㴴��Ϊһ���̣߳����̱߳��ʵ��ʱ��ͬ����
- [ ] ADD���Խڵ㼯�ϲ���ȫ�ֵ� x ��������洢��ʹ�������Լ��ٶ��ױ�����������������������ȡ��ȫ�֣����ٲ���Ҫ���ظ�����������
- [ ] RFCT���� `Global.h` �� `main.cpp` �е�ȫ�ָ����������� `CRunHelper` ?
- [ ] RFCT�������ò�����صĶ���Ͳ������������� `CConfiguration` �ࣻ
- [ ] RFCT�������ò�����Ĭ��ֵ��ȡ���� XML ʵ�֣�
- [ ] MNT���� `README.md` �У���� trace �ļ��ĸ�ʽ��Ŀ¼λ��Ҫ���˵����
- [ ] RFCT�����Զ�ÿ��ͷ�ļ��������� dll������������Ŀȫ�����ɵ�һ����ִ���ļ���
- [ ] ADD��ʹ�� `__FILE__`��`__LINE__` ��Ԥ����꣬Ϊδ��׽���쳣���������Ϣ��
- [ ] ADD��Ϊ Release �汾��Ӿ����ܶ�ĺϷ��Լ�顢�쳣����������
- [ ] ADD����д Epidemic ·�ɵ�����ͨ�����̣�
