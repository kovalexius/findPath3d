#include "Heightmap.h"

// HeightMapPoint
HeightMapPoint::HeightMapPoint(  float x, float h, float y )
{
	v = Vector2D( x, y );
	this->h = h;
}

HeightMapPoint::HeightMapPoint( const HeightMapPoint& other )
{
	v = other.v;
	h = other.h;
}

HeightMapPoint& HeightMapPoint::operator=(  const HeightMapPoint& other )
{
	v = other.v;
	h = other.h;
	return *this;
}

bool HeightMapPoint::operator != ( const HeightMapPoint& other ) const
{
	return ( v != other.v );
}

bool HeightMapPoint::operator == ( const HeightMapPoint& other ) const
{
	return ( v == other.v );
}

bool HeightMapPoint::operator < ( const HeightMapPoint& other ) const
{
	return ( v < other.v );
}
// end of HeightMapPoint

// Cell
Cell::Cell( float x, float h, float y, float step )
{
	v = Vector2D( x, y );
	this->h = h;
	this->step = step;
}
// end of Cell