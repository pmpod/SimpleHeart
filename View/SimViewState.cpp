#include "View\SimViewState.h"
#include "View\glAtrium.h"


SimViewState::SimViewState()
{
	cursorRadius = 0.5;
	_scale = 0.1;
	// Initialize them at bottom of setupGL
	_quat = QQuaternion(1, 0, 0, 0);
	_quatStart = QQuaternion(1, 0, 0, 0);
	_displayConduction = true;
	_dataDisplayMode = DM_POTENTIAL;//POTENTIAL
}


SimViewState::~SimViewState()
{
}

void SimViewState::ChangeState(glAtrium* view, SimViewState* s)
{
	view->ChangeState(s);
}

void SimViewState::setMode(const int mode)
{
	_dataDisplayMode = mode;
	vmax = 0;
	vmin = 0;
	previous_vmin = 0;
	previous_vmax = 0;
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
void SimViewState::paintLegend(glAtrium* view)
{
	glCallList(view->displayListIndex);
	GLfloat widthL = view->frustrumSize*_scale*0.5;
	GLfloat heightL = view->frustrumSize*_scale*0.2;
	QString str;
	switch (_dataDisplayMode)
	{
	case DM_POTENTIAL:
		view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, "-75 mV", QFont(), view->displayListIndex);
		view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, "25 mV", QFont(), view->displayListIndex);
		break;
	case DM_CSD:
		str = QString::number(vmin, 'f', 2) + " [mA/cm^2]";
		view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		str = QString::number(vmax, 'f', 2) + " [mA/cm^2]";
		view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		break;
	case DM_ACT_TIME:
		str = QString::number(vmin, 'f', 2) + " [ms]";
		view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		str = QString::number(vmax, 'f', 2) + " [ms]";
		view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		break;
	case DM_PPIR_TCL:
		str = QString::number(vmin, 'f', 2) + " [ms]";
		view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		str = QString::number(vmax, 'f', 2) + " [ms]";
		view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		break;
	case DM_ELECTROGRAM:
		str = QString::number(vmin, 'f', 2) + " [mV]";
		view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		str = QString::number(vmax, 'f', 2) + " [mV]";
		view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, str, QFont(), view->displayListIndex);
		break;
	}
}
void SimViewState::prepareLegend(glAtrium* view)
{

	float f[1];
	GLfloat widthL = view->frustrumSize*_scale*0.5;
	GLfloat heightL = view->frustrumSize*_scale*0.2;

	GLfloat col_w[] = { 1.0, 1.0, 1.0, 1.f };
	GLfloat col_g[] = { .5f, .5f, .5f, .5f };
	GLfloat col[] = { 0.0, 0.0, 1.0, 1.f };

	GLfloat ccol;
	// compile the display list, store a triangle in it
	glNewList(view->displayListIndex, GL_COMPILE);

	glBegin(GL_QUAD_STRIP);
	for (GLfloat i = -16.0; i <= 16.0; ++i)
	{
		ccol = -i;

		colorMap(ccol, -16.0, 16.0, col[0], col[1], col[2]);
		glColor3f(col[0], col[1], col[2]);
		glVertex3f(widthL, i*heightL, 0.0f);
		glVertex3f(-widthL, i*heightL, 0.0f);

	}
	glEnd();
	glBegin(GL_LINES);
	glColor3fv(col_g);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, col_g);
	for (GLfloat i = -15.0; i <= 15.0; ++i)
	{
		glVertex3f(-widthL, i*heightL, 0.0f);
		glVertex3f(widthL, i*heightL, 0.0f);

	}
	glEnd();

	glBegin(GL_LINES);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, col_w);
	glColor3fv(col_w);
	glVertex3f(-widthL, -16.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, -16.0*heightL, 0.0f);
	glVertex3f(-widthL, 0.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, 0.0*heightL, 0.0f);
	glVertex3f(-widthL, 16.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, 16.0*heightL, 0.0f);
	glEnd();

	glEndList();

}
void SimViewState::paintModel(glAtrium* view)
{
	CardiacMesh *msh = view->linkToMesh;
	std::vector<Oscillator*> oscs = view->linkToMesh->m_mesh;
	int vertexNumber = msh->m_vertexList.size();

	GLfloat val1;
	EpStimulator * stimulator;
	stimulator = view->linkToMachine->stimulator;
	switch (_dataDisplayMode)
	{
	case DM_POTENTIAL:
		vmin = msh->minPotential;
		vmax = msh->maxPotential;
		break;
	case DM_CSD:
		vmax = previous_vmax;
		vmin = previous_vmin;
		previous_vmin = 0.95*previous_vmin;
		previous_vmax = 0.95*previous_vmax;
		break;
	case DM_CUR1:
		vmin = 0;
		vmax = 1;
		break;
	case DM_CUR2:
		vmin = 0;
		vmax = 1;
		break;
	case DM_ELECTROGRAM:
		vmin = msh->minElectrogram;
		vmax = msh->maxElectrogram;
		break;
	case DM_ACT_TIME: //ActivationTime_Wzglednie
		vmax = previous_vmax;
		vmin = previous_vmin;
		previous_vmin = DBL_MAX;
		previous_vmax = -DBL_MAX;
		break;

	case DM_PPIR_TCL: //ActivationTime_Wzglednie
		vmax = previous_vmax;
		vmin = previous_vmin;
		previous_vmin = DBL_MAX;
		previous_vmax = -DBL_MAX;
		break;



	}

	for (int currentVertex = 0; currentVertex < oscs.size(); ++currentVertex)
	{
		if (oscs[currentVertex]->m_type != SOLID_WALL)
		{
			//stimulator->processActivationTimes(msh, currentVertex);
			switch (_dataDisplayMode)
			{
			case DM_POTENTIAL:
				val1 = oscs[currentVertex]->m_v_scaledPotential;
				colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				break;
			case DM_CSD:
				val1 = oscs[currentVertex]->getLastCurrentSource();
				val1 > previous_vmax ? previous_vmax = val1 : 0;
				val1 < previous_vmin ? previous_vmin = val1 : 0;
				colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				break;
			case DM_CUR1:
				val1 = oscs[currentVertex]->m_v_current[0];
				colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				break;
			case DM_CUR2:
				val1 = oscs[currentVertex]->m_v_current[1];
				colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				break;
			case DM_ELECTROGRAM:
				val1 = oscs[currentVertex]->m_v_electrogram;
				colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				break;
			case DM_ACT_TIME: //ActivationTime _ Wzglednie
				if (stimulator->activationTimeMode() == ATC_RELATIVE)
				{
					//stimulator->processActivationTimes(msh, currentVertex);
					val1 = oscs[currentVertex]->m_lastActivationTime;
					val1 > previous_vmax ? previous_vmax = val1 : 0;
					val1 < previous_vmin ? previous_vmin = val1 : 0;
					colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				}
				else
				{
					//stimulator->processActivationTimes(msh, currentVertex);
					val1 = fmod(stimulator->phaseZero(), stimulator->activationTimeCycle());
					val1 = fmod((oscs[currentVertex]->m_lastActivationTime - val1), stimulator->activationTimeCycle());
					vmin = 0;
					vmax = stimulator->activationTimeCycle();
					colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				}
				break;
			case DM_PPIR_TCL:
				val1 = oscs[currentVertex]->m_lastPPIR_TCL;
				val1 > previous_vmax ? previous_vmax = val1 : 0;
				val1 < previous_vmin ? previous_vmin = val1 : 0;
				colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
				break;
			}


		}
		else
		{
			msh->m_vertexMatrix[currentVertex].r = 0.1;
			msh->m_vertexMatrix[currentVertex].g = 0.1;
			msh->m_vertexMatrix[currentVertex].b = 0.1;
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glColorPointer(3, GL_FLOAT, sizeof(SVertex), &view->linkToMesh->m_vertexMatrix[0].r);

	glVertexPointer(3, GL_FLOAT, sizeof(SVertex), view->linkToMesh->m_vertexMatrix);

	glDrawElements(GL_TRIANGLES, view->linkToMesh->m_vertexList.size() * 3, GL_UNSIGNED_INT, view->linkToMesh->m_indicesMatrix);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);


	glPushMatrix();


	if (_displayConduction)
	{
		Vector3 pos;
		for (int currentVertex = 0; currentVertex < oscs.size(); ++currentVertex)
		{
			if (oscs[currentVertex]->m_type != SOLID_WALL)
			{
				pos = Vector3(oscs[currentVertex]->getPositionX(), oscs[currentVertex]->getPositionY(), oscs[currentVertex]->getPositionZ());
				drawVector(0.3, pos,msh->conductionVector(currentVertex), 1, 1, 1);
			}
		}
	}

		int oscID;
		// [3] Paint probes
		for (short n = 0; n < view->linkToMachine->stimulator->probeElectrodesCount(); ++n)
		{
			oscID = view->linkToMachine->stimulator->probeElectrodeID(n);

			drawSphere(0.2, 10, 10, oscs[oscID]->getPositionX(),
				oscs[oscID]->getPositionY(),
				oscs[oscID]->getPositionZ(), 1.0f / (n + 1), 1.0f / (n + 1), 1.0f);

			
			QString str = "Electrode ";
			str += QString::number(n);
			view->renderText(oscs[oscID]->getPositionX(),
				oscs[oscID]->getPositionY(),
				oscs[oscID]->getPositionZ() + 2, str);
		}
		if (view->paintRay)
		{
			drawSphere(0.2, 10, 10, view->testProbe.x, view->testProbe.y, view->testProbe.z, 1.0f, 1.0f, 1.0f);
		}
	glPopMatrix();
}


void SimViewState::drawVector(double size, Vector3 position, Vector3 direction, float rr, float gg, float bb) {

	Vector3 v;
	double orientation = 1;
	Matrix3 m = Matrix3(cos(2 * M_PI / 3), -sin(2 * M_PI / 3), 0, sin(2 * M_PI / 3), cos(2 * M_PI / 3), 0, 0, 0, 1);

	if(direction.z < 0)
		size = -size;

	v = direction.normalize();
	GLfloat col_g[] = { rr, gg, bb, 0.6f };
	double x, y, z;
	glColor3fv(col_g);
	//glBegin(GL_LINES);

	//	x = position.x + size* v.x / 2;
	//	y = position.y + size * v.y / 2;
	//	z = position.z + size * v.z + 0.2;
	//	glVertex3f(x, y, z);

	//	x = position.x - size *v.x / 2;
	//	y = position.y - size *v.y / 2;
	//	glVertex3f(x, y, z);

	//glEnd();
	glBegin(GL_TRIANGLES);

		x = position.x + size * v.x / 2;
		y = position.y + size * v.y / 2;
		z = position.z + abs(size) * v.z + 0.2;
		glVertex3f(x, y, z);

		v = m * v;

		x = position.x + size *v.x / 2;
		y = position.y + size *v.y / 2;
		z = position.z + abs(size) * v.z + 0.2;
		glVertex3f(x, y, z);

		v = m * v;

		x = position.x + size *v.x / 2;
		y = position.y + size *v.y / 2;
		z = position.z + abs(size) * v.z + 0.2;
		glVertex3f(x, y, z);

	glEnd();

}