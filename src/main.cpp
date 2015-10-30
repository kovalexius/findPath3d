#include <QtGui/QApplication>
#include <iostream>

#include "widgets/findpath3d.h"
#include "kfp_algo/Geometry.h"

class AA
{
public:
    AA(){ std::cout << "AA" << std::endl;}
    int i;
    int j;
};

int main(int argc, char *argv[])
{
    int i;
    //int k(0);

    AA *aa = (AA*) malloc( sizeof(AA) );
    new (aa) AA();

    Vector3D A( -2, 1, 4 );
    Vector3D B( -2, 3, 1 );

    Vector3D C( 0, 1, -4 );
    Vector3D D( 1, -1, 2 );

    double res = twoDirectDistanceFast( A, B, C, D );

    A = Vector3D( 5, 0, 0 );
    B = Vector3D( 0, 0, 0 );

    C = Vector3D( 0, 5, 0 );
    D = Vector3D( 0, 0, 0 );

    res = twoDirectDistanceFast( A, B, C, D );

    A = Vector3D( 0, 0, 0 );
    B = Vector3D( 5, 0, 0 );

    C = Vector3D( 0, 0, 6 );
    D = Vector3D( 5, 0, 6 );

    res = twoDirectDistanceFast( A, B, C, D );

    QApplication a(argc, argv);
    findPath3D w;
    w.show();
    return a.exec();
}
