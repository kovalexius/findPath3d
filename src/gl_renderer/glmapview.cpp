#include <iostream>
#include "glmapview.h"

void checkOpenGLerror()
{
    GLenum errCode;
    if((errCode=glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

char* filetobuf(char *file)
{
    FILE *fptr;
    long length;
    char *buf;
 
    fptr = fopen( file, "rb" ); // Открыть файл на чтение
    if ( !fptr ) // выйти если ошибка при инициализации fptr
        return NULL;
    fseek( fptr, 0, SEEK_END ); // Переместиться в конец файла
    length = ftell( fptr ); // Найти размер файла
    buf = ( char* ) malloc( length + 1 ); // Выделить буфер в который будет читаться файл
    fseek( fptr, 0, SEEK_SET ); // Переместиться в начало файла
    fread(buf, length, 1, fptr); // Прочитать содержимое файла в буфер
    fclose( fptr ); // Закрыть файл
    buf[length] = 0; // Символ конца буфера (иногда нужен ибо некоторые функции old C не принимают размер буфера)
 
    return buf; // Вернуть буфер
}

//! Функция печати лога шейдера
void shaderLog(unsigned int shader) 
{ 
  int   infologLen   = 0;
  int   charsWritten = 0;
  char *infoLog;

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

  if(infologLen > 1)
  { 
    infoLog = new char[infologLen];
    if(infoLog == NULL)
    {
      std::cout<<"ERROR: Could not allocate InfoLog buffer\n";
       exit(1);
    }
    glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
    std::cout<< "InfoLog: " << infoLog << "\n\n\n";
    delete[] infoLog;
  }
}

GLMapView::GLMapView(QWidget *parent): m_time(0), m_dtime(1)
{
	QGLFormat glFormat;
	glFormat.setRgba(true);
	glFormat.setRedBufferSize(8);
	glFormat.setGreenBufferSize(8);
	glFormat.setBlueBufferSize(8);
	glFormat.setAlphaBufferSize(8);
        ::QGLWidget(glFormat,parent);

	waterTriVBO = 0;
	waterNormVBO = 0;
	waterUvVBO = 0;
	objTriVBO = 0;
	objNormVBO = 0;
	objUvVBO = 0;

	znear=10.0;
	zfar=1000000.0;
	ref=1.0;		// Эталон непрозрачности =1

	xRot = 0.0;
	yRot = 0.5;
	R = 1000;
	shift = Vector3D(0,0,0);
	pos = Vector3D(0,0,0);
	xMov = 0.0;
	yMov = 0.0;

	beginP.setSize( 15.0f );
	beginP.setColor( 0.0, 1.0, 0.0 );

	endP.setSize( 15.0f );
	endP.setColor( 1.0, 1.0, 0.0 );

	setMouseTracking( true );

	timerFreqFps = new QTimer( this );
	connect( timerFreqFps, SIGNAL( timeout() ), this, SLOT( renderFrame() ) );
}

GLMapView::~GLMapView()
{
	if( waterTriVBO )
		glDeleteBuffers( 1, &waterTriVBO );

	if( waterNormVBO )
		glDeleteBuffers( 1, &waterNormVBO );

	if( objTriVBO )
		glDeleteBuffers( 1, &objTriVBO );

	if( objNormVBO )
		glDeleteBuffers( 1, &objNormVBO );

	if( objUvVBO )
		glDeleteBuffers( 1, &objUvVBO );
}

void GLMapView::AddObject( const shared_ptr<MeshObject> & ob )
{
	objects.insert( ob );	// deprecated если VBO
}

void GLMapView::AddObstacle( const shared_ptr<MeshObstacle> & ob )
{
	obstacles.insert( ob );
}

void GLMapView::AddWaterObject( const shared_ptr<MeshObject> &wr )
{
	waterObject = wr;
}

void GLMapView::AddWaterObstacle( const shared_ptr<MeshObstacle> &wr )
{
	waterObstacle = wr;
}



void GLMapView::RemoveObject( const shared_ptr<MeshObject> & ob )
{
	objects.erase( ob );
}

void GLMapView::StartRender()
{
	makeCurrent();										// Чтобы glNewList работал
	setFocusPolicy(Qt::StrongFocus);	// Установка политики фокусировки на виджет
	setMouseTracking(true);						// Включение режима mouse трекинга при котором виджет получает сигнал движения мышки даже когда ни одна кнопка мышки не нажата

	initWaterVBO();
	initObjectsVBO();
	m_begTime = clock();

	timerFreqFps->setInterval( 10 );
	timerFreqFps->start();
}

void GLMapView::initObjectsVBO()
{
	vector<float> verts;
	vector<float> norms;
	vector<float> uvs;

	for( auto it = objects.begin(); it != objects.end(); it++ )
	{
		auto curSet = (*it)->tris;
		for( auto itTri = curSet.begin(); itTri != curSet.end(); itTri++ )
		{
			auto curPoly = (*itTri).get();
			for( int i = 1; i < curPoly->v.size() - 1; i++ )
			{
				norms.push_back( curPoly->normal->x );
				norms.push_back( curPoly->normal->y );
				norms.push_back( curPoly->normal->z );

				norms.push_back( curPoly->normal->x );
				norms.push_back( curPoly->normal->y );
				norms.push_back( curPoly->normal->z );

				norms.push_back( curPoly->normal->x );
				norms.push_back( curPoly->normal->y );
				norms.push_back( curPoly->normal->z );

				verts.push_back( curPoly->v[0]->x );
				verts.push_back( curPoly->v[0]->y );
				verts.push_back( curPoly->v[0]->z );

				verts.push_back( curPoly->v[i]->x );
				verts.push_back( curPoly->v[i]->y );
				verts.push_back( curPoly->v[i]->z );

				verts.push_back( curPoly->v[i+1]->x );
				verts.push_back( curPoly->v[i+1]->y );
				verts.push_back( curPoly->v[i+1]->z );

				uvs.push_back( curPoly->tex[0].x);
				uvs.push_back( curPoly->tex[0].y);

				uvs.push_back( curPoly->tex[i].x);
				uvs.push_back( curPoly->tex[i].y);

				uvs.push_back( curPoly->tex[i+1].x);
				uvs.push_back( curPoly->tex[i+1].y);
			}
		}
	}

	// Удаление предыдущего буфера
	if( objTriVBO )
		glDeleteBuffers( 1, &objTriVBO );

	if( objNormVBO )
		glDeleteBuffers( 1, &objNormVBO );

	if( objUvVBO )
		glDeleteBuffers( 1, &objUvVBO );

	glGenBuffers( 1, &objTriVBO );
	glBindBuffer( GL_ARRAY_BUFFER, objTriVBO );
	countObjects = verts.size();
	glBufferData( GL_ARRAY_BUFFER, countObjects * sizeof(float), verts.data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &objNormVBO );
	glBindBuffer( GL_ARRAY_BUFFER, objNormVBO );
	glBufferData( GL_ARRAY_BUFFER, countObjects * sizeof(float), norms.data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &objUvVBO );
	glBindBuffer( GL_ARRAY_BUFFER, objUvVBO );
	countObjUV = uvs.size();
	glBufferData( GL_ARRAY_BUFFER, countObjUV * sizeof(float), uvs.data(), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	//-----------Создание шейдеров и компилиция----------
	GLchar *vertexSource, *fragmentSource;
	vertexSource = filetobuf("Land.vert");
	fragmentSource = filetobuf("Land.frag");
	GLuint vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	free(vertexSource);
	free(fragmentSource);
	glCompileShader(vertexShader);
	shaderLog(vertexShader);
	glCompileShader(fragmentShader);
	shaderLog(fragmentShader);
	//---------------------------------------------------
	//--------Создание программы шейдера и линковка------
	objShaderProgram = glCreateProgram();
	glAttachShader(objShaderProgram, vertexShader);
	glAttachShader(objShaderProgram, fragmentShader);
	checkOpenGLerror();
	glLinkProgram(objShaderProgram);	int link_ok;
	glGetProgramiv( waterShaderProgram, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
	{
	  std::cout << "error attach shaders \n";
	  return;
	}
	//---------------------------------------------------
	/*
	char buf[20];
	GLsizei length;
 	GLint size;
 	GLenum type;
	glGetActiveAttrib( objShaderProgram, 2, sizeof(buf), &length, &size, &type, buf );
	const char* attr_name = "m_normal";
	attribNormal = glGetAttribLocation( objShaderProgram, attr_name );
	*/
}

void GLMapView::initWaterVBO()
{
	vector<float> verts;
	vector< float > norms;
	vector<float> uvs;

	auto curSet = waterObject->tris;
	for( auto itTri = curSet.begin(); itTri != curSet.end(); itTri++ )
	{
		auto curPoly = (*itTri).get();
                for( size_t i = 1; i < curPoly->v.size() - 1; i++ )
		{
			norms.push_back( curPoly->normal->x );
			norms.push_back( curPoly->normal->y );
			norms.push_back( curPoly->normal->z );

			norms.push_back( curPoly->normal->x );
			norms.push_back( curPoly->normal->y );
			norms.push_back( curPoly->normal->z );

			norms.push_back( curPoly->normal->x );
			norms.push_back( curPoly->normal->y );
			norms.push_back( curPoly->normal->z );

			verts.push_back( curPoly->v[0]->x );
			verts.push_back( curPoly->v[0]->y );
			verts.push_back( curPoly->v[0]->z );

			verts.push_back( curPoly->v[i]->x );
			verts.push_back( curPoly->v[i]->y );
			verts.push_back( curPoly->v[i]->z );

			verts.push_back( curPoly->v[i+1]->x );
			verts.push_back( curPoly->v[i+1]->y );
			verts.push_back( curPoly->v[i+1]->z );

			uvs.push_back( curPoly->tex[0].x);
			uvs.push_back( curPoly->tex[0].y);

			uvs.push_back( curPoly->tex[i].x);
			uvs.push_back( curPoly->tex[i].y);

			uvs.push_back( curPoly->tex[i+1].x);
			uvs.push_back( curPoly->tex[i+1].y);
		}
	}

	// Удаление предыдущего буфера
	if( waterTriVBO )
		glDeleteBuffers( 1, &waterTriVBO );
	if( waterNormVBO )
		glDeleteBuffers( 1, &waterNormVBO );
	if( waterUvVBO )
		glDeleteBuffers( 1, &waterUvVBO );

	//Создание нового VBO и сохранение идентификатора VBO
	glGenBuffers( 1, &waterTriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, waterTriVBO);
	countWater = verts.size();
	glBufferData(GL_ARRAY_BUFFER, countWater * sizeof(float), verts.data(), GL_STATIC_DRAW);

	glGenBuffers( 1, &waterNormVBO );
	glBindBuffer( GL_ARRAY_BUFFER, waterNormVBO );
	glBufferData( GL_ARRAY_BUFFER, countWater * sizeof(float), norms.data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &waterUvVBO );
	glBindBuffer( GL_ARRAY_BUFFER, waterUvVBO );
	countWaterUV = uvs.size();
	glBufferData( GL_ARRAY_BUFFER, countWaterUV * sizeof(float), uvs.data(), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	//-----------Создание шейдеров и компилиция----------
	GLchar *vertexSource, *fragmentSource;
	vertexSource = filetobuf("Water.vert");
	fragmentSource = filetobuf("Water.frag");
	GLuint vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource( vertexShader, 1, (const GLchar**)&vertexSource, 0 );
	glShaderSource( fragmentShader, 1, (const GLchar**)&fragmentSource, 0 );
	free(vertexSource);
	free(fragmentSource);
	glCompileShader(vertexShader);
	shaderLog(vertexShader);
	glCompileShader(fragmentShader);
	shaderLog(fragmentShader);
	//---------------------------------------------------
	//--------Создание программы шейдера и линковка------
	waterShaderProgram = glCreateProgram();
	glAttachShader( waterShaderProgram, vertexShader );
	glAttachShader( waterShaderProgram, fragmentShader );
	checkOpenGLerror();
	glLinkProgram(  waterShaderProgram);
	//---------------------------------------------------
	timeLocation = glGetUniformLocation( waterShaderProgram, "time" );

}

bool GLMapView::getPoints( Vector3D &begin, Vector3D &end )
{
	if( beginP.isExist && endP.isExist )
	{
		begin = beginP.position;
		end = endP.position;
		return true;
	}
	return false;
}

void GLMapView::AddCheckPoint( const Vector3D & vec)
{
	checkPoints.push_back( vec );
}

void GLMapView::removeCheckPoints()
{
	checkPoints.clear();
}

void GLMapView::renderFrame()
{
	calculateCamera();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat lightPos0[4]={ pCamera.x, pCamera.y, pCamera.z, 1.0 };
	Vector3D dir = tCamera - pCamera;
	GLfloat lightDir0[3]={ dir.x, dir.y, dir.z };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);				//
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,lightDir0);	//

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pCamera.x,pCamera.y,pCamera.z,tCamera.x,tCamera.y,tCamera.z,upCamera.x,upCamera.y,upCamera.z);	// Установка камеры
	glGetDoublev( GL_MODELVIEW_MATRIX, vMatrix.getPtr() ); 
	doOpacityRender();

	updateGL();
}

void GLMapView::paintGL()
{
	
}

void GLMapView::initializeGL()
{
	bool rezult;
	rezult = Init_GLVERSION15();
	rezult = Init_VertexBufferObject();
	rezult = Init_VertexArray();
	rezult = Init_ShaderObjects();

	calculateCamera();
	GLfloat lightPos0[4]={ pCamera.x, pCamera.y, pCamera.z, 1.0 };
	Vector3D dir = tCamera - pCamera;
	GLfloat lightDir0[3]={ dir.x, dir.y, dir.z };
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);				//
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,lightDir0);	//
	//glEnable(GL_LIGHTING);		// Свет
	//glEnable(GL_LIGHT0);		// 			
	glEnable(GL_DEPTH_TEST);	// Тест глубины
	//glEnable(GL_CULL_FACE);		// Уничтожение back-facing полигонов
	//glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
	glCullFace(GL_FRONT_AND_BACK);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);	// Разрешить текстуры
	//glFrontFace(GL_CCW);			
	glEnable(GL_ALPHA_TEST);	// Включение альфа теста
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);	// Параметры смешивания
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);							// Присвоение цвета окну, но не помещает на экран
	glMatrixMode(GL_PROJECTION);								//
	glLoadIdentity();														//	Установка произвольной перспективной
	glFrustum(-1.333,1.333,-1.0,1.0,znear,zfar);	//	проекции
	glGetDoublev( GL_PROJECTION_MATRIX, pMatrix.getPtr() ); //returns matrix as float array
}

