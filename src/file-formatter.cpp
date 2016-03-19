#include <iostream>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

using std::ios;
using std::string;

// #### n.trace �ļ�˵��
// - n �� 1 ��ʼ��
// - ÿһ�еĸ�ʽ��`time	x	y`��

int main(int argc, char* argv[])
{
	int incre = 0;  //change of id if necessary
	int nfile = 100;
	string path;
	if( argc >= 2 )  //1st arg: file count
		nfile = atoi( argv[1] );
	if( argc >= 3 )  //2nd arg: relative file path
		path = argv[2];

	// 00 Existence only
	// 02 Write permission
	// 04 Read permission
	// 06 Read and write permission

	if( _access("0.newlocation", 00) == 0 )  //if file exists
		incre = 1;

	for(int new_id = 1; new_id <= nfile; ++new_id)
	{
		string new_name(".trace");
		string old_name(".newlocation");
		int old_id = new_id - incre;

		char buff_1[5] = {0};
		itoa(new_id, buff_1, 10);
		new_name.insert(0, buff_1);
		new_name.insert(0, path);

		char buff_0[5] = {0};
		itoa(old_id, buff_0, 10);
		old_name.insert(0, buff_0);
		old_name.insert(0, path);

		if( _access(old_name.c_str(), 02) == 0
			&& _access(new_name.c_str(), 00) != 0 )  //if writeable
			rename(old_name.c_str(), new_name.c_str());
		else
			break;
	}

}