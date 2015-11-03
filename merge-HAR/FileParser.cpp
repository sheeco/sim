#include "FileParser.h"

void CFileParser::getPositionFromFile(int nodeID, int time, double &x, double &y)
{
	FILE *file;
	char filename[20] = {'\0'};
	double tmp_time = 0;
	double tmp_x = 0;
	double tmp_y = 0;

	try
	{
		sprintf(filename, "%d.newlocation", nodeID);  //新的location文件，将time和坐标信息都包含在内
		file = fopen(filename,"rb");

		while(!feof(file))
		{
			fscanf(file, "%lf %lf %lf", &tmp_time, &tmp_x, &tmp_y);

			if((int)(time - tmp_time) < SLOT_MOBILITYMODEL)  
				break;
		}

		//取得坐标
		x = tmp_x;
		y = tmp_y;

		fclose(file);
	}
	catch(exception e)
	{
		cout<<endl<<"Error: CFileParser::getPositionFromFile()"<<endl;
		_PAUSE;
	}
}

CPosition* CFileParser::findPositionByID(vector<CPosition *> positions, int ID)
{
	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ipos++)
	{
		if((*ipos)->getID() == ID)
			return *ipos;
	}
	return NULL;
}

vector<CHotspot *> CFileParser::getHotspotFromFile(char *filename, int time, vector<CPosition *> positions)
{
	ifstream file;
	vector<CHotspot *> hotspots;
	int tmp_time = 0;
	int tmp_id = 0;
	double tmp_x = 0;
	double tmp_y = 0;
	int tmp_n_position = 0;
	int tmp_position_id = 0;
	try
	{
		file.open(filename);

		if( file.good() )
		{
			CHotspot *hotspot = NULL;
			while( ! file.eof() )
			{
				file >> tmp_time;
				string line;

				//如果time不符合，直接跳过
				if(tmp_time < time)
				{
					getline(file, line);
					continue;
				}
				if(tmp_time > time)
				{
					getline(file, line);
					break;
				}

				//读取hotspot信息
				file >> tmp_id >> tmp_x >> tmp_y >> tmp_n_position;
				
				hotspot = new CHotspot();
				hotspot->setID(tmp_id);
				hotspot->setLocation(tmp_x, tmp_y, tmp_time);
				for(int i = 0; i < tmp_n_position; i++)
				{
					file >> tmp_position_id;
					CPosition *position = findPositionByID(positions, tmp_n_position);
					hotspot->addPosition(position);
				}

				if(hotspot != NULL)
				{
					hotspots.push_back(hotspot);
					hotspot = NULL;
				}			
			}
		}
	}
	catch(exception e)
	{
		cout<<endl<<"Error: CFileParser::getHotspotFromFile() Wrong Format"<<endl;
		_PAUSE;
	}
	file.close();

	return hotspots;
}

void CFileParser::writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail)
{
	ofstream file(filename, ios::app);
	file << hotspot->toString(withDetail);
	file.close();
}

void CFileParser::writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail)
{
	for(vector<CHotspot *>::iterator it = list_hotspot.begin(); it != list_hotspot.end(); it++)
	{
		if(*it != NULL)
			writeHotspotToFile(filename, *it, withDetail);
		else
		{
			cout<<"Error: CFileParser::writeHotspotToFile() list_hotspot has a NULL"<<endl;
			_PAUSE;
		}
	}
}

void CFileParser::writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot)
{
	int coverSum = 0;
	ofstream file(filename, ios::app);
	for(vector<CHotspot *>::iterator it = list_hotspot.begin(); it != list_hotspot.end(); it++)
	{
		if(*it != NULL)
		{
			coverSum += (*it)->getNCoveredPosition();
		}
		else
		{
			cout<<"Error: CFileParser::writeHotspotToFile() list_hotspot has a NULL"<<endl;
			_PAUSE;
		}
	}

	file << time << TAB << coverSum << TAB << list_hotspot.size() << TAB << coverSum / list_hotspot.size() << endl;
	
	file.close();
}
