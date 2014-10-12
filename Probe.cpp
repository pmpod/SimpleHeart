#include "Probe.h"

Probe::Probe(QObject *parent)
{
	myColor = Qt::yellow;
	isHidden = false;
}

Probe::~Probe(void)
{
}
//------------------------------
void Probe::draw()
{
	//if(!isHidden)
	//{
	//	logo->draw();	
	//}

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);
		double rr = myColor.red();
		double gg = myColor.green();
		double bb = myColor.blue();
		//HSVtoRGB(coloura,1.0,1.0,&rr,&gg,&bb);
		glColor3f(rr, gg, bb);
		double deltaX=0.3;
		double deltaY=0.3;
		glBegin(GL_TRIANGLES);
		
		glColor3f(0, 0, 0);
				glVertex3f((0.05)*deltaX, deltaY*(0.05), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((0.2)*deltaX, deltaY*(0.13), 0.06f);
				glVertex3f((0.13)*deltaX, deltaY*(0.2), 0.06f);
				
		glColor3f(0, 0, 0);
				glVertex3f((0.05)*deltaX, deltaY*(0.05), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((0.13)*deltaX, deltaY*(-0.1), 0.06f);
				glVertex3f((0.2)*deltaX, deltaY*(-0.03), 0.06f);
				
		glColor3f(0, 0, 0);
				glVertex3f((0.05)*deltaX, deltaY*(0.05), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((-0.1)*deltaX, deltaY*(-0.03), 0.06f);
				glVertex3f((-0.03)*deltaX, deltaY*(-0.1), 0.06f);
				
		glColor3f(0, 0, 0);
				glVertex3f((0.05)*deltaX, deltaY*(0.05), 0.6f);
		glColor3f(rr, gg, bb);
				glVertex3f((-0.03)*deltaX, deltaY*(0.2), 0.6f);
				glVertex3f((-0.1)*deltaX, deltaY*(0.13), 0.6f);
		glEnd();
		glLineWidth(f[0]);


}
//------------------------------
void Probe::hide()
{
	isHidden = true;
}
//------------------------------
void Probe::show()
{
	isHidden = false;
}
//------------------------------
QString Probe::label() const
{
    return myLabel;
}

void Probe::setLabel(const QString &label)
{
    myLabel = label;
}
QPoint Probe::position() const
{
    return myPosition;
}

QColor Probe::color() const
{
    return myColor;
}

void Probe::setPosition(const QPoint &position)
{
    myPosition = position;
}

void Probe::setColor(const QColor &color)
{
    myColor = color;
}

