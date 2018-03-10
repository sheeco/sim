#pragma once

#ifndef __HOTSPOT_H__
#define __HOTSPOT_H__

#include <iostream>
#include "Configuration.h"
#include "Position.h"
#include "Node.h"


//存储hostspot信息的类
class CHotspot : 
	virtual public CBasicEntity, public CGeoEntity
{
	friend class CHotspotSelect;
	friend class CPostSelect;
	friend class CNodeRepair;
	//friend class HAR;
	//friend class CHDC;


protected:

	typedef enum EnumHotspotType {_new_hotspot, _old_hotspot, _merge_hotspot } EnumHotspotType;


	static map<int, CHotspot*> atHotspots;

	static int encounterAtHotspot;
	static int visiterAtHotspot;
	//热点区域所有可能的相遇（只与数据集和热点选取有关）

private:
	
	//int birth;  //hotspot的ID即birth，即起始position的ID
	vector<CPosition *> coveredPositions;  //覆盖列表
	vector<int> coveredNodes;  //覆盖的node列表，hotspot选取结束后手动调用generateCoveredNodes生成
	double heat;
	map<int, int> countsDelivery;  //存储该热点上的投递计数，连任的热点应对每一任期内的投递计数进行统计
	map<int, int> waitingTimes;  //存储该热点上的等待时间: key指示开始时间，value指示等待时长
	double ratio;  //用于测试新的ratio计算方法，将在贪婪选取和后续选取过程中用到

	static int COUNT_ID;

	//merge-HAR
	EnumHotspotType typeHotspotCandidate;  //用于标记热点候选类型，将在贪婪选取（和热度计算）时使用：旧热点 / 新热点 / 归并热点
	int age;  //用于标记旧热点或归并热点的年龄，即连任轮数

	//检查某个position是否已在覆盖列表中
	bool ifPositionExists(CPosition* pos);

	//从覆盖列表中删除某个position，只有贪婪算法会用到
	void removePosition(CPosition* pos);

	//计算两个热点的重叠面积，如无重叠则返回0
	static double getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot);

	void init();
	CHotspot()
	{
		init();
	}

	static CHotspot* generateHotspot(CCoordinate location, vector<CPosition*> positions, int time);


