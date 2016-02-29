#include "HDC.h"
#include "NodeRepair.h"

int CHDC::HOTSPOT_COST_SUM = 0;
int CHDC::HOTSPOT_COST_COUNT = 0;
double CHDC::MERGE_PERCENT_SUM = 0;
int CHDC::MERGE_PERCENT_COUNT = 0;
double CHDC::OLD_PERCENT_SUM = 0;
int CHDC::OLD_PERCENT_COUNT = 0;
double CHDC::SIMILARITY_RATIO_SUM = 0;
int CHDC::SIMILARITY_RATIO_COUNT = 0;

extern _HotspotSelect HOTSPOT_SELECT;

CHDC::CHDC(void)
{
}

void CHDC::CompareWithOldHotspots(int currentTime)
{
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots, CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots.size() * AREA_SINGLE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots);
	double newArea = CHotspot::selectedHotspots.size() * AREA_SINGLE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

	ofstream similarity("similarity.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection + SLOT_HOTSPOT_UPDATE )
	{
		similarity << INFO_LOG;
		similarity << "#Time" << TAB << "#Overlap/Old" << TAB << "#Overlap/New" << TAB
				   << "#OverlapArea" << TAB << "#OldArea" << TAB << "#NewArea" << endl;
	}
	similarity << currentTime << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//用于计算最终选取出的热点的前后相似度的历史平均值信息
	SIMILARITY_RATIO_SUM += overlapArea / oldArea;
	SIMILARITY_RATIO_COUNT++;
}

extern string INFO_HOTSPOT;
extern string INFO_HOTSPOT_STATISTICS;
extern string INFO_MERGE;
extern string INFO_MERGE_DETAILS;

void CHDC::PrintInfo(int currentTime)
{
	if( ! ( ( currentTime % SLOT_HOTSPOT_UPDATE == 0 
		      && currentTime >= startTimeForHotspotSelection )
			|| currentTime == RUNTIME  ) )
		return;

	HAR::PrintInfo(currentTime);
}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	if( ! ( currentTime % SLOT_MOBILITYMODEL == 0 ) )
		return;

	vector<CHotspot *> hotspots = CHotspot::selectedHotspots;
	if( hotspots.empty() )
		return;
	cout << "########  < " << currentTime << " >  DUTY CYCLE UPDATE" << endl ;

	int atHotspotCount = 0;
	vector<CNode *> nodes = CNode::getNodes();
	nodes = CPreprocessor::mergeSort( nodes );
	hotspots = CPreprocessor::mergeSort( hotspots, CPreprocessor::ascendByLocationX );
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		CHotspot *atHotspot = nullptr;

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
		{
			if( (*ihotspot)->getX() + TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + TRANS_RANGE < (*ihotspot)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **ihotspot ) <= TRANS_RANGE )		
			{
				atHotspot = *ihotspot;
				break;
			}
		}

		//update duty cycle
		if( (*inode)->isAtHotspot() && atHotspot == nullptr )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " leaves Hotspot " << (*inode)->getAtHotspot()->getID() << " )              " ;			
			(*inode)->setAtHotspot(nullptr);
			(*inode)->resetDutyCycle();
		}
		else if(  ( ! (*inode)->isAtHotspot() ) && atHotspot != nullptr )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " enters Hotspot " << atHotspot->getID() << " )               " ;
			(*inode)->setAtHotspot(atHotspot);
			(*inode)->raiseDutyCycle();
		}
		if( (*inode)->isAtHotspot() )
		{
			CNode::visitAtHotspot();
			++atHotspotCount;
		}
		else
			CNode::visitOnRoute();

	}

	//控制台输出时保留一位小数
	double encounterRatio = NDigitFloat( CNode::getEncounterAtHotspotPercent() * 100, 1);
	flash_cout << "####  [ Hotspot Encounter ]  " << encounterRatio << " %                                           " << endl;
	//flash_cout << "####  [ At Hotspot ]  " << atHotspotCount << " / " << CNode::getNodes().size() << "                              " ;

}