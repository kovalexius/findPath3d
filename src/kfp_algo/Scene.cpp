#include <iostream>

#include "Scene.h"

using namespace std;

Scene::Scene( const float level, const HeightMap &hmap )
{
	m_level = level;
    m_step = hmap.getCellSize();
    m_ground = hmap.getGround();

    std::set<Vector3D> vertexes;
    std::set< std::shared_ptr<Poly> > triangles;
    std::set<Vector3D> normals;
    //! Генерация вершин, полигонов и нормалей
    createMesh( hmap, vertexes, triangles, normals );

	map< Vector3D*, shared_ptr<set<shared_ptr<Poly>>>> vecToPoly;
	createLinkPolyes( triangles, vecToPoly );
	//! Создание MeshObject и разделение мешей
	while( !vecToPoly.empty() )
	{
		shared_ptr<MeshObject> curObj( new MeshObject( vecToPoly) );
		objects.insert( curObj );
	}

	//! Создание MeshObject препятствий
	vecToPoly.clear();
	createLinkPolyes( triangles, vecToPoly );
	while( !vecToPoly.empty() )
	{
		bool isObstacle = false;
		shared_ptr<MeshObstacle> curObj( new MeshObstacle( vecToPoly, level, isObstacle ) );
		if( isObstacle )
			obstacles.insert( curObj );
	}
	vecToPoly.clear();
	triangles.clear();

	//! Создание поверхности воды
	Vector3D *vec[4];
	Vector2D tex[4] = { Vector2D( 0, 0 ), Vector2D( 0, 1 ), Vector2D( 1, 0 ), Vector2D( 1, 1 ) };	// Текстурные координаты одинаковы для всех полигонов ( пополигонный тайлинг)
	auto v1 = vertexes.insert( Vector3D( hmap.getMin().x, level, hmap.getMin().y ) );
	auto v2 = vertexes.insert( Vector3D( hmap.getMin().x, level, hmap.getMax().y + m_step ) );
	auto v3 = vertexes.insert( Vector3D( hmap.getMax().x + m_step, level, hmap.getMax().y + m_step ) );
	auto v4 = vertexes.insert( Vector3D( hmap.getMax().x + m_step, level, hmap.getMin().y ) );
	vec[0] = const_cast<Vector3D*>( &(*v1.first) ); 
	vec[1] = const_cast<Vector3D*>( &(*v2.first) ); 
	vec[2] = const_cast<Vector3D*>( &(*v3.first) ); 
	vec[3] = const_cast<Vector3D*>( &(*v4.first) );
  Poly *tri = new Poly( vec, tex, 4, (new Vector3D(0, 1, 0)) );
  triangles.insert( std::shared_ptr<Poly>( tri ) );                 // (new Vector3D(0, 1, 0)) Нормаль

	createLinkPolyes( triangles, vecToPoly );
	waterMesh = shared_ptr<MeshObject>( new MeshObject( vecToPoly) );
	vecToPoly.clear();
	createLinkPolyes( triangles, vecToPoly );
	waterObstacle = shared_ptr< MeshObstacle>( new MeshObstacle( vecToPoly ) );
}

