//------------------------------------------------------------------------------
//! @author Edited by: Ковалев А.Г.
//!
/*! @file

    @brief
     Класс Scene генерирующий препятствия из текстового файла формата: float шаг, 
		 float x, float y, float z (n раз)

    @details  */
//------------------------------------------------------------------------------
#ifndef Scene_H
#define Scene_H
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <set>
#include <map>
#include <list>
#include <memory>
#include "Heightmap.h"
#include "Geometry.h"
#include "MeshObject.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------- class Scene  ---------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! Класс Scene генерирующий препятствия
/*! */
//------------------------------------------------------------------------------
class Scene
{
public:
	//! Конструктор класса, принимающий параметры: имя файла, уровень воды
	Scene( char* filename, float level );
	~Scene();
	//! Шаг или ширина ячейки
	float step;
	//! Координата по Y определяющая нижний уровень кубообразных объектов
	float ground;
	//! Уровень воды
	float level;

	//! Контейнер, содержащий список всех потенциальных объектов-препятствий.
        std::set< std::shared_ptr<MeshObject> > objects;
	//! Контейнер, содержащий актуальный список объектов-препятствий для заданного уровня воды.
        std::set< std::shared_ptr<MeshObstacle> > obstacles;
	//! Объект содержащий поверхность воды
        std::shared_ptr<MeshObject> waterMesh;
        std::shared_ptr<MeshObstacle> waterObstacle;
	//! Найти путь
  /*! Вход: Vector3D начала пути
						Vector3D конца пути
			Выход:
						result - список промежуточных точек пути list<Vector3D>
   *  @return  true если возможно проложить маршрут, иначе false */
         bool FindPath( std::list<Vector3D> &result, const Vector3D& source, const Vector3D& dest );
         bool FindPathDijkstra( std::list<Vector3D> &result, const Vector3D& source, const Vector3D& dest );
private:
	//! Найти ближайшее к точке P0 пересечение луча с объектами-препятствиями
	/*! Пересечение исключает ребра полигонов.
			Вход: P0 - первая точка Vector3D луча
						P1 - вторая точка Vector3D луча
			Выход:	crossP - точка перечечения
							face - грань пересечения
							obj - объект пересечения
	*		@return	 Возвращает true если было перечение, иначе false	*/
        bool HalfIntersectRay( Vector3D &crossP, std::shared_ptr<Poly> &face, std::shared_ptr<MeshObstacle> &obj,
                               const Vector3D &P0, const Vector3D &P1 );
	//! Найти ближайшее к точке P0 пересечение луча с объектами-препятствиями
	/*! Пересечение засчитывается, если луч прошел полигон объекта а также если попал на ребро полигона.
			Вход: P0 - первая точка Vector3D луча
						P1 - вторая точка Vector3D луча
			Выход:	crossP - точка перечечения
							face - грань пересечения
							obj - объект пересечения
	*		@return	 Возвращает true если было перечение, иначе false	*/
        bool FullIntersectRay( Vector3D &crossP, std::shared_ptr<Poly> &face,
                               std::shared_ptr<MeshObstacle> &obj, const Vector3D &P0, const Vector3D &P1 );
	//! Найти ближайшее к точке P0 пересечение отрезка с объектами-препятствиями
	/*! Пересечение исключает ребра полигонов, но срабатывает, если отрезок касается полигона своим началом или концом
			Вход: P0 - первая точка отрезка
						P1 - конечная точка отрезка 
			Выход:	crossP - точка перечечения
							face - грань пересечения
							obj - объект пересечения
	*		@return	 Возвращает true если было перечение, иначе false	*/
        bool HalfIntersect( Vector3D &crossP, std::shared_ptr<Poly> &face, std::shared_ptr<MeshObstacle> &obj,
                            const Vector3D &P0, const Vector3D &P1 );

	//! Найти ближайшее к точке P0 пересечение отрезка с объектами-препятствиями
	/*! Пересечение срабатывает если отрезок проходит через ребро полигона, а также если отрезок касается 
			полигона своим началом или концом.
			Вход: P0 - первая точка отрезка
						P1 - конечная точка отрезка 
			Выход:	crossP - точка перечечения
							face - грань пересечения
							obj - объект пересечения
	*		@return	 Возвращает true если было перечение, иначе false	*/