void GLMapView::calculateCamera()
{
	GetXYZ();
	GetDXYZ();

	pCamera=pos+shift;
	tCamera=shift;
	upCamera=up;
}

void GLMapView::mouseMoveEvent(QMouseEvent *event)
{
	if(event->modifiers()&Qt::ShiftModifier)
	{
		if(xRot>-6.28||xRot<6.28)
			xRot+=-(float)(event->x()-mPos.x())*0.0017444;
		else
			xRot=(float)6.28;
		if(yRot>-6.28||yRot<6.28)
			yRot+=(float)(event->y()-mPos.y())*0.0017444;
		else
			yRot=(float)6.28;
		GetXYZ();
		
		setCursor(Qt::SizeAllCursor);
	}
	if(event->modifiers()&Qt::AltModifier)
	{
		xMov=(float)(-event->x()+mPos.x())/4;
		yMov=(float)(event->y()-mPos.y())/4;
		GetDXYZ();
		xMov=0;
		yMov=0;

		setCursor(Qt::ClosedHandCursor);
	}
	mPos=event->pos();
}

void GLMapView::keyReleaseEvent( QKeyEvent * event )
{
	setCursor( Qt::ArrowCursor );
}

void GLMapView::enterEvent(QEvent *event)
{
	setFocus(Qt::OtherFocusReason);		// Установка фокуса на виджет при наезде мышки на виджет
}

