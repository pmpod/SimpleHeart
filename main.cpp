#include "simpleheart.h"
#include <QtGui/QApplication>
#include <QPlastiqueStyle>
#include <QWidget> 

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SimpleHeart w;
	QApplication::setStyle(new QPlastiqueStyle);
	w.showFullScreen();
	w.show();
	return a.exec();
}