void Scene::createMesh( const HeightMap& hmap,
                        set<Vector3D> &vertexes, 
                        set<shared_ptr<Poly>> &triangles, 
                        set<Vector3D> &normals )
{
    Vector3D *vec[4];
    Vector2D tex[4] = { Vector2D( 0, 0 ), Vector2D( 0, 1 ), Vector2D( 1, 0 ), Vector2D( 1, 1 ) };	// Текстурные координаты одинаковы для всех полигонов ( пополигонный тайлинг)
    int max_i = hmap.getColumnsNumber();
    int max_j = hmap.getRowsNumber();
    for( int i = 0; i < max_i; i++ )
    {
        for( int j = 0; j < max_j; j++ )
        {
            HMPoint *pnt = hmap.getPoint(i, j);
            if( pnt )
            {
                auto v1 = vertexes.insert( Vector3D( pnt->m_coord.x,      m_ground,           pnt->m_coord.y ) );
                auto v2 = vertexes.insert( Vector3D( pnt->m_coord.x,      pnt->m_height,      pnt->m_coord.y ) );
                auto v3 = vertexes.insert( Vector3D( pnt->m_coord.x+m_step,  m_ground,        pnt->m_coord.y ) );
                auto v4 = vertexes.insert( Vector3D( pnt->m_coord.x+m_step,  pnt->m_height,   pnt->m_coord.y) );
                auto v5 = vertexes.insert( Vector3D( pnt->m_coord.x,       m_ground,          pnt->m_coord.y+m_step ) );
                auto v6 = vertexes.insert( Vector3D( pnt->m_coord.x,       pnt->m_height,     pnt->m_coord.y+m_step ) );
                auto v7 = vertexes.insert( Vector3D( pnt->m_coord.x+m_step,  m_ground,        pnt->m_coord.y+m_step ) );
                auto v8 = vertexes.insert( Vector3D( pnt->m_coord.x+m_step,  pnt->m_height,   pnt->m_coord.y+m_step ) );

                vec[0] = const_cast<Vector3D*>( &(*v2.first) ); 
                vec[1] = const_cast<Vector3D*>( &(*v6.first) ); 
                vec[2] = const_cast<Vector3D*>( &(*v8.first) ); 
                vec[3] = const_cast<Vector3D*>( &(*v4.first) );
                auto norm = normals.insert( Vector3D( 0, 1, 0 ) );
                shared_ptr<Poly> tri = shared_ptr<Poly>( new Poly( vec, tex, 4, &(*norm.first) ) );
                triangles.insert( tri );

                vec[0] = const_cast<Vector3D*>( &(*v1.first) ); 
                vec[1] = const_cast<Vector3D*>( &(*v5.first) );
                vec[2] = const_cast<Vector3D*>( &(*v7.first) );
                vec[3] = const_cast<Vector3D*>( &(*v3.first) );
                norm = normals.insert( Vector3D( 0, -1, 0 ) );
                tri = shared_ptr<Poly>( new Poly( vec, tex, 4, &(*norm.first) ) );
                triangles.insert( tri );

                if( j>0 )
                {
                    HMPoint *pntj1 = hmap.getPoint(i,j-1);
                    if( !pntj1 )
					{
						vec[0] = const_cast<Vector3D*>( &(*v1.first) ); 
						vec[1] = const_cast<Vector3D*>( &(*v2.first) );
						vec[2] = const_cast<Vector3D*>( &(*v4.first) );
						vec[3] = const_cast<Vector3D*>( &(*v3.first) );
						norm = normals.insert( Vector3D( 0, 0, -1 ) );
						shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
						triangles.insert( tri );
					}
					else
					{
                        //! Доборочная грань вниз
                        if( pnt->m_height > pntj1->m_height )
                        {
							vec[0] = const_cast<Vector3D*>( &(*v2.first) );
							vec[1] = const_cast<Vector3D*>( &(*v4.first) );
							vec[2] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[1]->x, pntj1->m_height, vec[1]->z ) ) ) );
							vec[3] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[0]->x, pntj1->m_height, vec[0]->z ) ) ) );
							//! Нормаль смотрит вниз
							norm = normals.insert( Vector3D(0, 0, -1) );
							shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
							triangles.insert( tri );
						}
                        if( pnt->m_height < pntj1->m_height)
						{
							vec[0] = const_cast<Vector3D*>( &(*v2.first) );
							vec[1] = const_cast<Vector3D*>( &(*v4.first) );
							vec[2] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[1]->x, pntj1->m_height, vec[1]->z ) ) ) );
							vec[3] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[0]->x, pntj1->m_height, vec[0]->z ) ) ) );
							//! Нормаль смотрит вверх
							norm = normals.insert( Vector3D(0, 0, 1) );
							shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
							triangles.insert( tri );
						}
					}
				}
                if ( i>0 )
				{
                    HMPoint *pnti1 = hmap.getPoint(i-1,j);
					if( !pnti1 )
					{
						vec[0] = const_cast<Vector3D*>( &(*v1.first) );
						vec[1] = const_cast<Vector3D*>( &(*v2.first) );
						vec[2] = const_cast<Vector3D*>( &(*v6.first) );
						vec[3] = const_cast<Vector3D*>( &(*v5.first) );
						norm = normals.insert( Vector3D(-1, 0, 0) );
						shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
						triangles.insert( tri );
					}
					else
					{
						//! Доборочная грань влево
                        if( pnt->m_height > pnti1->m_height )
						{
							vec[0] = const_cast<Vector3D*>( &(*v2.first) );
							vec[1] = const_cast<Vector3D*>( &(*v6.first) );
							vec[2] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[1]->x, pnti1->m_height, vec[1]->z ) ) ) );
							vec[3] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[0]->x, pnti1->m_height, vec[0]->z ) ) ) );
							//! Нормаль смотрит влево
							norm = normals.insert( Vector3D(-1, 0, 0) );
							shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
							triangles.insert( tri );
						}
                        if ( pnt->m_height < pnti1->m_height )
						{
							vec[0] = const_cast<Vector3D*>( &(*v2.first) );
							vec[1] = const_cast<Vector3D*>( &(*v6.first) );
							vec[2] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[1]->x, pnti1->m_height, vec[1]->z ) ) ) );
							vec[3] = const_cast<Vector3D*>( &(*vertexes.find( Vector3D( vec[0]->x, pnti1->m_height, vec[0]->z ) ) ) );
							//! Нормаль смотрит вправо
							norm = normals.insert( Vector3D( 1, 0, 0 ) );
							shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
							triangles.insert( tri );
						}
					}
				}
				if( i < hmap.getColumnsNumber()-1 )
				{
					if( !hmap.getPoint(i+1,j) )
					{
						vec[0] = const_cast<Vector3D*>( &(*v3.first) ); 
						vec[1] = const_cast<Vector3D*>( &(*v4.first) ); 
						vec[2] = const_cast<Vector3D*>( &(*v8.first) ); 
						vec[3] = const_cast<Vector3D*>( &(*v7.first) );
						norm = normals.insert( Vector3D(1, 0, 0) );
						shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
						triangles.insert( tri );
					}
				}
                if( j < hmap.getRowsNumber()-1 )
                {
                    if( !hmap.getPoint(i,j+1) )
                    {
                        vec[0] = const_cast<Vector3D*>( &(*v5.first) ); 
                        vec[1] = const_cast<Vector3D*>( &(*v6.first) ); 
                        vec[2] = const_cast<Vector3D*>( &(*v8.first) ); 
                        vec[3] = const_cast<Vector3D*>( &(*v7.first) );
                        norm = normals.insert( Vector3D(0, 0, 1) );
                        shared_ptr<Poly> tri( new Poly( vec, tex, 4, &(*norm.first) ) );
                        triangles.insert( tri );
                    }
                }
            }
        }
    }
}

