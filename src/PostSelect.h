#pragma once

#ifndef __POST_SELECT_H__
#define __POST_SELECT_H__

#include "Hotspot.h"
#include "Process.h"


//HAR�еĺ���ѡȡ�㷨
class CPostSelect : 
	virtual public CAlgorithm
{
private:

	double maxRatio;
	vector<int> idNodes;
	vector<int> coveredNodes;
	vector<int> coveredPositions;
	vector<CHotspot*> unselectedHotspots;  //��ѡ���ϣ�ʹ��̰��ѡȡ�������ʼ��������ʱֵΪPostSelect��δ��ѡ�е�hotspot����
	vector<CHotspot*> selectedHotspots;  //PostSelectѡ�е�hotspot����

	static double ALPHA;

	/** �������� **/
	double getRatioForHotspot(CHotspot *hotspot) const;
	void updateForNewlySelectedHotspot(CHotspot *hotspot);
	vector<int> findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//���ѡȡ��hotspot�����Ƿ��ܸ�������node
	bool verifyCompleted();

	CPostSelect(vector<CHotspot *> selectedHotspots, vector<int> idNodes);
	~CPostSelect(){};

	void PostSelect();

public:

	static void Init();
	//ִ��hotspotѡȡ�����صõ���hotspot����
	static void PostSelect(vector<CHotspot *> &selectedHotspots, vector<CHotspot *> &unselectedHotspots, vector<int> idNodes);
};

#endif // __POST_SELECT_H__
