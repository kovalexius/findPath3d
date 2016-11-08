#include "Heightmap.h"

HMPoint::HMPoint(  float x, float height, float y )
{
	m_coord = Vector2D( x, y );
	m_height = height;
}

HMPoint::HMPoint(const Vector2D& vec, float height)
{
  m_coord = vec;
  m_height = height;
}

bool HMPoint::operator != ( const HMPoint& other ) const
{
	return ( m_coord != other.m_coord );
}

bool HMPoint::operator == ( const HMPoint& other ) const
{
	return ( m_coord == other.m_coord );
}

bool HMPoint::operator < ( const HMPoint& other ) const
{
	return ( m_coord < other.m_coord );
}



HeightMap::HeightMap()
{
  m_rownum = 0;
  m_colnum = 0;
  m_cell_size = 1.0;
}

void HeightMap::init( const std::vector<HMPoint> &points,
                      const float step,
                      const Vector2D &min,
                      const Vector2D &max,
                      const float ground /* = -1000.0f*/)
{
    m_ground = ground;
    m_min = min;
    m_max = max;
  //! Определение количества ячеек
  float wdt = (max.x - min.x);
  float hght = (max.y - min.y);
  m_colnum = (int)(wdt / step) + 1;
  m_rownum = (int)(hght / step) + 1;
  m_colnum += ( m_colnum*step < wdt ) ? 1 : 0;
  m_rownum += ( m_rownum*step < hght ) ? 1 : 0;

  m_cell_size = step;

  m_cells.resize( m_colnum );
  for (auto it = points.begin(); it != points.end(); it++)
  {
    int i = (it->m_coord.x - min.x) / m_cell_size;
    int j = (it->m_coord.y - min.y) / m_cell_size;
    if (m_cells[i].size() == 0)
      m_cells[i].resize(m_rownum);

    float h = it->m_height;

    //if (h == m_level)
    //  h += step * 0.0001f;

    if (m_cells[i][j] == nullptr)
      m_cells[i][j].reset( new HMPoint( i*m_cell_size + min.x, h, j*m_cell_size + min.y ) );
  }
}

HMPoint* HeightMap::getPoint(const int col, const int row) const
{
  if (m_cells.size() == 0 || m_cells[col].size() == 0 )
    return nullptr;
  return m_cells[col][row].get();
}

int HeightMap::getColumnsNumber() const
{
  return m_colnum;
}

int HeightMap::getRowsNumber() const
{
  return m_rownum;
}

const Vector2D& HeightMap::getMin() const
{
    return m_min;
}

const Vector2D& HeightMap::getMax() const
{
    return m_max;
}

float HeightMap::getCellSize() const
{
    return m_cell_size;
}

float HeightMap::getGround() const
{
    return m_ground;
}