void GLMapView::mousePressEvent( QMouseEvent *event )
{
	if( !waterObstacle )
		return;

	if( event->button() == Qt::LeftButton )
	{
		auto x = event->x();
		auto y = height() - event->y();

		double xObj, yObj, zObj;
		gluUnProject( x, y, 0.0, vMatrix.getPtr(), pMatrix.getPtr(), view, &xObj, &yObj, &zObj );
		Vector3D P1( xObj, yObj, zObj );
		Vector3D P0( pCamera.x, pCamera.y, pCamera.z );
		Vector3D dst;
		shared_ptr<Poly> face;
		if( waterObstacle->FullIntersectRay( dst, face, P0, P1 ) )
			beginP.setPosition( Vector3D( dst.x, dst.y, dst.z ) );
		else
			beginP.removePoint();
	}

	if( event->button() == Qt::RightButton )
	{
		auto x = event->x();
		auto y = height() - event->y();

		double xObj, yObj, zObj;
		gluUnProject( x, y, 0.0, vMatrix.getPtr(), pMatrix.getPtr(), view, &xObj, &yObj, &zObj );
		Vector3D P1( xObj, yObj, zObj );
		Vector3D P0( pCamera.x, pCamera.y, pCamera.z );
		Vector3D dst;
		shared_ptr<Poly> face;
		if( waterObstacle->FullIntersectRay( dst, face, P0, P1 ) )
			endP.setPosition( Vector3D( dst.x, dst.y, dst.z ) );
		else
			endP.removePoint();
	}
}

