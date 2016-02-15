#include <iostream>
#include <windows.h>
#include <io.h>

using namespace std;

int main(int argc, char* argv[])
{
	int nfile = 29;
	if( argc > 2 )
		nfile = atoi( argv[1] );
	for(int new_id = nfile; new_id >= 1; new_id--)
	{
		string new_name(".newlocation");
		string old_name(".newlocation");
		char buff1[3] = {0};
		itoa(new_id, buff1, 10);
		new_name.insert(0, buff1);
		char buff0[3] = {0};
		itoa(new_id - 1, buff0, 10);
		old_name.insert(0, buff0);

		if( ! _access(old_name.c_str(), 0) )
			rename(old_name.c_str(), new_name.c_str());
	}
}