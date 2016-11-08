#include <set>
#include <limits>

#include "fileMapReader.h"

using namespace std;

#ifdef _WIN32
void readMap( const wchar_t* filename, 
              HeightMap &cells )
{
  wstreambuf* backup = wcin.rdbuf();
  wifstream f;
  f.open( filename );
  if (!f.is_open())
    throw custom_exception( std::string("file not exist") );
  wcin.rdbuf(f.rdbuf());

  vector<HMPoint> points;
  float step;
  Vector2D min;
  Vector2D max;
  f >> step;
  min.x = min.y = std::numeric_limits<float>::max();
  max.x = max.y = std::numeric_limits<float>::min();
  while (!f.eof())
  {
    float x, h, y;
    f >> x >> h >> y;
    points.push_back(HMPoint(x, h, y));
    if (x < min.x)
      min.x = x;
    if (x > max.x)
      max.x = x;
    if (y < min.y)
      min.y = y;
    if (y > max.y)
      max.y = y;
  }
  wcin.rdbuf(backup);

  cells.init(points, step, min, max);
}
#else
void readMap( const char* filename, 
              HeightMap &cells )
{
  ifstream f;
  f.open( filename );
  if (!f.is_open())
    throw custom_exception( std::string("file not exist") );

  vector<HMPoint> points;
  float step;
  Vector2D min;
  Vector2D max;
  f >> step;
  min.x = min.y = std::numeric_limits<float>::max();
  max.x = max.y = std::numeric_limits<float>::min();
  while (!f.eof())
  {
    float x, h, y;
    f >> x >> h >> y;
    points.push_back(HMPoint(x, h, y));
    if (x < min.x)
      min.x = x;
    if (x > max.x)
      max.x = x;
    if (y < min.y)
      min.y = y;
    if (y > max.y)
      max.y = y;
  }

  cells.init(points, step, min, max);
}
#endif

custom_exception::custom_exception( const std::string& message )
{
  m_message = message;
}

const char* custom_exception::what() const noexcept
{
  return m_message.c_str();
}