void GLMapView::wheelEvent(QWheelEvent *e)
{	
	e->delta()>0?R+=R*0.05f:R-=R*0.05f;
	GetXYZ();
}

void GLMapView::resizeGL( int width, int height )
{
	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT, view);
}

void GLMapView::drawObjects()
{
	glEnableClientState( GL_NORMAL_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, objNormVBO );
	glNormalPointer( GL_FLOAT, 0, NULL );

	glEnableClientState( GL_VERTEX_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, objTriVBO );
	glVertexPointer( 3, GL_FLOAT, 0, NULL);

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, objUvVBO );
	glTexCoordPointer( 2, GL_FLOAT, 0, NULL );

	glUseProgram( objShaderProgram );
	glDrawArrays( GL_TRIANGLES, 0, countObjects );
	glUseProgram(0);

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	/*
	//GLfloat color[4] = { 0.9, 0.0, 0.0, 1.0 };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	for( auto it = objects.begin(); it != objects.end(); it++ )
	{
		auto curSet = (*it)->tris;
		for( auto itTri = curSet.begin(); itTri != curSet.end(); itTri++ )
		{
			auto curPoly = (*itTri).get();
			for( int i = 1; i < curPoly->v.size() - 1; i++ )
			{
				glBegin( GL_POLYGON );
				glNormal3f( curPoly->normal->x, curPoly->normal->y, curPoly->normal->z );
				glVertex3f(	curPoly->v[0]->x, curPoly->v[0]->y, curPoly->v[0]->z );
				glVertex3f(	curPoly->v[i]->x, curPoly->v[i]->y, curPoly->v[i]->z );
				glVertex3f(	curPoly->v[i+1]->x, curPoly->v[i+1]->y, curPoly->v[i+1]->z );
				glEnd();
			}
		}
	}
	*/
}

