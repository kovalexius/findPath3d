#include "findpath3d.h"

using namespace std;

findPath3D::findPath3D(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	centralWidget = new QWidget;

	scn = 0;
	vw = new GLMapView(this);

	centralWidget->setLayout( ui.mainLayout );
	setCentralWidget( centralWidget );
	ui.sa->setWidget( vw );
	ui.sa->setWidgetResizable( true );

	connect( ui.pButtonOpen, SIGNAL( clicked() ), this, SLOT( onMapOpen() ) );
	connect( ui.pButtonGenerate, SIGNAL( clicked()), this, SLOT(onMapCreate()) );
	connect( ui.pButtonFind, SIGNAL( clicked() ), this, SLOT( onFindPath() ) );
}

findPath3D::~findPath3D()
{
	delete vw;
}


void findPath3D::onMapOpen()
{
	QString str;
	m_filePath = QFileDialog::getOpenFileName(this,
     tr("Open MAP"), "", tr("MAP (*.map)"));
	if(!m_filePath.isEmpty())
		ui.label->setText( m_filePath );
}

void findPath3D::onFindPath()
{
	vw->removeCheckPoints();
	Vector3D begin, end;
	
	if( vw->getPoints( begin, end ) )
	{
		list<Vector3D> lVecs;
		if( scn->FindPath( lVecs, begin, end ) )
		{
			for( auto it = lVecs.begin(); it != lVecs.end(); it++ )
				vw->AddCheckPoint ( *it );
		}
	}

}

void findPath3D::onMapCreate()
{
    QString str = ui.lineEdit->text();
    float h = str.toFloat();

    std::wstring filePath( (wchar_t*)m_filePath.unicode());
    scn = new Scene( filePath.c_str(), h );

    for( auto it = scn->objects.begin(); it != scn->objects.end(); it++ )
		vw->AddObject( *it );
	for( auto it = scn->obstacles.begin(); it != scn->obstacles.end(); it++ )
		vw->AddObstacle( *it );

	vw->AddWaterObject( scn->waterMesh );
	vw->AddWaterObstacle( scn->waterObstacle );
	vw->StartRender();
}