public:

	//从pos出发生成一个初始hotspot，并完成此候选hotspot的构建
	//time应当是当前time，而不是pos的time
	//由merge函数调用
	CHotspot(CCoordinate location, int time)
	{
		init();
		this->setLocation(location);
		this->setTime(time);
		this->generateID();
	}

	~CHotspot(){};

	//setters & getters
	inline int getNCoveredPosition() const
	{
		return coveredPositions.size();
	}
	inline vector<CPosition *> getCoveredPositions() const
	{
		return coveredPositions;
	}
	inline double getHeat() const
	{
		return heat;
	}
	inline void setHeat(double heat)
	{
		this->heat = heat;
	}
	//merge_HAR
	inline EnumHotspotType getTypeHotspotCandidate() const
	{
		return this->typeHotspotCandidate;
	}
	inline void setCandidateType(EnumHotspotType typeHotspotCandidate)
	{
		this->typeHotspotCandidate = typeHotspotCandidate;
	}
	inline int getAge() const
	{
		return this->age;
	}
	//FIXEME: 未测试
	inline void setAge(int age)
	{
		this->age = age;
		if( countsDelivery.find(this->time) == countsDelivery.end() )
			countsDelivery[this->time] = 0;
	}
	inline int getNCoveredNodes() const
	{
		return coveredNodes.size();
	}
	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}
	inline double getRatio() const
	{
		return ratio;
	}
	//新的ratio计算方法，将在贪婪选取和后续选取过程中用到，不使用-balanced-ratio时ratio==nCoverdPosition
	//注意：调用之前必须确保coveredNodes已得到更新
	double calculateRatio();

	//当覆盖列表发生更改时调用，更新坐标、覆盖节点列表和ratio
	void updateStatus()
	{
		recalculateCenter();
		generateCoveredNodes();
		calculateRatio();
	}

	int getNCoveredPositionsForNode(int inode);	
	
	static CHotspot* getAtHotspot(int nodeid)
	{
		if(!isAtWaypoint(nodeid))
			return nullptr;
		else
			return atHotspots[nodeid];
	}

	static void setAtWaypoint(int nodeid, CHotspot* atHotspot)
	{
		atHotspots[nodeid] = atHotspot;
	}

	static bool isAtWaypoint(int nodeid)
	{
		if(atHotspots.find(nodeid) == atHotspots.end())
			return false;
		else
			return atHotspots[nodeid] != nullptr;
	}
	static void encountAtHotspot()
	{
		++encounterAtHotspot;
	}

	static int getEncounterAtHotspot()
	{
		return encounterAtHotspot;
	}
	static double getPercentEncounterAtHotspot()
	{
		if(encounterAtHotspot == 0)
			return 0.0;
		return double(encounterAtHotspot) / double(CNode::getEncounter());
	}
	//	static double getPercentEncounterActive() 
	//	{
	//		if(encounterActive == 0)
	//			return 0.0;
	//		return double(encounterActive) / double(encounter);
	//	}

	//访问计数：用于统计节点位于热点内的百分比（HAR路由中尚未添加调用）
	static void visitAtHotspot()
	{
		++visiterAtHotspot;
	}
	static double getPercentVisiterAtHotspot()
	{
		if(visiterAtHotspot == 0)
			return 0.0;
		return double(visiterAtHotspot) / double(CNode::getVisiter());
	}
	static int getVisiterAtHotspot()
	{
		return visiterAtHotspot;
	}

	inline void recordCountDelivery(int n)
	{
		map<int, int>::iterator imap = countsDelivery.find(this->time);
		if( imap == countsDelivery.end() )
			countsDelivery[this->time] = n;
		else
			countsDelivery[this->time] = imap->second + n;
	}

	//最后这一任期内的投递计数
	//FIXEME: 未测试
	inline int getCountDelivery()
	{
		map<int, int>::iterator imap = countsDelivery.find(this->time);
		if( imap != countsDelivery.end() )
		{
			return imap->second;
		}
		else
			throw string("CHotspot::getCountDelivery() : Cannot find delivery count for time " + STRING(this->time) + ") !");
	}
	//返回( sinceTime, sinceTime + 900 )期间的投递计数
	inline int getCountDelivery(int sinceTime)
	{
		map<int, int>::iterator imap = countsDelivery.find(sinceTime);
		if( imap != countsDelivery.end() )
		{
			return imap->second;
		}
		else
			throw string("CHotspot::getCountDelivery(" + STRING(sinceTime) + ") : Cannot find delivery count for given arg !");
	}

	double getRatioByTypeHotspotCandidate() const;

	//在 CMANode::updateStatus 中，在每次等待结束waitingState重置时调用
	inline void recordWaitingTime(int startTime, int duration)
	{
		if( waitingTimes.find(startTime) != waitingTimes.end() )
			throw string("CHotspot::recordWaitingTime(" + STRING(startTime) + ", " + STRING(duration) + ") : "
						 "Record (" + STRING(startTime) + ", " + STRING(waitingTimes.find(startTime)->second) + ") has already existed !");
		waitingTimes[startTime] = duration;
	}
	//返回等待时间map，用于统计等待时间
	inline map<int, int> getWaitingTimes()
	{
		return waitingTimes;
	}
	string getWaitingTimesString(bool details)
	{
		stringstream sstr;
		int sum = 0;
		for( map<int, int>::iterator imap = waitingTimes.begin(); imap != waitingTimes.end();  )
		{
			if( details )
				sstr << imap->first << ":";
			sstr << imap->second;
			sum += imap->second;
			if( ++imap != waitingTimes.end() )
				sstr << ",";
			else
			{
				sstr << ";";
				break;
			}
		}
		sstr << "=" << STRING(sum);
		return sstr.str();
	}

	//自动生成ID，需手动调用，为了确保热点ID的唯一性，制作临时拷贝时不应调用此函数
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}
	string format() const
	{
		return "Hotspot " + this->location.format();
	}
	string getName() const
	{
		return "Hotspot " + STRING(this->ID);
	}

	//用于在ifExists和RemoveFromList等函数中作为判断参数传入
	inline static bool identical(CHotspot *x, CHotspot *y)
	{
		return x->getID() == y->getID();
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

	//为所有节点检查是否位于热点区域内，并统计visiter和encounter的热点区域计数
	//visit 和 encounter 计数的统计时槽仅由轨迹文件决定
	static bool UpdateAtHotspotForNodes(vector<CNode *> nodes, vector<CHotspot*> hotspots, int now);
//	//为所有MA节点检查是否位于热点区域内（用于xHAR）
//	static bool UpdateAtHotspotForMANodes(int now);
		
	/****************************************   merge-HAR   ****************************************/ 

	//计算两个热点集合中所有热点pair的重叠面积之和（如有三个或更多个热点共同重叠，重叠部分的面积将多次计入，以度量相似程度）
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//计算一个热点集合内的重叠面积，用于更准确的计算集合间重叠面积
	static double getOverlapArea(vector<CHotspot *> hotspots);

};

#endif // __HOTSPOT_H__