void GLMapView::drawObstacles()
{
	GLfloat color[4] = { 0.9, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	for( auto it = obstacles.begin(); it != obstacles.end(); it++ )
	{
		auto curSet = (*it)->tris;
		for( auto itTri = curSet.begin(); itTri != curSet.end(); itTri++ )
		{
			auto curPoly = (*itTri).get();
			for( int i = 1; i < curPoly->v.size() - 1; i++ )
			{
				glBegin( GL_POLYGON );
				glNormal3f( curPoly->normal->x, curPoly->normal->y, curPoly->normal->z );
				glVertex3f(	curPoly->v[0]->x, curPoly->v[0]->y, curPoly->v[0]->z );
				glVertex3f(	curPoly->v[i]->x, curPoly->v[i]->y, curPoly->v[i]->z );
				glVertex3f(	curPoly->v[i+1]->x, curPoly->v[i+1]->y, curPoly->v[i+1]->z );
				glEnd();
			}
		}
	}
}

void GLMapView::drawObstaclesEdgesAndTri()
{
	GLfloat colorTri[4] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat colorEdge[4] = { 0.0, 1.0, 0.0, 1.0 };
	for( auto it = obstacles.begin(); it != obstacles.end(); it++ )
	{
		auto curMap = (*it)->edgeToTri;
		for( auto itMap = curMap.begin(); itMap != curMap.end(); itMap++ )
		{
			auto curSet = itMap->second;
			int number = curSet->size();
			if( curSet->size() <= 2)
			{
				Edge *edg = const_cast<Edge*>( &(itMap->first) );
				glDisable(GL_LIGHTING);		// Свет
				glDisable(GL_LIGHT0);			// отключить
				glEnable(GL_POINT_SMOOTH);
				glLineWidth( 4.0 );
				glBegin( GL_LINE_STRIP );
				glColor3f( colorEdge[0], colorEdge[1], colorEdge[2] );
				glVertex3f( edg->v1->x, edg->v1->y, edg->v1->z );
				glVertex3f( edg->v2->x, edg->v2->y, edg->v2->z );
				glEnd();
				colorEdge[1] -= 0.3;
				if( colorEdge[1] < 0.2 )
					colorEdge[1] = 1.0;
				glEnable(GL_LIGHTING);		// Свет
				glEnable(GL_LIGHT0);			// включить назад

				
				for( auto itTri = curSet->begin(); itTri != curSet->end(); itTri++ )
				{
					auto curPoly = *itTri;
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorTri);
                                        for( size_t i = 1; i < curPoly->v.size() - 1; i++ )
					{
						glBegin( GL_POLYGON );
						glNormal3f( curPoly->normal->x, curPoly->normal->y, curPoly->normal->z );
						glVertex3f(	curPoly->v[0]->x, curPoly->v[0]->y, curPoly->v[0]->z );
						glVertex3f(	curPoly->v[i]->x, curPoly->v[i]->y, curPoly->v[i]->z );
						glVertex3f(	curPoly->v[i+1]->x, curPoly->v[i+1]->y, curPoly->v[i+1]->z );
						glEnd();
					}
					colorTri[0] -= 0.3;
					if( colorTri[0] < 0.2 )
						colorTri[0] = 1.0;
				}
			}
		}
	}
}

