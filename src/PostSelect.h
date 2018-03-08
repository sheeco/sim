#pragma once

#ifndef __POST_SELECT_H__
#define __POST_SELECT_H__

#include "Hotspot.h"
#include "Position.h"
#include "Algorithm.h"


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

	static void Init()
	{
		if(ALPHA == INVALID)
			ALPHA = getConfig<double>("hs", "alpha");
	}
	//ִ��hotspotѡȡ�����صõ���hotspot����
	static void PostSelect(vector<CHotspot *> &selectedHotspots, vector<CHotspot *> &unselectedHotspots, vector<int> idNodes)
	{
		Init();

		CPrintHelper::PrintDoing("POST SELECT");

		CPostSelect selector(selectedHotspots, idNodes);
		selector.PostSelect();
		selectedHotspots = selector.selectedHotspots;
		unselectedHotspots.insert(unselectedHotspots.end(), selector.unselectedHotspots.begin(), selector.unselectedHotspots.end());

		CPrintHelper::PrintDoing(STRING(selectedHotspots.size()) + " hotspots");
		CPrintHelper::PrintDone();
	}
};

#endif // __POST_SELECT_H__
