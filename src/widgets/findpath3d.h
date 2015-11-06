#ifndef FINDPATH3D_H
#define FINDPATH3D_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QtGui/QApplication>
#include "ui_findpath3d.h"
#include "../gl_renderer/glmapview.h"
#include "../kfp_algo/Scene.h"

class findPath3D : public QMainWindow
{
	Q_OBJECT
public:
	findPath3D(QWidget *parent = 0, Qt::WFlags flags = 0);
	~findPath3D();

private:
	QWidget *centralWidget;
	Ui::findPath3DClass ui;
	GLMapView *vw;
	Scene *scn;
	QString m_filePath;
public slots:
	void onMapOpen();
	void onMapCreate();
	void onFindPath();
};

#endif // FINDPATH3D_H