void GLMapView::drawWater()
{
	GLfloat color[4] = { 0.0, 0.0, 0.8, 0.5 };
	//glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );

	glEnableClientState( GL_VERTEX_ARRAY );
	glBindBuffer(GL_ARRAY_BUFFER, waterTriVBO);
	glVertexPointer( 3, GL_FLOAT, 0, NULL );

	glEnableClientState( GL_NORMAL_ARRAY );
	glBindBuffer(GL_ARRAY_BUFFER, waterNormVBO);
	glNormalPointer( GL_FLOAT, 0, NULL );

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, waterUvVBO );
	glTexCoordPointer( 2, GL_FLOAT, 0, NULL );

	glUseProgram( waterShaderProgram );
	if(m_time > 1000)
	{
	  m_time = 1000;
	  m_dtime = -m_dtime;
	}
	if(m_time < 0)
	{
	  m_time = 0;
	  m_dtime = -m_dtime;
	}
	m_time += m_dtime;
	//auto time = clock() - m_begTime;
	//if( time > 20000 )
	//	m_begTime = clock();
	glUniform1f( timeLocation, (GLfloat)m_time/1000.0f );
	glDrawArrays( GL_TRIANGLES, 0, countWater );
	glUseProgram( 0 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

void GLMapView::drawCheckPoints()
{
	glDisable(GL_LIGHTING);		// Свет
	glDisable(GL_LIGHT0);			// отключить
	glEnable(GL_POINT_SMOOTH);
	if( beginP.isExist )
	{
		glPointSize( beginP.pSize );
		glBegin(GL_POINTS);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, beginP.color);
			glColor3f( beginP.color[0], beginP.color[1], beginP.color[2] );
			glVertex3f( beginP.position.x, beginP.position.y, beginP.position.z );
		glEnd();
	}
	if( endP.isExist )
	{
		glPointSize( endP.pSize );
		glBegin(GL_POINTS);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, endP.color);
			glColor3f( endP.color[0], endP.color[1], endP.color[2] );
			glVertex3f( endP.position.x, endP.position.y, endP.position.z );
		glEnd();
	}
	glEnable(GL_LIGHTING);		// Свет
	glEnable(GL_LIGHT0);			// включить назад
}

