#include "Global.h"
#include "FileHelper.h"
#include <filesystem>


bool CFileHelper::IfExists(string filename)
{
	ifstream input(filename, ios::in);
	if( input.is_open()
	   && ( ! input.eof() ) )
		return true;

	return false;
}

bool CFileHelper::IsEmpty(string filename)
{
	ofstream input(filename, ios::app);
	input.seekp(0, ios::end);
	if( ! input.tellp() )
		return true;

	return false;
}

vector<string> CFileHelper::ListDirectory(string pathDir)
{
	using std::experimental::filesystem::directory_iterator;
	using std::experimental::filesystem::directory_entry;
	using std::experimental::filesystem::path;

	vector<string> ret;

	for( directory_entry it : directory_iterator(pathDir) )
		ret.push_back(it.path().string());
	// TODO: sort by node id ?
	return ret;
}

vector<string> CFileHelper::FilterByExtension(vector<string> filenames, string extension)
{
	vector<string> ret;
	for( string name : filenames )
	{
		if( name.rfind(extension) == name.length() - extension.length() )
			ret.push_back(name);
	}
	return ret;
}

pair<string, string> CFileHelper::SplitPath(string path)
{
	size_t pos = path.rfind('\\');
	if( pos == path.npos )
		pos = path.rfind('/');
	if( pos == path.npos )
		return pair<string, string>("", path);

	string dir = string(path.begin(), path.begin() + pos + 1);
	string filename = string(path.begin() + pos + 1, path.end());
	return pair<string, string>(dir, filename);
}

pair<string, string> CFileHelper::SplitFilename(string filename)
{
	size_t pos = filename.rfind('.');
	string name = string(filename.begin(), filename.begin() + pos);
	string extension = string(filename.begin() + pos, filename.end());
	return pair<string, string>(name, extension);
}

void CFileHelper::test()
{
	vector<string> ls = ListDirectory(string("../res/NCSU"));
	ls = FilterByExtension(ls, string(".trace"));
	ASSERT(ls.size());
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
//				for(int i = 0; i < temp_n_position; ++i)
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
//		throw string("CFileHelper::getHotspotFromFile() : Wrong Format");
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
//	for(vector<CHotspot *>::iterator it = list_hotspot.begin(); it != list_hotspot.end(); ++it)
//	{
//		if(*it != nullptr)
//			writeHotspotToFile(filename, *it, withDetail);
//		else
//		{
//			throw string("CFileHelper::writeHotspotToFile() : list_hotspot has a nullptr");
//		}
//	}
//}

//void CFileHelper::writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot)
//{
//	int coverSum = 0;
//	ofstream file(filename, ios::app);
//	for(vector<CHotspot *>::iterator it = list_hotspot.begin(); it != list_hotspot.end(); ++it)
//	{
//		if(*it != nullptr)
//		{
//			coverSum += (*it)->getNCoveredPosition();
//		}
//		else
//		{
//			throw string("CFileHelper::writeHotspotToFile() : list_hotspot has a nullptr");
//		}
//	}
//
//	file << time << TAB << coverSum << TAB << list_hotspot.size() << TAB << coverSum / list_hotspot.size() << endl;
//	
//	file.close();
//}

//void CFileHelper::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
//{
//	ofstream toFile("hotspots.log");
//	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
//		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
//}