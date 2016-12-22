#pragma once

#ifndef __NODE_H__
#define __NODE_H__

#include "DutyCycle.h"


class CNode :
	public CDutyCycle
{
private:

	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���
								 //������
	//������
	static void newNodes(int n);
	static void removeNodes(int n);


protected:

	static vector<CNode &> nodes;  //���ڴ������д������ڵ�
	static vector<CNode &> deadNodes;  //�����ľ��Ľڵ�
	static vector<CNode &> deletedNodes;  //�����ݴ�Node������̬�仯ʱ����ʱ�Ƴ��Ľڵ�

	static void pushBackNode(CNode& node);
	static vector<CNode&>::iterator eraseNode(vector<CNode&>::iterator inode);


public:

	CNode();
	CNode(double dataRate) : CBasicNode(dataRate)
	{};
	virtual ~CNode();


	static bool checkInitialization();
	static bool assertInitialization();

	static vector<CNode &>& getNodes();

	static int getNNodes();

	//�����Ѿ�ʧЧ�Ľڵ��ɾ���Ľڵ㣬����ID����
	static vector<CNode &> getAllNodes(bool sort);

	static vector<int>& getIdNodes();

	//�ýڵ㲻����ʱ�޷���ֵ���Խ��������Ա�����ÿ�ε���֮ǰ���ú���ifNodeExists()���м��
	static CNode& getNodeByID(int id);

	static bool ifNodeExists(int id);

	static bool hasNodes(int currentTime);

	//�������ڵ������Ƴ�
	static void ClearDeadNodes(int currentTime);

	//���޶���Χ�������ɾһ��������node
	static int ChangeNodeNumber();

	static double getSumEnergyConsumption();


};

#endif // __NODE_H__
