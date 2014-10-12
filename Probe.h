#pragma once
#include <QtGui>
#include <QGLWidget>
#include "heartDefines.h"
class Probe : public QObject
{
public:
	Probe(QObject *parent);
	~Probe(void);

    void setPosition(const QPoint &position);
    void setColor(const QColor &color);
	void draw();
    QColor color() const;
    QPoint position() const;

    void setLabel(const QString &toolTip);
    QString label() const;

	void hide();
	void show();
	bool isHidden;
   	CELL_TYPE type;
    QPoint myPosition;
private:
    QPixmap *pix;
    QColor myColor;
    QString myLabel;
};
