#pragma once

#include "Position.h"
#include "GeoEntity.h"

extern bool TEST_BALANCED_RATIO;
extern bool TEST_LEARN;
extern int NUM_NODE;
extern int TRANS_RANGE;
extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

using namespace std;

//存储hostspot信息的类
class CHotspot : 
	public CGeoEntity
{
private:
	
	//int birth;  //hotspot的ID即birth，即起始position的ID
	vector<CPosition *> coveredPositions;  //覆盖列表
	vector<int> coveredNodes;  //覆盖的node列表，hotspot选取结束后手动调用generateCoveredNodes生成
	double heat;
	vector<int> deliveryCounts;  //存储该热点上的投递计数，连任的热点应对每一任期内的投递计数进行统计
	vector<int> waitingTimes;  //存储该热点上的等待时间，连任的热点应对每一任期内的投递计数进行统计
	double ratio;  //用于测试新的ratio计算方法，将在贪婪选取和后续选取过程中用到

	static int ID_COUNT;

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

	void init()
	{
		this->heat = 0;
		this->waitingTimes.push_back(0);
		this->ratio = 0;		
		
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
		this->deliveryCounts.push_back(0);		
	}

	void generateHotspot(double x, double y, int time)
	{
		init();
		this->setX(x);
		this->setY(y);
		this->setTime(time);
		this->generateID();

		//重置flag
		for(int i = 0; i < CPosition::nPositions; i++)
			CPosition::positions[i]->setFlag(false);

		bool modified;
		//循环，直到没有新的position被加入
		do
		{
			modified = false;
			//对新的hotspot重心，再次遍历position
			for(int i = 0; i < CPosition::nPositions; i++)
			{
				if(CPosition::positions[i]->getFlag())
					continue;
				if( CPosition::positions[i]->getX() + TRANS_RANGE < this->getX() )
					continue;
				//若水平距离已超出range，则可以直接停止搜索
				if( this->getX() + TRANS_RANGE < CPosition::positions[i]->getX() )
					break;
				if(CBasicEntity::getDistance(*this, *CPosition::positions[i]) <= TRANS_RANGE)
				{
					this->addPosition(CPosition::positions[i]);
					CPosition::positions[i]->setFlag(true);
					modified = true;
				}
			}

			//重新计算hotspot的重心
			if(modified)
				this->recalculateCenter();
		}while(modified);
		
	}

	CHotspot()
	{
		init();
	}


public:

	//以下公有静态变量是从原来的g_系列全局变量移动到此处的，所有原来的引用都已作出替换
	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> selectedHotspots;
	//上一次贪婪选取最终得到的热点集合，保留
	//注意：merge操作得到的输出hotspot应该使用CHotspot::hotspotCandidates中的实例修改得到，不可保留对CHotspot::oldSelectedHotspots中实例的任何引用，因为在merge结束后将被free
	static vector<CHotspot *> oldSelectedHotspots;
	static vector<CHotspot *> deletedHotspots;


	//从pos出发生成一个初始hotspot，并完成此候选hotspot的构建
	//time应当是当前time，而不是pos的time
	CHotspot(CPosition* pos, int time)
	{
		addPosition(pos);
		generateHotspot(x, y, time);
	}

	//由merge函数调用
	CHotspot(double x, double y, int time)
	{
		generateHotspot(x, y, time);
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
	//用于在ifExists和RemoveFromList等函数中作为判断参数传入
	inline static bool identical(CHotspot *x, CHotspot *y)
	{
		return x->getID() == y->getID();
	}

	//merge_HAR
	inline int getCandidateType() const
	{
		return this->candidateType;
	}
	inline void setCandidateType(int candidateType)
	{
		this->candidateType = candidateType;
	}
	inline int getAge() const
	{
		return this->age;
	}
	//FIXEME: 未测试
	inline void setAge(int age)
	{
		this->age = age;
		while( deliveryCounts.size() < age + 1 )
			deliveryCounts.push_back(0);
		while( waitingTimes.size() < age + 1 )
			waitingTimes.push_back(0);
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
	double calculateRatio()
	{
		if( TEST_BALANCED_RATIO )
		{
			ratio = coveredPositions.size() * static_cast<double>(NUM_NODE - coveredNodes.size() + 1) / static_cast<double>(NUM_NODE);
			return ratio;
		}
		else if( TEST_LEARN )
		{
			ratio = 0;
			for(vector<CPosition*>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
				ratio += (*ipos)->getWeight();
			return ratio;

		}
		else
		{
			ratio = coveredPositions.size();
			return ratio;
		}
	}
	//当覆盖列表发生更改时调用，更新坐标、覆盖节点列表和ratio
	void updateStatus()
	{
		recalculateCenter();
		generateCoveredNodes();
		calculateRatio();
	}

	int getNCoveredPositionsForNode(int inode);	
	
	double getCoByCandidateType()
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
	//FIXEME: 未测试
	inline int getDeliveryCount()
	{
		if(age == 0)
			return deliveryCounts.at(0);
		else
			return deliveryCounts.at( age - 1 );
	}
	//返回( untilTime - 900, untilTime )期间的投递计数
	inline int getDeliveryCount(int untilTime)
	{
		int i = ( untilTime - time ) / SLOT_HOTSPOT_UPDATE - 1;
		if( i < 0 || i > deliveryCounts.size() )
		{
			cout << "Error @ CHotspot::getDeliveryCount(" << untilTime << ") : " << i << " exceeds (0," << deliveryCounts.size() - 1 << ") !" << endl;
			_PAUSE;
		}
		return deliveryCounts.at( i );
	}
	inline void addDeliveryCount(int n)
	{
		deliveryCounts.at( deliveryCounts.size() - 1 ) += n;
	}
	//从当前这一热点任期内的等待时间，该函数应当在MA的路径更新时调用输出统计结果
	//FIXEME: 未测试
	inline int getWaitingTime()
	{
		if(age == 0)
			return waitingTimes.at(0);
		else
			return waitingTimes.at( age - 1 );
	}
	//返回( untilTime - 900, untilTime )期间的等待时间
	inline int getWaitingTime(int untilTime)
	{
		int i = ( untilTime - time ) / SLOT_HOTSPOT_UPDATE - 1;
		if( i < 0 || i > waitingTimes.size() )
		{
			cout << "Error @ CHotspot::getDeliveryCount(" << untilTime << ") : " << i << " exceeds (0," << waitingTimes.size() - 1 << ") !" << endl;
			_PAUSE;
		}		return waitingTimes.at( i );
	}
	inline void addWaitingTime(int t)
	{
		waitingTimes.at( waitingTimes.size() - 1 ) += t;
	}

	//自动生成ID，需手动调用，为了确保热点ID的唯一性，制作临时拷贝时不应调用此函数
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;
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
		
	/****************************************   merge-HAR   ****************************************/ 

	//计算两个热点集合中所有热点pair的重叠面积之和（如有三个或更多个热点共同重叠，重叠部分的面积将多次计入，以度量相似程度）
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//计算一个热点集合内的重叠面积，用于更准确的计算集合间重叠面积
	static double getOverlapArea(vector<CHotspot *> hotspots);

};


