#include "../src/GPS.h"

using namespace gps;


//Latitude / longitude - to - UTM 测试值
//
//(0.0000    0.0000)     "31 N 166021 0"
//( 0.1300 - 0.2324 )     "30 N 808084 14385"
//( -45.6456   23.3545 )     "34 G 683473 4942631"
//( -12.7650 - 33.8765 )     "25 L 404859 8588690"
//( -80.5434 - 170.6540 )     "02 C 506346 1057742"
//( 90.0000   177.0000 )     "60 Z 500000 9997964"
//( -90.0000 - 177.0000 )     "01 A 500000 2035"
//( 90.0000    3.0000 )     "31 Z 500000 9997964"
//( 23.4578 - 135.4545 )     "08 Q 453580 2594272"
//( 77.3450   156.9876 )     "57 X 450793 8586116"
//( -89.3454 - 48.9306 )     "22 A 502639 75072"

void testGPS(double lat, double lon, int zone)
{
	WGS84Coor inputWGS84 = { lat, lon };
	UTMCoor outputUTM = { 0, 0 };

	LatLonToUTMXY(inputWGS84.lat, inputWGS84.lon, zone, outputUTM);

	cout << "Input (Lat, Lon, zone):" << endl;
	cout << TAB << inputWGS84.lat << ", " << inputWGS84.lon << ", " << zone << endl;
	cout << "Output (x, y):" << endl;
	cout << TAB << outputUTM.x << ", " << outputUTM.y << endl;
}

void testGPS(double lat, double lon)
{
	WGS84Coor inputWGS84 = { lat, lon };
	UTMCoor outputUTM = { 0, 0 };
	int outputZone = 0;

	outputZone = LatLonToUTMXY(inputWGS84.lat, inputWGS84.lon, outputUTM);

	cout << "Input (Lat, Lon):" << endl;
	cout << TAB << inputWGS84.lat << ", " << inputWGS84.lon << endl;
	cout << "Output (x, y, zone):" << endl;
	cout << TAB << outputUTM.x << ", " << outputUTM.y << ", " << outputZone << endl;
}

//
// default test input
// lat: 40.001467, lon: 116.326665
// 
// correct output
// x: 442525.32406472584, y: 4428137.134626836, zone: 50
//
void testGPS()
{
	testGPS(40.001467, 116.326665);

	cout << "Correct Ouput (x, y) for Default Input (in Beijing):" << endl;
	cout << TAB << "442525.32406472584, 4428137.134626836, 50" << endl;
}

//int main(int argc, char* argv[])
//{
//	// 禁用科学记数法格式，以检查精度
//	cout.setf(ios::fixed);
//
//	if( argc >= 4 )
//		testGPS( atoi( argv[1] ), atoi( argv[2] ), atoi( argv[3] ) );
//	else if( argc >= 3)
//		testGPS( atoi( argv[1] ), atoi( argv[2] ) );
//	else
//	{
//		testGPS();
//		testGPS( -10, 100 );
//		testGPS( -10, 100 , 47);
//		testGPS( -10, 100 , 50);
//	}
//
//	_PAUSE_;
//	return 0;
//}

