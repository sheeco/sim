#include "Coordinate.h"


//CCoordinate::CCoordinate()
//{
//}
//
//CCoordinate::~CCoordinate()
//{
//}

CCoordinate operator+ (const CCoordinate& lhs, const CCoordinate& rhs)
{
	return CCoordinate(lhs.getX() + rhs.getX(), lhs.getY() + rhs.getY());
}

CCoordinate operator- (const CCoordinate& lhs, const CCoordinate& rhs)
{
	return CCoordinate(lhs.getX() - rhs.getX(), lhs.getY() - rhs.getY());
}

CCoordinate operator* (const double ratio, const CCoordinate& hs)
{
	return CCoordinate(hs.getX() * ratio, hs.getY() * ratio);
}

CCoordinate operator* (const CCoordinate& hs, const double ratio)
{
	return ratio * hs;
}

CCoordinate operator/ (const CCoordinate& hs, const double ratio)
{
	return CCoordinate(hs.getX() / ratio, hs.getY() / ratio);
}