        bool FullIntersect( Vector3D &crossP, std::shared_ptr<Poly> &face, std::shared_ptr<MeshObstacle> &obj,
                            const Vector3D &P0, const Vector3D &P1 );
	//! Найти ближайшее к точке P0 пересечение отрезка с объектами-препятствиями
	/*! Пересечение срабатывает если отрезок проходит через ребро полигона. 
			Не срабатывает, если отрезок лишь касается полигона своим началом или концом.
			Вход: P0 - первая точка отрезка
						P1 - конечная точка отрезка 
			Выход:	crossP - точка перечечения
							face - грань пересечения
							obj - объект пересечения
	*		@return	 Возвращает true если было перечение, иначе false	*/
        bool FullIntersectExcludingBoth( Vector3D &crossP, std::shared_ptr<Poly> &face, std::shared_ptr<MeshObstacle> &obj,
                                         const Vector3D &P0, const Vector3D &P1 );

	//! Определить, было ли пересечение отрезка с объектами-препятствиями
	/*! Пересечение засчитывается, если отрезок прошел полигон объекта, а также если попал на ребро полигона.
			Пересечение исключает начало отрезка P0, т.е. если точка P0 лежит в полигоне.
			Если лишь P1 лежит на полигоне и отрезок ложится изнутри объекта то пересечение засчитывается
			Вход: P0 - первая точка Vector3D отрезка
						P1 - вторая точка Vector3D отрезка
	*		@return	 Возвращает true если было пересечение, иначе false	*/
	bool SuperIntersect( const Vector3D &P0, const Vector3D &P1 );

	//! Обойти заданный объект
	/*! Обойти заданный объект
			Направление обхода задается двумя типами данных:
			Начальной гранью и начальным ребром. Этой информации достаточно чтобы определить направление обхода.
			Вход: obj - объект
						poly - грань
						edg  - ребро
						src - начальная точка, для которой нужно определять обход
						dest - конечная точка, для которой нужно определять обход
			Выход:
						result - структура типа pair<float, list<Vector3D>>, где первая переменная пары - общий путь обхода, 
						а вторая переменная это список с точками траектории обхода.
	*		@return	 Возвращает true если обход можно выполнить, иначе false	*/	
        bool bypassObject( std::pair<float, std::list<Vector3D>> &result, std::shared_ptr<Poly> &outFace,
                           Edge *&outEdg, const std::shared_ptr<MeshObstacle> &obj, const std::shared_ptr<Poly> &poly,
                           Edge * const &edg, const Vector3D & src, const Vector3D &dest );
        void initGraph( std::map< Vector3D, std::set<Vector3D> > &graph, std::map< Vector3D, double > &metki );
        bool CheckPath( std::list<Vector3D> &result, const Vector3D& source, const Vector3D& dest );

	//! Создает двухмерный массив объектов типа Cell 
	/*!	Вход: HMPoints - контейнер объектов типа HeightMapPoint
						xmin - координата начала карты по x
						ymin - координата начала карты по y
						im - количество ячеек по x
						jm - количество ячеек по y
			Выход:
						cell - двумерный массив объектов типа Cell */
        void createCells( std::vector<std::vector<std::shared_ptr<Cell>>> &cell, const std::set<HeightMapPoint> &HMPoints,
                          const float &xmin, const float &ymin, const int &im, const int &jm );

	//! Создает параллелепипедную поверхность всех ячеек
	/*!	Вход: cell - двумерный массив объектов Cell
						im - количество ячеек по x
						jm - количество ячеек по y
			Выход:
						vertexes - набор всех вершин поверхности
						triangles - набор всех полигонов
						normals - набор нормалей к полигонам */
        void createMesh( std::set<Vector3D> &vertexes, std::set<shared_ptr<Poly>> &triangles, std::set<Vector3D> &normals,
                         const std::vector<std::vector<shared_ptr<Cell>>> &cell, const int &im, const int &jm );

	//! Создает список вершин с привязанными к каждому группе полигонов
	/*! Вход: triangles - список всех полигонов
			Выход:
						vecToPoly - map, ключом которому служит указатель на вершины значением которого являются списки полигонов  */
        void createLinkPolyes( const std::set<std::shared_ptr<Poly>> &triangles,
                                std::map<Vector3D*, std::shared_ptr<std::set<std::shared_ptr<Poly>>>> &vecToPoly );

	//! Контейнер, содержащий убывающий список непройденных препятствий.
        std::set< std::shared_ptr<MeshObstacle> > hotObjs;

        std::shared_ptr<Poly> lastPoly;
	Edge *lastEdg;
        std::shared_ptr< MeshObstacle > lastObj;
};

#endif
