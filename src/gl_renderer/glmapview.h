#ifndef GLMAPVIEW_H
#define GLMAPVIEW_H

#ifdef _WIN32
#include <QGLWidget>
#else
#include <QGLWidget>
//#include <qgl.h>
#endif

#include <QtCore>
#include <QtGui>
#include <list>
#include <ctime>

#include "../kfp_algo/Geometry.h"
#include "../kfp_algo/MeshObject.h"

#include <GL/gl.h>
#include "glExtInit.h"

using namespace std;

struct CheckPoint
{
	CheckPoint() 
	{ 
		isExist = false; 
		pSize = 1.0f; 
		color[0] = 0.5; 
		color[1] = 0.5; 
		color[2] = 0.5; 
		color[3] = 1.0; 
	}
	void setSize ( const float &size ) { pSize = size; }
	void setPosition ( const Vector3D & pos ) { isExist = true; position = pos; }
	void setColor( const float &red, const float &green, const float &blue ) 
	{
		color[0] = red;
		color[1] = green;
		color[2] = blue;
	}
	void removePoint() { isExist = false; }
	Vector3D position;
	float pSize;
	bool	isExist;
	float color[4];
};

class GLMapView : public QGLWidget
{
	Q_OBJECT
public:
	GLMapView(QWidget *parent = 0);
	~GLMapView();

	void AddObject( const shared_ptr<MeshObject> & ob );
	void AddObstacle( const shared_ptr<MeshObstacle> & ob );
	void RemoveObject( const shared_ptr<MeshObject> & ob );
	void AddWaterObject( const shared_ptr<MeshObject> &wr );
	void AddWaterObstacle( const shared_ptr<MeshObstacle> &wr );
	void initWaterVBO();
	void initObjectsVBO();
	void StartRender();
	bool getPoints( Vector3D &begin, Vector3D &end );
	void AddCheckPoint( const Vector3D & vec);
	void removeCheckPoints();
private:
	GLuint waterTriVBO;
	GLuint waterNormVBO;
	GLuint waterUvVBO;
	GLuint waterShaderProgram;
	GLint  timeLocation;
	clock_t	m_begTime;
	GLuint objTriVBO;
	GLuint objNormVBO;
	GLuint objUvVBO;
	GLuint objShaderProgram;
	GLsizei countWaterUV;
	GLsizei countWater;
	GLsizei countObjects;
	GLsizei countObjUV;

	GLfloat	znear;	// ������� � �������
	GLfloat zfar;  // ��������� ���������
	GLclampf ref;			// ������ �������������� ����� ��������, ref=1 - ��������� ������������
	Vector3D up;	// ������� ������ ����� ������
	Vector3D pos;	// ������� ������� ������
	Vector3D shift; // �������� ������ � ����
	Vector3D pCamera;		//��� ������
	Vector3D tCamera;		//���� �������
	Vector3D upCamera;		//���� ������� ���� ������
	float	R;			//	��������� �� ������ ������
	float	xRot;	//	���� �������� � ��������� XZ � ��������
	float	yRot;	//	���� �������� � ��������� YZ � ��������
	float	xMov;	// ��������� ��������
	float	yMov;	// ����������� ������ (������� Alt)
	QTimer *timerFreqFps;
	QPoint mPos;		// ��������� ������� ����
	Matrix4x4 pMatrix;		// ������� �������� �����������
	Matrix4x4 vMatrix;		// ������� �������
	GLint view[4];
	CheckPoint beginP;
	CheckPoint endP;
	list< Vector3D > checkPoints;
	
	int m_time;
	int m_dtime;

	set< shared_ptr< MeshObject > > objects;		// ��� ������������
	set< shared_ptr< MeshObstacle > > obstacles;	// � ����� ��������
	shared_ptr< MeshObject > waterObject;
	shared_ptr< MeshObstacle > waterObstacle;    // ����� ������ �� ���� �������
	virtual void paintGL();
	virtual void initializeGL();
	virtual void mouseMoveEvent( QMouseEvent *event ); // ���� ������� ������
	virtual void mousePressEvent( QMouseEvent *event ); // ���� �������� ������ � �������� 
	virtual void keyReleaseEvent( QKeyEvent * event );
	virtual void enterEvent(QEvent *event); // ���� ����� ������� �� ������
	virtual void wheelEvent( QWheelEvent *e );
	virtual void resizeGL( int width, int height ); // ���� �������� ������ �������;
	void GetXYZ();
	void GetDXYZ();
	void calculateCamera();
	void doOpacityRender();
	void drawObjects();
	void drawObstacles();
	void drawObstaclesEdgesAndTri();	// ��������� �������
	void drawWater();
	void drawCheckPoints();
	void drawPath();
public slots:
	void renderFrame();

};

#endif // GLMAPVIEW_H
