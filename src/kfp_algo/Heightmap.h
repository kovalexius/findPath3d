//------------------------------------------------------------------------------
//! @author Edited by: Ковалев А.Г.
//!
/*! @file

    @brief
			Классы описывающие вспомогательные типы для построения карты
    @details  */
//------------------------------------------------------------------------------
#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <memory>
#include "Geometry.h"


//------------------------------------------------------------------------------
//---------------------------------- HMPoint -----------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! Represent 2D horizontal point with height
/*! */
//------------------------------------------------------------------------------
struct HMPoint
{
	HMPoint();
	HMPoint( float x, float height, float y );
	HMPoint( const Vector2D& vec, float height );

	//! Сравнивает двумерны вектора. Оператор неравно
	bool operator != ( const HMPoint& other ) const;
	//! Сравнивает двумерны вектора. Оператор равно
	bool operator == ( const HMPoint& other ) const;
	//! Сравнивает двумерные вектора. Оператор меньше
	bool operator < ( const HMPoint& other ) const;

	Vector2D m_coord;
	float	m_height;
};


//------------------------------------------------------------------------------
//----------------------------------- Cell -------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! Represent height map with min, max, step.
//! If point at specified coordinates doesn't exist, then ground is returned
/*!  */
//------------------------------------------------------------------------------
class HeightMap
{
public:
    HeightMap();

    //! Сравнивает двумерные вектора. Оператор неравно
    bool operator != (const HeightMap& other) const;
    //! Сравнивает двумерные вектора. Оператор равно
    bool operator == (const HeightMap& other) const;
    //! Сравнивает двумерные вектора. Оператор меньше
    bool operator < (const HeightMap& other) const;

    void init( const std::vector<HMPoint> &points, 
               const float step, 
               const Vector2D &min,
               const Vector2D &max,
               const float ground = -1000.0f);
    HMPoint* getPoint( const int col, const int row ) const;
    int getColumnsNumber() const;
    int getRowsNumber() const;
    const Vector2D& getMin() const;
    const Vector2D& getMax() const;
    float getCellSize() const;
    float getGround() const;

private:
    std::vector<std::vector<std::unique_ptr<HMPoint>>> m_cells;
    float m_cell_size;
    Vector2D m_min;
    Vector2D m_max;
    int m_rownum;
    int m_colnum;
    float m_ground;
};

#endif