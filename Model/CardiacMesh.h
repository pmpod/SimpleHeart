#pragma once
#include <QObject>
#include <QMessageBox>
#include <string>
#include <vector>
#include <set>
#include "Model\Oscillator.h"
#include "Model\v3model.h"
#include "Model\FitzHughNagumo.h"
#include "Model\vanDerGru.h"
#include "heartDefines.h"
#include "NumericStrategy\TimeTree.h"
#include "Support\Vectors.h"
#include <matio.h>
#include<qgl.h>
#include "Support\Vectors.h"
#include "Support\Matrices.h"



/*! \struct VertexTriangle
* \brief VertexTriangle is a three-int-structure containing the IDs of each oscillator (vertex on the mesh) forming a valid face on a mesh
* VertexTriangle is a three-int-structure containing the IDs of each oscillator (vertex on the mesh) forming a valid face on a mesh.
* vertex triangles are stored in CardiacMesh container m_vertexList and build on the initialization of the mesh.
*
*/
struct VertexTriangle
{
	int id_1;
	int id_2;
	int id_3;
};

/*! \struct SVertex
* \brief SVertex is a structure for OpenGL 2.0 Vertex Buffer Object. It stores information about location and color of each vertex. 
* Location values are created on the initialization of the mesh, while color is changed due to the View settings / at each simulation frame.
*
*/
struct SVertex
{
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat nx, ny, nz;
};


/*! \class CardiacMesh
* \brief Class contatining the full information about the model and helper functions to build it.
*
*/
class CardiacMesh
{
public:
	CardiacMesh();
	~CardiacMesh(void);
	void destroyGrid();

	/**
	* \brief A static function constructing rectangular, Cartesian CardiacMesh object consisting of one type of the cell, with von Neumann boundary conditions.
	* @param x - number of horizontal cells 
	* @param y - number of vertical cells 
	* @param dx - horizontal spacing between cells
	* @param dy - vertical spacing between cells
	* @param type - cell type to be set for every cell (accepts CELL_TYPE enum value).
	*/
	static CardiacMesh* constructCartesianGrid(int x, int y, double dx, double dy, CardiacMesh *grid);
	static CardiacMesh* constructCartesianGridPlain(int x, int y, double dx, double dy, CELL_TYPE type);
	static CardiacMesh* constructCartesianGridRightAtrium(int x, int y, double dx, double dy, double percentageOfWidth);

	/**
	* \brief A static function constructing cylindrical CardiacMesh object consisting of one type of the cell
	* @param x - number of horizontal cells
	* @param y - number of vertical cells
	* @param dx - horizontal spacing between cells
	* @param dy - vertical spacing between cells
	* @param type - cell type to be set for every cell (accepts CELL_TYPE enum value).
	*/
	static CardiacMesh* constructCylindricalGrid(int x, int y, double dx, double dy);
	static CardiacMesh* importGrid(const char *inname);
	static CardiacMesh* constructStl(std::string inname, CELL_TYPE type);

	///\brief calculate the rotation and display center of the mesh (geometrical center)
	void calculateCenter();

	/**
	* \brief Create a list of all faces
	* @param doublesided - should both side of face be created (doubles the size of the list).
	*/
	void setVertexTriangleList(bool doublesided);
	void addNode(int idnode, double x, double y, double z, CELL_TYPE type);
	void setWallCells();
	void clearWallCells();

	void setDiffusionCoefficients();



	///\brief Returns the distance between two most distant nodes in mm(estimate of the size of the mesh)
	const double sizeInMm();
	const double minNodalSpacing();



	void processActivationTime(Oscillator* osc);
	double calculateElectrogram(Oscillator* osc);
	double calculateElectrogramRecursively(Oscillator* srcosc, Oscillator* osc, int currentLevel, std::set<int>& setOfadded);
	Vector3 conductionVector(int oscID);
	void calculateConductionVector(Oscillator* src, Oscillator* osc, const double radius);

	float FindLargestEntry(const Matrix3 &m);
	Vector3 FindEigenVectorAssociatedWithLargestEigenValue(const Matrix3 &m);

	void FindLLSQPlane(Vector3 *points, int count, Vector3 *destCenter, Vector3 *destNormal);

	std::vector<Oscillator*> m_markedOscillators;
	std::map < int, bool > m_marked;
	std::vector<Oscillator*> m_mesh;
	std::vector<Oscillator*> m_underStimulation;
	bool stimulationBegun;

	double m_simulationTime; //?



	double _derivativeTreshold;
	double _minRefractoryTreshold;
	double maximumCV;

	double maxElectrogram;
	double minElectrogram;

	double maxPotential;
	double minPotential;

	double maxDiffusion;
	double minDiffusion;
	double maxERP;
	double minERP;
	double maxExcitability;
	double minExcitability;



	bool structureUpdated;





	//TODO - poprawiæ getSize na double !!!

	//3d

	double m_radius;
	std::vector<VertexTriangle*> m_vertexList;
	SVertex* m_vertexMatrix;
	GLuint* m_indicesMatrix;
	Vector3 centerGeom;


private:
	double _sizeInMm;
	double _minNodalSpacing;
	double _maxNodalSpacing;

};
