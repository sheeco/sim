#include "Global.h"
#include "FileHelper.h"
#include <filesystem>
using std::experimental::filesystem::directory_iterator;
using std::experimental::filesystem::directory_entry;
using std::experimental::filesystem::path;
using std::experimental::filesystem::is_directory;
using std::experimental::filesystem::is_regular_file;


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

bool CFileHelper::IsDirectory(string strPath)
{
	path p(strPath);
	return is_directory(p);
}

bool CFileHelper::IsFile(string strPath)
{
	path p(strPath);
	return is_regular_file(p);
}

vector<string> CFileHelper::ListDirectory(string pathDir)
{
	vector<string> ret;

	for( directory_entry it : directory_iterator(pathDir) )
		ret.push_back(it.path().string());
	// TODO: sort by node id ?
	return ret;
}

//TODO: retest
vector<string> CFileHelper::FilterByExtension(vector<string> filenames, string extension)
{
	vector<string> ret;
	for( string name : filenames )
	{
		path p(name);
		if( p.extension() == extension )
			ret.push_back(name);
	}
	return ret;
}

pair<string, string> CFileHelper::SplitPath(string strPath)
{
	path p(strPath);
	if(p.has_parent_path())
		return pair<string, string>(p.parent_path().relative_path().string(), p.filename().string());
	else
		return pair<string, string>("", strPath);
}

pair<string, string> CFileHelper::SplitFilename(string filename)
{
	path p(filename);
	p = p.filename();
	return pair<string, string>(p.stem().string(), p.extension().string());
}

void CFileHelper::test()
{
	ASSERT(IsDirectory("../res/NCSU"));
	ASSERT(! IsFile("../res/NCSU"));
	ASSERT(IsFile("../.project/version.h"));
	ASSERT(!IsDirectory("../.project/version.h"));

	vector<string> ls = ListDirectory(string("../res/NCSU"));
	ls = FilterByExtension(ls, string(".trace"));
	ASSERT(ls.size());

	pair<string, string> pair;
	pair = SplitPath("../.project/version.h");
	ASSERT(pair.second == "version.h");
	pair = SplitFilename("../.project/version.h");
	ASSERT(pair.first == "version");
	ASSERT(pair.second == ".h");
}
