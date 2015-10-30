#include "MeshObject.h"

// MeshObject
MeshObject::MeshObject( map< Vector3D*, shared_ptr<set<shared_ptr<Poly>>>> &vecToTri )
{
	queue< Vector3D* > vecQueue;
	{
	auto it = vecToTri.begin();
	Vector3D vec( Vector3D( *(it->first) ) );
	points.insert( vec );
	vecQueue.push( it->first );
	}
	while( !vecQueue.empty() )
	{
		Vector3D *vecKey = vecQueue.front();
		auto record =  vecToTri.find( vecKey );
		if( record != vecToTri.end() )
		{
			shared_ptr<set<shared_ptr<Poly>>> curSet = record->second;
                        for( auto it = curSet->begin(); it != curSet->end(); it++ )
			{
				shared_ptr<Poly> curPoly = *it;
                                for( size_t i = 0; i < curPoly->v.size(); i++ )
				{
					Vector3D vec( Vector3D( *(curPoly->v[i]) ) );
					auto itPair = points.insert( vec );
					if( itPair.second == true )				// если была вставка нового вектора
						vecQueue.push( curPoly->v[i] );
					curPoly->v[i] = const_cast<Vector3D*>( &(*itPair.first) );
				}
				auto itPair = normals.insert( Vector3D( *(curPoly->normal) ) );
				curPoly->normal = const_cast<Vector3D*>( &(*itPair.first) );
				tris.insert( curPoly );								// Добавление треугольника в набор объекта
			}
			vecToTri.erase( vecKey );		// Удаление записи из vecToTri
		}
		vecQueue.pop();						// Удаление ключа из очереди
	}
}
//end of MeshObject

//MeshObstacle
MeshObstacle::MeshObstacle( map< Vector3D*, shared_ptr<set<shared_ptr<Poly>>>> &vecToTri )
{
	set<Vector3D *> points;
	queue< Vector3D* > vecQueue;
	{
	auto it = vecToTri.begin();
	points.insert( it->first );
	vecQueue.push( it->first );
	}
	while( !vecQueue.empty() )
	{
		Vector3D *vecKey = vecQueue.front();
		auto record =  vecToTri.find( vecKey );
		if( record != vecToTri.end() )
		{
			shared_ptr<set<shared_ptr<Poly>>> curSet = record->second;
			for( auto it = curSet->begin(); it != curSet->end(); it++ )
			{
				shared_ptr<Poly> curPoly = *it;
                                for( size_t i = 0; i < curPoly->v.size(); i++ )
				{
					auto itPair = points.insert( curPoly->v[i] );
					if( itPair.second == true )				// если была вставка нового вектора
						vecQueue.push( curPoly->v[i] );
				}
				tris.insert( curPoly );								// Добавление треугольника в набор объекта
			}
			vecToTri.erase( vecKey );		// Удаление записи из vecToTri
		}
		vecQueue.pop();						// Удаление ключа из очереди
	}
}

MeshObstacle::MeshObstacle( map<Vector3D*, shared_ptr<set<shared_ptr<Poly>>>> &vecToTri, const float &level, bool &isObstacle )
{
	set<Vector3D *> points;
	this->level = level;
	queue< Vector3D* > vecQueue;
	{
	auto it = vecToTri.begin();
	points.insert( it->first );
	vecQueue.push( it->first );
	}
	while( !vecQueue.empty() )
	{
		Vector3D *vecKey = vecQueue.front();
		auto record = vecToTri.find( vecKey );
		if( record != vecToTri.end() )
		{
			shared_ptr<set<shared_ptr<Poly>>> curSet = record->second;
			for( auto it = curSet->begin(); it != curSet->end(); it++ )
			{
				//shared_ptr<Poly> curPoly( new Poly( *(it->get()) ) );
				shared_ptr<Poly> curPoly = *it;
                                for( size_t i = 0; i < curPoly->v.size(); i++ )
				{
					//Vector3D vec( Vector3D( *(curPoly->v[i]) ) );
					auto itPair = points.insert( curPoly->v[i] );
					if( itPair.second == true )				// если была вставка нового вектора
						vecQueue.push( curPoly->v[i] );
					//curPoly->v[i] = const_cast<Vector3D*>( &(*itPair.first) );
				}
				//auto itPair = normals.insert( Vector3D( *(curPoly->normal) ) );
				//curPoly->normal = const_cast<Vector3D*>( &(*itPair.first) );
				if( curPoly->isBelongY(level) )
					tris.insert( curPoly );								// Добавление треугольника в набор объекта
			}
			vecToTri.erase( vecKey );		// Удаление записи из vecToTri
		}
		vecQueue.pop();						// Удаление ключа из очереди
	}
	
	if( tris.empty() )
	{
		isObstacle = false;
		return;
	}
	else
		isObstacle = true;

	FindEdges();
	setUpEdges();
	/**/
}