void GLMapView::drawPath()
{
	glDisable(GL_LIGHTING);		// Свет
	glDisable(GL_LIGHT0);			// отключить

	glLineWidth( 10.0 );

	GLfloat colorPath[4] = { 0.0, 1.0, 0.0, 1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorPath );
	glColor3f( colorPath[0], colorPath[1], colorPath[2] );
	glBegin( GL_LINE_STRIP );
	for( auto it = checkPoints.begin(); it != checkPoints.end(); it++ )
	{
		glVertex3f( it->x, it->y, it->z );
	}
	glEnd();

	glPointSize( 18.0 );

	GLfloat colorPoints[4] = { 0.8, 0.8, 0.8, 1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorPoints );
	glColor3f( colorPoints[0], colorPoints[1], colorPoints[2] );
	glBegin(GL_POINTS);
	for( auto it = checkPoints.begin(); it != checkPoints.end(); it++ )
	{
		glVertex3f( it->x, it->y, it->z );
	}
	glEnd();

	auto n = checkPoints.size();

	glEnable(GL_LIGHTING);		// Свет
	glEnable(GL_LIGHT0);			// включить назад
}

void GLMapView::doOpacityRender()
{
	glAlphaFunc(GL_EQUAL,ref);	
	glDisable(GL_BLEND);	// Отключить смешение цветов
	drawCheckPoints();
	drawObjects();
	drawPath();
	glAlphaFunc(GL_LESS,ref);
	glEnable(GL_BLEND);	// Включить смешение цветов для прозрачности
	drawWater();
	//.............Возвращение в исходное.............
	glAlphaFunc(GL_EQUAL,ref);	
	glDisable(GL_BLEND);	// Отключить смешение цветов
	//................................................

	glFlush();
}

void GLMapView::GetXYZ(void)
{
	pos.y=sin(yRot)*R;
	pos.z=cos(xRot)*cos(yRot)*R;
	pos.x=sin(xRot)*cos(yRot)*R;

	up.y=cos(-yRot);
	up.x=sin(xRot)*sin(-yRot);
	up.z=cos(-xRot)*sin(-yRot);
}

void GLMapView::GetDXYZ(void)
{
	shift.x+=cos(xRot)*xMov+sin(xRot)*sin(-yRot)*yMov;
	shift.z+=sin(-xRot)*xMov+sin(-yRot)*cos(xRot)*yMov;
	shift.y+=cos(yRot)*yMov;
}
