#include "GlobalParameters.h"
#include "FileHelper.h"


// TODO: add CTrace to avoid frequent file reading
bool CFileHelper::getLocationFromFile(int nodeID, int time, CCoordinate &location)
{
	FILE *file;
	char filename[20] = {'\0'};
	double temp_time = 0;
	double temp_x = 0;
	double temp_y = 0;

	try
	{
		// TODO: 改为 .trace 文件
		// TODO: 文件第一行读取 SLOT_MOBILITY
		sprintf(filename, "%d.trace", nodeID);  //新的 location 文件，将 time 和坐标信息都包含在内
		string filepath = "../res/" + DATASET + "/" + filename;  //.exe 文件必须在 bin/ 文件夹下
		file = fopen(filepath.c_str(),"rb");

		if( file == nullptr)
		{
			cout << endl << "Error @ CFileHelper::getLocationFromFile() : Cannot find file \"" << nodeID << ".trace\" ! " << endl;
			_PAUSE_;
			Exit(-1);
		}
		while( ! feof( file ) )
		{
			fscanf(file, "%lf %lf %lf", &temp_time, &temp_x, &temp_y);

			if(int(time - temp_time) < SLOT_MOBILITYMODEL)  
				break;
		}

		if( ( time - temp_time ) >= SLOT_MOBILITYMODEL )
		{
//			cout << endl << "Error @ CFileHelper::getLocationFromFile() : Cannot find location info for Node " << nodeID << " at Time " << time << endl;
			if( time != RUNTIME )
			{
				return false;
//				RUNTIME = time;
//				CRoutingProtocol::PrintFinal(time);
			}
//			debug.close();
//			_ALERT_;
//			Exit(time - SLOT_MOBILITYMODEL);
		}
		//取得坐标
		location.setX(temp_x);
		location.setY(temp_y);

		fclose(file);
		return true;
	}
	catch(exception e)
	{
		cout << endl << "Error @ CFileHelper::getLocationFromFile() : Unknown error without assumption" << endl;
		_PAUSE_;
		return false;
	}
}

//CPosition* CFileHelper::findPositionByID(vector<CPosition *> positions, int ID)
//{
//	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
//	{
//		if((*ipos)->getID() == ID)
//			return *ipos;
//	}
//	return nullptr;
//}

//vector<CHotspot *> CFileHelper::getHotspotFromFile(char *filename, int time, vector<CPosition *> positions)
//{
//	ifstream file;
//	vector<CHotspot *> hotspots;
//	int temp_time = 0;
//	int temp_id = -1;
//	double temp_x = 0;
//	double temp_y = 0;
//	int temp_n_position = 0;
//	int temp_position_id = 0;
//	try
//	{
//		file.open(filename);
//
//		if( file.good() )
//		{
//			CHotspot *hotspot = nullptr;
//			while( ! file.eof() )
//			{
//				file >> temp_time;
//				string line;
//
//				//如果time不符合，直接跳过
//				if(temp_time < time)
//				{
//					getline(file, line);
//					continue;
//				}
//				if(temp_time > time)
//				{
//					getline(file, line);
//					break;
//				}
//
//				//读取hotspot信息
//				file >> temp_id >> temp_x >> temp_y >> temp_n_position;
//				
//				hotspot = new CHotspot();
//				hotspot->setID(temp_id);
//				hotspot->setLocation(temp_x, temp_y, temp_time);
//				for(int i = 0; i < temp_n_position; i++)
//				{
//					file >> temp_position_id;
//					CPosition *position = findPositionByID(positions, temp_n_position);
//					hotspot->addPosition(position);
//				}
//
//				if(hotspot != nullptr)
//				{
//					hotspots.push_back(hotspot);
//					hotspot = nullptr;
//				}			
//			}
//		}
//	}
//	catch(exception e)
//	{
//		cout << endl << "Error @ CFileHelper::getHotspotFromFile() : Wrong Format"<<endl;
//		_PAUSE_;
//	}
//	file.close();
//
//	return hotspots;
//}

//void CFileHelper::writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail)
//{
//	ofstream file(filename, ios::app);
//	file << hotspot->toString(withDetail);
//	file.close();
//}

//void CFileHelper::writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail)
//{
//	for(vector<CHotspot *>::iterator it = list_hotspot.begin(); it != list_hotspot.end(); it++)
//	{
//		if(*it != nullptr)
//			writeHotspotToFile(filename, *it, withDetail);
//		else
//		{
//			cout << endl << "Error @ CFileHelper::writeHotspotToFile() : list_hotspot has a nullptr"<<endl;
//			_PAUSE_;
//		}
//	}
//}

//void CFileHelper::writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot)
//{
//	int coverSum = 0;
//	ofstream file(filename, ios::app);
//	for(vector<CHotspot *>::iterator it = list_hotspot.begin(); it != list_hotspot.end(); it++)
//	{
//		if(*it != nullptr)
//		{
//			coverSum += (*it)->getNCoveredPosition();
//		}
//		else
//		{
//			cout << endl << "Error @ CFileHelper::writeHotspotToFile() : list_hotspot has a nullptr"<<endl;
//			_PAUSE_;
//		}
//	}
//
//	file << time << TAB << coverSum << TAB << list_hotspot.size() << TAB << coverSum / list_hotspot.size() << endl;
//	
//	file.close();
//}

//void CFileHelper::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
//{
//	ofstream toFile("hotspots.txt");
//	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
//		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
//}