void MeshObstacle::FindEdges()
{
	for( auto it = tris.begin(); it != tris.end(); it++ )
	{
		auto curPoly = (*it);
		int n = curPoly->v.size();
		for( int i = 0; i < n; i++ )
		{
			Vector3D *v1, *v2;
			v1 = curPoly->v[i];
			if( i < n-1 )
				v2 = curPoly->v[i+1];
			else
				v2 = curPoly->v[0];
			
			Edge edg( v1, v2 );
			Vector3D vec;
			if( edg.GetXZ( level, vec ) )
			{
				auto curRow = edgeToTri.find( edg );
				if( curRow != edgeToTri.end() )
					curRow->second->insert( curPoly );
				else
				{
					shared_ptr<set<shared_ptr<Poly>>> curSet( new set<shared_ptr<Poly>> );
					curSet->insert( curPoly );
					edgeToTri.insert( pair< Edge, shared_ptr<set<shared_ptr<Poly>>>>( edg, curSet ) );
				}
			}
		}
	}

}

void MeshObstacle::setUpEdges()
{
    for( auto it = edgeToTri.begin(); it != edgeToTri.end(); it++ )
    {
        Vector3D vec;
        if( it->first.GetXZ( level, vec ) )
        {
            auto curSet = it->second;
            if( curSet->size() > 1)
            {
                for( auto it1 = curSet->begin(); it1 != curSet->end(); it1++ )
                {
                    //auto wh = it->first;
                    (*it1)->AddEdge( it->first );
                }
            }
        }
    }
}

bool MeshObstacle::HalfIntersectRay( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->HalfIntersectRay( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( P0 - point ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}

	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::HalfIntersectRay( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->HalfIntersectRay( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::FullIntersectRay( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->FullIntersectRay( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( P0 - point ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::FullIntersectRay( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->FullIntersectRay( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::HalfIntersect( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->HalfIntersect( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( P0 - point ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::HalfIntersect( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->HalfIntersect( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::FullIntersect( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->FullIntersect( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( point - P0 ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::FullIntersect( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->FullIntersect( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::HalfIntersectExcludingP0( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->HalfIntersectExcludingP0( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( P0 - point ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::HalfIntersectExcludingP0( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->HalfIntersectExcludingP0( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::FullIntersectExcludingP0( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->FullIntersectExcludingP0( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( P0 - point ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::FullIntersectExcludingP0( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->FullIntersectExcludingP0( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::FullIntersectExcludingBoth( const Vector3D &P0, const Vector3D &P1 )
{
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
		if( (*pl)->FullIntersectExcludingBoth( P0, P1 ) )
			return true;
	return false;
}

bool MeshObstacle::FullIntersectExcludingBoth( Vector3D &dst, shared_ptr<Poly> &face, const Vector3D &P0, const Vector3D &P1 )
{
	map< double, pair< shared_ptr<Poly>, Vector3D > > listPoints;
	for( auto pl = tris.begin(); pl != tris.end(); pl++ )
	{
		Vector3D point;
		if( (*pl)->FullIntersectExcludingBoth( point, P0, P1 ) )
		{
			shared_ptr<Poly> f = *pl;
			float h  = ( P0 - point ).Scalar();
			listPoints.insert( pair< double, pair< shared_ptr<Poly>, Vector3D > > ( h, pair< shared_ptr<Poly>, Vector3D >( f, point ) ) );
		}
	}
	if( !listPoints.empty() )
	{
		auto it = listPoints.begin();
		face = it->second.first;
		dst = it->second.second;
		return true;
	}
	return false;
}

bool MeshObstacle::SuperIntersect( const Vector3D &P0, const Vector3D &P1 )
{
    set< shared_ptr<Poly> > crossedTris;
    for( auto pl = tris.begin(); pl != tris.end(); pl++ )
    {
        Vector3D point;
        shared_ptr<Poly> f = *pl;
        if( f->FullIntersectExcludingP0( point, P0, P1 ) )
        {
            if( f->FullIntersectExcludingBoth( point, P0, P1 ) )
                return true;
            else
                crossedTris.insert(f);
        }
    }

    Vector3D vec;
    vec = P1 - DotScal( P1 - P0, 0.0000001f );
    for( auto it = crossedTris.begin(); it != crossedTris.end(); it++ )
    {
        Vector3D *norm = (*it)->normal;
        for( auto f = crossedTris.begin(); f != crossedTris.end(); f++ )
            if( (*f)->FullIntersectExcludingP0( vec, vec+*norm ) )
                return true;
    }
    return false;
}
// end of MeshObstacle
