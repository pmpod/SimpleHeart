#pragma once
#include <QObject>
#include <QMessageBox>
#include <vector>
#include "Model\Oscillator.h"
#include "Model\v3model.h"
#include "heartDefines.h"
#include "NumericStrategy\TimeTree.h"
#include "Support\Vectors.h"
#include <matio.h>
#include<qgl.h>



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
	static CardiacMesh* constructCartesianGrid(int x, int y, double dx, double dy, CELL_TYPE type);

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

	///\brief calculate the rotation and display center of the mesh (geometrical center)
	void calculateCenter();

	/**
	* \brief Create a list of all faces
	* @param doublesided - should both side of face be created (doubles the size of the list).
	*/
	void setVertexTriangleList(bool doublesided);
	
	void setWallCells();
	void clearWallCells();

	void setDiffusionCoefficients();



	///\brief Returns the distance between two most distant nodes in mm(estimate of the size of the mesh)
	const double sizeInMm();
	const double minNodalSpacing();




	double calculateElectrogram(Oscillator* osc);

	std::vector<Oscillator*> m_mesh;
	std::vector<Oscillator*> m_underStimulation;
	bool stimulationBegun;

	double m_simulationTime; //?



	double maximumCV;

	double maxElectrogram;
	double minElectrogram;

	double maxPotential;
	double minPotential;

	double maxDiffusion;
	double minDiffusion;
	double maxERP;
	double minERP;



	bool structureUpdated;





	//TODO - poprawi� getSize na double !!!

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
