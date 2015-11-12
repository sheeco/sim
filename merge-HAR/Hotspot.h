#pragma once

#include "Position.h"

extern int currentTime;
extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

//存储hostspot信息的类
class CHotspot: public CBase
{
private:
	
	//int birth;  //hotspot的ID即birth，即起始position的ID
	int nCoveredPosition;
	vector<CPosition *> coveredPositions;  //覆盖列表
	vector<int> coveredNodes;  //覆盖的node列表，hotspot选取结束后手动调用generateCoveredNodes生成
	double heat;
	vector<int> deliveryCounts;  //存储该热点上的投递计数，连任的热点应对每一任期内的投递计数进行统计
	static long int ID_COUNT;

	//merge-HAR
	int candidateType;  //用于标记热点候选类型，将在贪婪选取（和热度计算）时使用：旧热点 / 新热点 / 归并热点
	int age;  //用于标记旧热点或归并热点的年龄，即连任轮数

	//检查某个position是否已在覆盖列表中
	bool ifPositionExists(CPosition* pos);

	//从覆盖列表中删除某个position，只有贪婪算法会用到
	void removePosition(CPosition* pos);


	//检查某个node是否在覆盖列表中，必须在generateCoveredNodes之后调用
	bool ifNodeExists(int inode);	

	//计算两个热点的重叠面积，如无重叠则返回0
	static double getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot);

public:
	CHotspot()
	{
		this->nCoveredPosition = 0;
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
		this->deliveryCounts.push_back(0);
	}

	//从某个Position的位置生成一个hotspot
	CHotspot(CPosition* pos, int time)  //time应当是当前time，而不是pos的time
	{
		this->nCoveredPosition = 0;
		this->x = pos->getX();
		this->y = pos->getY();
		this->ID = -1;
		this->time = time;
		this->heat = 0;
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
		this->deliveryCounts.push_back(0);

		addPosition(pos);
	}

	~CHotspot(){};

	//setters & getters
	inline void setNCoveredPosition(int nCoveredPosition)
	{
		this->nCoveredPosition = nCoveredPosition;
	}
	inline int getNCoveredPosition()
	{
		return nCoveredPosition;
	}
	inline vector<CPosition *> getCoveredPositions()
	{
		return coveredPositions;
	}
	inline double getHeat()
	{
		return heat;
	}
	inline void setHeat(double heat)
	{
		this->heat = heat;
	}
	inline static bool identical(CHotspot *x, CHotspot *y)
	{
		return x->getID() == y->getID();
	}

	//merge_HAR
	inline int getCandidateType()
	{
		return this->candidateType;
	}
	inline void setCandidateType(int candidateType)
	{
		this->candidateType = candidateType;
	}
	inline int getAge()
	{
		return this->age;
	}
	inline void setAge(int age)
	{
		this->age = age;
	}
	inline int getNCoveredNodes()
	{
		return coveredNodes.size();
	}
	inline vector<int> getCoveredNodes()
	{
		return coveredNodes;
	}

	int getNCoveredPositionsForNode(int inode);	
	
	double getRatioByCandidateType()
	{
		switch( this->candidateType )
		{
			case TYPE_MERGE_HOTSPOT: 
				return RATIO_MERGE_HOTSPOT;
			case TYPE_NEW_HOTSPOT: 
				return RATIO_NEW_HOTSPOT;
			case TYPE_OLD_HOTSPOT: 
				return RATIO_OLD_HOTSPOT;
			default:
				return 1;
		}
	}

	//从当前这一热点任期内的投递计数，该函数应当在MA的路径更新时调用输出统计结果
	inline int getDeliveryCount()
	{
		while( deliveryCounts.size() < ( ( currentTime - time ) / SLOT_HOTSPOT_UPDATE + 1 ) )
			deliveryCounts.push_back(0);
		if(age == 0)
			return deliveryCounts.at(0);
		else
			return deliveryCounts.at( ( currentTime - time ) / SLOT_HOTSPOT_UPDATE - 1 );
	}

	inline void addDeliveryCount(int n)
	{
		while( deliveryCounts.size() < ( ( currentTime - time ) / SLOT_HOTSPOT_UPDATE + 1 ) )
			deliveryCounts.push_back(0);
		deliveryCounts.at( deliveryCounts.size() - 1 ) += n;
	}

	//自动生成ID，需手动调用
	inline void generateID()
	{
		//if(this->ID != -1)
		//	return;
		this->ID = ID_COUNT;
		ID_COUNT++;
	}

	//从覆盖列表中删除多个position，只有贪婪算法会用到
	void removePositionList(vector<CPosition *> positions);

	//将一个position加入此hotspot的覆盖列表
	void addPosition(CPosition* pos);

	//根据包含的position重新计算重心
	//修改覆盖列表之后不会自动调用，应该在需要的时候手动调用
	void recalculateCenter();

	//确定覆盖的node列表，在hotspot选取结束后手动调用
	void generateCoveredNodes();

	//生成包含该热点的时间、年龄、ID、坐标、cover数等信息的字符串
	string toString(bool withDetails);
	
	//计算两个热点集合中所有热点pair的重叠面积之和（如有三个或更多个热点共同重叠，重叠部分的面积将多次计入，以度量相似程度）
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//计算一个热点集合内的重叠面积，用于更准确的计算集合间重叠面积
	static double getOverlapArea(vector<CHotspot *> hotspots);

};


