#include "View\SimViewState.h"
#include "View\glAtrium.h"


SimViewState::SimViewState()
{
	_scale = 0.1;
	// Initialize them at bottom of setupGL
	_quat = QQuaternion(1, 0, 0, 0);
	_quatStart = QQuaternion(1, 0, 0, 0);

}


SimViewState::~SimViewState()
{
}

void SimViewState::ChangeState(glAtrium* view, SimViewState* s)
{
	view->ChangeState(s);
}
const double SimViewState::getRadius(){ return cursorRadius; }
void  SimViewState::setPalette(const DISP_PALETTE pal)
{
	_palette = pal;
	switch (pal)
	{
	case DP_GRAY:
		colorMap = &grayMap;
		break;
	case DP_HOTTOCOLD:
		colorMap = &hotToColdMap;
		break;
	case DP_HOT:
		colorMap = &hotMap;
		break;
	case DP_COLD:
		colorMap = &coldMap;
		break;
	}
}

void SimViewState::computeIncremental(Vector3 last, Vector3 next)
{
	Vector3 vec = last.cross(next);
	QVector3D axis = QVector3D(vec.x, vec.y, vec.z);
	float dott = last.dot(next);
	float angle = 30 * acosf(dott);


	QQuaternion Q_rot = QQuaternion::fromAxisAndAngle(axis, angle * 2);
	Q_rot.normalize();

	_quat = Q_rot * _quatStart;

}

void SimViewState::paintScale(glAtrium* view)
{
	double scaledisplay = 5.0;
	GLfloat sizeOfScale = abs(scaledisplay * view->frustrumSize * (view->nearClippingPlaneDistance+0.5f) / (view->distanceToCamera));
	
	char buff[20];
	sprintf(buff, "%1.1f mm", scaledisplay);
	std::string str(buff);


	float f[1];
	GLfloat col_w[] = { 1.0, 1.0, 1.0, 0.7f };
	glGetFloatv(GL_LINE_WIDTH, f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	glColor3fv(col_w);
	glVertex3f(0.01f, -sizeOfScale / 2, 0.0f);
	glVertex3f(0.01f, sizeOfScale / 2, 0.0f);
	glVertex3f(-0.01f, -sizeOfScale / 2, 0.0f);
	glVertex3f(-0.01f, sizeOfScale / 2, 0.0f);

	for (short k = 0; k <= 5; ++k)
	{
		glVertex3f(0.01f, -sizeOfScale / 2 + (k*sizeOfScale/5), 0.0f);
		glVertex3f(0.02f, -sizeOfScale / 2 + (k*sizeOfScale / 5), 0.0f);
		glVertex3f(-0.01f, -sizeOfScale / 2 + (k*sizeOfScale / 5), 0.0f);
		glVertex3f(-0.02f, -sizeOfScale / 2 + (k*sizeOfScale / 5), 0.0f);
	}
	
	glEnd();
	glColor3fv(col_w);
	view->renderText(0.02f, 0.02f, 0.0f, str.c_str());
}