void Scene::createLinkPolyes( const set<shared_ptr<Poly>> &triangles, 
															map< Vector3D*, shared_ptr<set<shared_ptr<Poly>>> > &vecToPoly )
{
	for( auto curPoly = triangles.begin(); curPoly != triangles.end(); curPoly++ )
	{
                for( size_t i = 0; i < (*curPoly)->v.size(); i++ )
		{
			auto it = vecToPoly.find( (*curPoly)->v[i] );
			if( it != vecToPoly.end() )
				(it->second)->insert( *curPoly );
			else
			{
				shared_ptr<set<shared_ptr<Poly>>> curSet(new set<shared_ptr<Poly>>);
				curSet->insert( *curPoly );
				vecToPoly.insert( pair< Vector3D*, shared_ptr<set<shared_ptr<Poly>>>>( (*curPoly)->v[i], curSet ) );
			}
		}
	}
}

bool Scene::HalfIntersectRay(  Vector3D &crossP, shared_ptr<Poly> &face, shared_ptr<MeshObstacle> &obj, const Vector3D &P0, const Vector3D &P1 )
{
	bool result = false;
	
	for( auto ob = hotObjs.begin(); ob != hotObjs.end(); ob++ )
	{
		if( (*ob)->HalfIntersectRay( crossP, face, P0, P1 ) )
		{
			result = true;
			obj = (*ob);
			break;
		}
	}
	return result;
}

bool Scene::FullIntersectRay(  Vector3D &crossP, shared_ptr<Poly> &face, shared_ptr<MeshObstacle> &obj, const Vector3D &P0, const Vector3D &P1 )
{
	bool result = false;
	
	for( auto ob = hotObjs.begin(); ob != hotObjs.end(); ob++ )
	{
		if( (*ob)->FullIntersectRay( crossP, face, P0, P1 ) )
		{
			result = true;
			obj = (*ob);
			break;
		}
	}
	return result;
}

bool Scene::HalfIntersect(  Vector3D &crossP, shared_ptr<Poly> &face, shared_ptr<MeshObstacle> &obj, const Vector3D &P0, const Vector3D &P1 )
{
	bool result = false;
	
	for( auto ob = hotObjs.begin(); ob != hotObjs.end(); ob++ )
	{
		if( (*ob)->HalfIntersect( crossP, face, P0, P1 ) )
		{
			result = true;
			obj = (*ob);
			break;
		}
	}
	return result;
}

bool Scene::FullIntersect(  Vector3D &crossP, shared_ptr<Poly> &face, shared_ptr<MeshObstacle> &obj, const Vector3D &P0, const Vector3D &P1 )
{
	typedef pair< shared_ptr<Poly>, Vector3D > pairPV;
	typedef pair< shared_ptr<MeshObstacle>, pairPV> pairObj;
	map< double, pair< shared_ptr<MeshObstacle>, pair< shared_ptr<Poly>, Vector3D > > > listPoints;
	for( auto ob = hotObjs.begin(); ob != hotObjs.end(); ob++ )
	{
		Vector3D point;
		if( (*ob)->FullIntersect( point, face, P0, P1 ) )
		{
			float h  = ( point - P0 ).Scalar();
			pairPV curPairPV;
			pairObj curPairObj;
			curPairPV.first = face;
			curPairPV.second = point;
			curPairObj.first = *ob;
			curPairObj.second = curPairPV;

			listPoints.insert( pair< double, pairObj >( h, curPairObj ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.second.first;
		crossP = it->second.second.second;
		obj = it->second.first;
		return true;
	}
	return false;
}

bool Scene::FullIntersectExcludingBoth(  Vector3D &crossP, shared_ptr<Poly> &face, shared_ptr<MeshObstacle> &obj, const Vector3D &P0, const Vector3D &P1 )
{
	typedef pair< shared_ptr<Poly>, Vector3D > pairPV;
	typedef pair< shared_ptr<MeshObstacle>, pairPV> pairObj;
	map< double, pair< shared_ptr<MeshObstacle>, pair< shared_ptr<Poly>, Vector3D > > > listPoints;
	for( auto ob = hotObjs.begin(); ob != hotObjs.end(); ob++ )
	{
		Vector3D point;
		if( (*ob)->FullIntersectExcludingBoth( point, face, P0, P1 ) )
		{
			float h  = ( point - P0 ).Scalar();
			pairPV curPairPV;
			pairObj curPairObj;
			curPairPV.first = face;
			curPairPV.second = point;
			curPairObj.first = *ob;
			curPairObj.second = curPairPV;

			listPoints.insert( pair< double, pairObj >( h, curPairObj ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.second.first;
		crossP = it->second.second.second;
		obj = it->second.first;
		return true;
	}
	return false;
}

bool Scene::SuperIntersect( const Vector3D &P0, const Vector3D &P1 )
{
	bool result = false;
	
	for( auto ob = hotObjs.begin(); ob != hotObjs.end(); ob++ )
	{
		if( (*ob)->SuperIntersect( P0, P1 ) )
		{
			result = true;
			break;
		}
	}
	return result;
}

void Scene::initGraph( map< Vector3D, set<Vector3D> > &graph, map< Vector3D, double > &metki )
{
	set< Vector3D > allVertexes;
	for( auto obs = obstacles.begin(); obs != obstacles.end(); obs++ )
		for( auto edg = (*obs)->edgeToTri.begin(); edg != (*obs)->edgeToTri.end(); edg++ )
		{
			Vector3D vec;
			if( edg->first.GetXZ( m_level, vec ) )
				allVertexes.insert( vec );
		}

		/*
		for( auto vec = allVertexes.begin(); vec != allVertexes.end(); vec++ )
			for
			*/
}

bool Scene::FindPathDijkstra( list<Vector3D> &result, const Vector3D& src, const Vector3D& dst )
{
	map< Vector3D, set<Vector3D> > graph;
	map< Vector3D, double > metki;
	map< Vector3D, list<Vector3D> > paths;

	initGraph( graph, metki );

	return true;
}

bool Scene::FindPath( list<Vector3D> &result, const Vector3D& source, const Vector3D& dest )
{
	Vector3D src = source, dst = dest;
	src.y = m_level;
	dst.y = m_level;

	

	//! Инициализация списка препятствий
	//! hotObjs будет убывать по мере детекта пересечений объектов в нем
	hotObjs.clear();
	for( auto it = obstacles.begin(); it != obstacles.end(); it++ )
		hotObjs.insert( *it );																				

	lastObj = 0;

	result.push_front( src );
	if( !CheckPath( result, src, dst ) )
		return false;
	result.push_back( dst );

	return true;
}

bool Scene::CheckPath( list<Vector3D> &result, const Vector3D& source, const Vector3D& dst )
{
	shared_ptr<Poly> face;
	Vector3D	outVec;
	shared_ptr <MeshObstacle> obj;

	Vector3D src = source;
	while( FullIntersectExcludingBoth( outVec, face, obj, src, dst ) )
	{
		shared_ptr<Poly> outFace;
		Edge *outEdg;
		
		hotObjs.erase( obj );
		map< float, list< Vector3D>> tmpPaths;
		map< float, pair< shared_ptr<Poly>, Edge* > > lastPoints;
		for( auto edg = face->edges.begin(); edg != face->edges.end(); edg++ )
		{
			pair< float, list<Vector3D> > path;
			pair< shared_ptr<Poly>, Edge*> tmpPair;
			if( bypassObject( path, outFace, outEdg, obj, face, (*edg), src, dst ) )
			{
				tmpPaths.insert( path );
				tmpPair.first = outFace;
				tmpPair.second = outEdg;
				lastPoints.insert( pair< float, pair< shared_ptr<Poly>, Edge* > >( path.first, tmpPair ) );
			}
		}
		if( !tmpPaths.empty() )
		{
			list< Vector3D> &paths = tmpPaths.begin()->second;			//! Берем наименьший путь
			result.insert( result.end(), paths.begin(), paths.end() );
			src = result.back();
			tmpPaths.clear();
			
			lastPoly = lastPoints.begin()->second.first;
			lastEdg = lastPoints.begin()->second.second;
			lastObj = obj;
			lastPoints.clear();
		}
		else
			return false;
	}
	return true;
}

bool Scene::bypassObject( pair< float, list<Vector3D>> &result, shared_ptr<Poly> &outFace, Edge *&outEdg, 
                        const shared_ptr<MeshObstacle> &obj, const shared_ptr<Poly> &poly,
                        Edge * const &edg, const Vector3D & src, const Vector3D &dest )
{
    Vector3D point, pointNext;			//! Предыдущая точка, следущая точка
    edg->GetXZ( m_level, point );
    shared_ptr<Poly> curFace = poly;
    Edge *curEdg = const_cast<Edge*>( edg );
    shared_ptr<MeshObstacle> curObj = obj;

    list< Vector3D > listVec;		//! список обхода точек текущего объекта

    bool isFree = true;   //! Показывает не пересекает ли отрезок от src до грани текущий объект
    bool isBypassing = true; //! Закончен ли обход объекта

    pointNext = point;
    set< shared_ptr<Poly> > passedPolyes;
    passedPolyes.insert( curFace );
    while( isBypassing )
    {
        if( curObj->FullIntersectExcludingP0( pointNext, dest ) )		// Проверка пересечения только с текущим объектом от грани до точки назначения
        {
            auto setTri = curObj->edgeToTri.find( *curEdg )->second;				// По ребру ищем список полигонов
            if( setTri->size() > 1 )
            {
                    for( auto itF = setTri->begin(); itF != setTri->end(); itF++ )
                            if( curFace != (*itF) )
                            {
                                    curFace = (*itF);
                                    passedPolyes.insert( curFace );
                                    set<Edge*> *setEdg = &(curFace->edges);
                                    if( setEdg->size() > 1 )
                                    {
                                            for( auto itE = setEdg->begin(); itE != setEdg->end(); itE++ )
                                                    if( curEdg != (*itE) )
                                                    {
                                                            curEdg = (*itE);
                                                            curEdg->GetXZ( m_level, pointNext );									// Обновление точки
                                                            if( !isFree )
                                                                    listVec.push_back( point );
                                                            break;
                                                    }
                                    }
                                    else
                                            return false;		// эта грань уходит вникуда, объект нельзя обойти
                                    break;
                            }
            }
            else
                return false;			// это ребро уходит вникуда, объект нельзя обойти
        }
        else
        {
            outFace = curFace;
            outEdg = curEdg;
            listVec.push_back( pointNext );
            isBypassing = false;			// мы обошли объект
        }
		
        if( isFree )
        {
            Vector3D crossVec;
            shared_ptr< Poly > crossPoly;
            if( curObj->FullIntersectExcludingBoth( crossVec, crossPoly, src, pointNext ) )	// Проверка пересечения только с текущим объектом от начала до грани
            {
                if( passedPolyes.find(crossPoly) != passedPolyes.end() )
                {
                    isFree = false;																				// Больше не проверять
                    listVec.push_back( point );
                }
            }
        }
        point = pointNext;
    }

    //! Проверка пересечения от начала до грани с остальными объектами
    pointNext = *( listVec.begin() );
    Vector3D istochnik = src;
    list< Vector3D > spisok;			//! список обхода точек других лбъектов - если было пересечение
    if( lastObj )
    {
            if( lastObj->FullIntersectExcludingP0( istochnik, pointNext ) )
            {
                    pair< float, list<Vector3D> > path;
                    shared_ptr<Poly> outFace;
                    Edge *outEdg;
                    shared_ptr< MeshObstacle > in_obj = lastObj;
                    lastObj = 0;
                    if( bypassObject( path, outFace, outEdg, in_obj, lastPoly, lastEdg, istochnik, pointNext ) )
                    {
                            spisok.insert( spisok.end(), path.second.begin(), path.second.end() );
                            istochnik = *(--spisok.end());
                    }
            }
    }
    /**/
    if( !CheckPath( spisok, istochnik, pointNext) )
            return false;
    //

    //! Сглаживание listVec если были вогнутости и интеграция маршрута.
    list< Vector3D > &res = listVec;
    if( res.size() > 2 )
    {
            auto it1 = res.begin();
            auto it2 = ++(++res.begin());
            while( it2 != res.end() )
            {
                    if( curObj->SuperIntersect( (*it1), (*it2 ) ) )
                    {
                            it1 = it2;
                            it1--;
                            it2++;
                    }
                    else
                    {
                            auto it = it2;
                            it--;
                            res.erase( it );
                            it2++;
                    }
            }
    }

    Vector3D vec = src;
    float h = 0;
    for( auto it = spisok.begin(); it != spisok.end(); it++ )
    {
            h += (*it - vec).Length();					// подсчет длины пути
            vec = *it;
    }
    for( auto it = listVec.begin(); it != listVec.end(); it++ )
    {
            h += (*it - vec).Length();					// подсчет длины пути
            vec = *it;
    }
    h += (dest - vec).Length();
    result.first = h;
    result.second.insert( result.second.end(), spisok.begin(), spisok.end() );
    result.second.insert( result.second.end(), listVec.begin(), listVec.end() );
    //

    return true;
}
