//
//  ObjModel.cpp
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2015.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.  If you are in a position of
//    authority, you may forbid others ffrom using them in areas
//    that fall under your authority.  For example, a professor
//    could forbid students from using them for a class project,
//    or an employer could forbid employees using for a company
//    project.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#include <cassert>
#include <cctype>
#include <cstdlib>	// for atoi
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include "GetGlut.h"
#include "ObjSettings.h"
#include "ObjStringParsing.h"
#include "DisplayList.h"
#include "Material.h"
#include "MtlLibrary.h"
#include "MtlLibraryManager.h"
#include "ObjModel.h"

using namespace std;
namespace
{
	//
	//  Are these guaranteed to be initialized before they
	//    are needed?  That would include either of:
	//    -> at compile time
	//    -> at run time, but before any function in this
	//       file is executed.
	//
	const char* DEFAULT_FILE_NAME = "unnamed.obj";
	const char* DEFAULT_FILE_PATH = "";

	const bool DEBUGGING = false;
	const bool DEBUGGING_VERTEX_BUFFER = false;
}



const unsigned int ObjModel :: NO_TEXTURE_COORDINATES = 0xFFFFFFFF;
const unsigned int ObjModel :: NO_NORMAL = 0xFFFFFFFF;



void ObjModel :: loadDisplayTextures ()
{
	MtlLibraryManager::loadDisplayTextures();
}

void ObjModel :: loadAllTextures ()
{
	MtlLibraryManager::loadAllTextures();
}



ObjModel :: ObjModel ()
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_point_sets(),
		  mv_polylines(),
		  mv_meshes()
{
	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	assert(isEmpty());
	assert(invariant());
}

ObjModel :: ObjModel (const string& filename)
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_point_sets(),
		  mv_polylines(),
		  mv_meshes()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	load(filename);
	// load calls validate()

	assert(invariant());
}

ObjModel :: ObjModel (const string& filename, const string& logfile)
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_point_sets(),
		  mv_polylines(),
		  mv_meshes()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	load(filename, logfile);
	// load calls validate()

	assert(invariant());
}

ObjModel :: ObjModel (const string& filename, ostream& r_logstream)
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_point_sets(),
		  mv_polylines(),
		  mv_meshes()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	load(filename, r_logstream);
	// load calls validate()

	assert(invariant());
}

ObjModel :: ObjModel (const ObjModel& original)
		: mv_material_libraries(original.mv_material_libraries),
		  mv_vertexes(original.mv_vertexes),
		  mv_texture_coordinates(original.mv_texture_coordinates),
		  mv_normals(original.mv_normals),
		  mv_point_sets(original.mv_point_sets),
		  mv_polylines(original.mv_polylines),
		  mv_meshes(original.mv_meshes)
{
	m_file_name         = original.m_file_name;
	m_file_path         = original.m_file_path;
	m_file_load_success = original.m_file_load_success;
	m_valid             = original.m_valid;

	assert(invariant());
}

ObjModel& ObjModel :: operator= (const ObjModel& original)
{
	if(&original != this)
	{
		mv_material_libraries = original.mv_material_libraries;
		mv_vertexes = original.mv_vertexes;
		mv_texture_coordinates = original.mv_texture_coordinates;
		mv_normals = original.mv_normals;
		mv_point_sets = original.mv_point_sets;
		mv_polylines = original.mv_polylines;
		mv_meshes = original.mv_meshes;

		m_file_name         = original.m_file_name;
		m_file_path         = original.m_file_path;
		m_file_load_success = original.m_file_load_success;
		m_valid             = original.m_valid;
	}

	assert(invariant());
	return *this;
}

ObjModel :: ~ObjModel ()
{
}



const string& ObjModel :: getFileName () const
{
	return m_file_name;
}

const string& ObjModel :: getFilePath () const
{
	return m_file_path;
}

string ObjModel :: getFileNameWithPath () const
{
	return m_file_path + m_file_name;
}

bool ObjModel :: isLoadedSuccessfully () const
{
	return m_file_load_success;
}

bool ObjModel :: isEmpty () const
{
	if(!mv_material_libraries.empty()) return false;
	if(!mv_vertexes.empty()) return false;
	if(!mv_texture_coordinates.empty()) return false;
	if(!mv_normals.empty()) return false;
	if(!mv_point_sets.empty()) return false;
	if(!mv_polylines.empty()) return false;
	if(!mv_meshes.empty()) return false;
	return true;
}

unsigned int ObjModel :: getVertexCount () const
{
	return mv_vertexes.size();
}

double ObjModel :: getVertexX (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex].x;
}

double ObjModel :: getVertexY (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex].y;
}

double ObjModel :: getVertexZ (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex].z;
}

const Vector3& ObjModel :: getVertexPosition (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex];
}


unsigned int ObjModel :: getTextureCoordinateCount () const
{
	return mv_texture_coordinates.size();
}

double ObjModel :: getTextureCoordinateU (unsigned int texture_coordinate) const
{
	assert(texture_coordinate < getTextureCoordinateCount());

	return mv_texture_coordinates[texture_coordinate].x;
}

double ObjModel :: getTextureCoordinateV (unsigned int texture_coordinate) const
{
	assert(texture_coordinate < getTextureCoordinateCount());

	return mv_texture_coordinates[texture_coordinate].y;
}

const Vector2& ObjModel :: getTextureCoordinate (unsigned int texture_coordinate) const
{
	assert(texture_coordinate < getTextureCoordinateCount());

	return mv_texture_coordinates[texture_coordinate];
}

unsigned int ObjModel :: getNormalCount () const
{
	return mv_normals.size();
}

double ObjModel :: getNormalX (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal].x;
}

double ObjModel :: getNormalY (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal].y;
}

double ObjModel :: getNormalZ (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal].z;
}

const Vector3& ObjModel :: getNormalVector (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal];
}

unsigned int ObjModel :: getPointSetCount () const
{
	return mv_point_sets.size();
}

unsigned int ObjModel :: getPointSetVertexCount (unsigned int point_set) const
{
	assert(point_set < getPointSetCount());

	return mv_point_sets[point_set].mv_vertexes.size();
}

unsigned int ObjModel :: getPointSetVertexIndex (unsigned int point_set, unsigned int vertex) const
{
	assert(point_set < getPointSetCount());
	assert(vertex < getPointSetVertexCount(point_set));

	return mv_point_sets[point_set].mv_vertexes[vertex];
}

unsigned int ObjModel :: getPolylineCount () const
{
	return mv_polylines.size();
}

unsigned int ObjModel :: getPolylineVertexCount (unsigned int polyline) const
{
	assert(polyline < getPolylineCount());

	return mv_polylines[polyline].mv_vertexes.size();
}

unsigned int ObjModel :: getPolylineVertexIndex (unsigned int polyline, unsigned int vertex) const
{
	assert(polyline < getPolylineCount());
	assert(vertex < getPolylineVertexCount(polyline));

	return mv_polylines[polyline].mv_vertexes[vertex].m_vertex;
}

unsigned int ObjModel :: getPolylineVertexTextureCoordinates (unsigned int polyline, unsigned int vertex) const
{
	assert(polyline < getPolylineCount());
	assert(vertex < getPolylineVertexCount(polyline));

	return mv_polylines[polyline].mv_vertexes[vertex].m_texture_coordinate;
}

unsigned int ObjModel :: getMeshCount () const
{
	return mv_meshes.size();
}

bool ObjModel :: isMeshMaterial (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	if(mv_meshes[mesh].m_material_name != "")
		return true;
	else
		return false;
}

const string& ObjModel :: getMeshMaterialName (unsigned int mesh) const
{
	assert(mesh < getMeshCount());
	assert(isMeshMaterial(mesh));

	return mv_meshes[mesh].m_material_name;
}

bool ObjModel :: isMeshAllTriangles (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	return mv_meshes[mesh].m_all_triangles;
}

unsigned int ObjModel :: getFaceCount (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	return mv_meshes[mesh].mv_faces.size();
}

unsigned int ObjModel :: getFaceVertexCount (unsigned int mesh, unsigned int face) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes.size();
}

unsigned int ObjModel :: getFaceVertexIndex (unsigned int mesh, unsigned int face, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_vertex;
}

unsigned int ObjModel :: getFaceVertexTextureCoordinates (unsigned int mesh, unsigned int face, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_texture_coordinate;
}

unsigned int ObjModel :: getFaceVertexNormal (unsigned int mesh, unsigned int face, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_normal;
}

unsigned int ObjModel :: getFaceCountTotal () const
{
	unsigned int total = 0;

	for(unsigned int i = 0; i < mv_meshes.size(); i++)
		total += mv_meshes[i].mv_faces.size();
	return total;
}

bool ObjModel :: isAllTriangles () const
{
	for(unsigned int m = 0; m < mv_meshes.size(); m++)
		if(!mv_meshes[m].m_all_triangles)
			return false;
	return true;
}

bool ObjModel :: isSingleMaterial () const
{
	if(mv_meshes.size() <= 1)
		return true;

	assert(0 <= mv_meshes.size());
	const string& material_name = mv_meshes[0].m_material_name;

	for(unsigned int i = 1; i < mv_meshes.size(); i++)
		if(mv_meshes[i].m_material_name != material_name)
			return false;
	return true;
}

vector<string> ObjModel :: getAllMaterialNames () const
{
	unsigned int mesh_count = mv_meshes.size();
	vector<string> v_names(mesh_count, "");

	for(unsigned int m = 0; m < mesh_count; m++)
		if(isMeshMaterial(m))
			v_names[m] = getMeshMaterialName(m);

	return v_names;
}

bool ObjModel :: isValid () const
{
	return m_valid;
}



void ObjModel :: print () const
{
	print(cout);
}

void ObjModel :: print (const string& logfile) const
{
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	print(logstream);
	logstream.close();
}

void ObjModel :: print (ostream& r_logstream) const
{
	if(m_valid)
		r_logstream << m_file_path << m_file_name << " (valid)" << endl;
	else
		r_logstream << m_file_path << m_file_name << " (invalid)" << endl;

	if(mv_material_libraries.size() > 0)
	{
		r_logstream << "    " << mv_material_libraries.size() << " material libraries" << endl;
		for(unsigned int m = 0; m < mv_material_libraries.size(); m++)
			r_logstream << "        " << mv_material_libraries[m].m_name << endl;
	}

	r_logstream << "    Vertices: " << getVertexCount() << endl;
	for(unsigned int v = 0; v < getVertexCount(); v++)
		r_logstream << "    " << setw(6) << v << ": " << mv_vertexes[v] << endl;

	r_logstream << "    Texture Coordinate Pairs: " << getTextureCoordinateCount() << endl;
	for(unsigned int t = 0; t < getTextureCoordinateCount(); t++)
		r_logstream << "    " << setw(6) << t << ": (" << mv_texture_coordinates[t].x << ", " << mv_texture_coordinates[t].y << ")" << endl;

	r_logstream << "    Normals: " << getNormalCount() << endl;
	for(unsigned int n = 0; n < getNormalCount(); n++)
		r_logstream << "    " << setw(6) << n << ": " << mv_normals[n] << endl;

	r_logstream << "    Point Sets: " << getPointSetCount() << endl;
	for(unsigned int p = 0; p < getPointSetCount(); p++)
	{
		r_logstream << "    " << setw(6) << p << ": " << getPointSetVertexCount(p) << endl;
		for(unsigned int i = 0; i < getPointSetVertexCount(p); i++)
			cout << "        " << setw(6) << i << ": " << getPointSetVertexIndex(p, i) << endl;
	}

	r_logstream << "    Polylines: " << getPolylineCount() << endl;
	for(unsigned int l = 0; l < getPolylineCount(); l++)
	{
		r_logstream << "    " << setw(6) << l << ": " << getPolylineVertexCount(l) << endl;
		for(unsigned int i = 0; i < getPolylineVertexCount(l); i++)
		{
			r_logstream << "        " << setw(6) << i << ": ";
			r_logstream << getPolylineVertexIndex(l, i);
			if(getPolylineVertexTextureCoordinates(l, i) != NO_TEXTURE_COORDINATES)
				cout << "\t" << getPolylineVertexTextureCoordinates(l, i);
			r_logstream << endl;
		}
	}

	r_logstream << "    Meshes: " << getMeshCount() << endl;
	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		r_logstream << "        Mesh " << m << ": " << getFaceCount(m) << " faces";
		if(isMeshAllTriangles(m))
			cout << " (triangles)";
		r_logstream << endl;

		if(isMeshMaterial(m))
			r_logstream << "            Material: \"" << getMeshMaterialName(m) << "\"" << endl;
		else
			r_logstream << "            Material: None" << endl;

		for(unsigned int f = 0; f < getFaceCount(m); f++)
		{
			r_logstream << "        " << setw(6) << f << ": " << getFaceVertexCount(m, f) << endl;
			for(unsigned int i = 0; i < getFaceVertexCount(m, f); i++)
			{
				r_logstream << "            " << setw(6) << i << ": ";
				r_logstream << getFaceVertexIndex(m, f, i);
				if(getFaceVertexTextureCoordinates(m, f, i) != NO_TEXTURE_COORDINATES)
				{
					r_logstream << "\t" << getFaceVertexTextureCoordinates(m, f, i);
					if(getFaceVertexNormal(m, f, i) != NO_NORMAL)
						r_logstream << "\t" << getFaceVertexNormal(m, f, i);
				}
				else if(getFaceVertexNormal(m, f, i) != NO_NORMAL)
					r_logstream << "\t\t" << getFaceVertexNormal(m, f, i);
				r_logstream << endl;
			}
		}
	}

	r_logstream << endl;
}

void ObjModel :: printMtlLibraries () const
{
	printMtlLibraries(cout);
}

void ObjModel :: printMtlLibraries (const string& logfile) const
{
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	printMtlLibraries(logstream);
	logstream.close();
}

void ObjModel :: printMtlLibraries (ostream& r_logstream) const
{
	r_logstream << "Libraries referenced by \"" << m_file_path << m_file_name << "\":" << endl;
	for(unsigned int m = 0; m < mv_material_libraries.size(); m++)
		r_logstream << "\t\"" << mv_material_libraries[m].m_name << "\"" << endl;
}

void ObjModel :: printBadMaterials () const
{
	printBadMaterials(cout);
}

void ObjModel :: printBadMaterials (const string& logfile) const
{
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	printBadMaterials(logstream);
	logstream.close();
}

void ObjModel :: printBadMaterials (ostream& r_logstream) const
{
	for(unsigned int m = 0; m < getMeshCount(); m++)
		if(mv_meshes[m].m_material_name != "" && mv_meshes[m].mp_material == NULL)
		{
			r_logstream << "Invalid material referenced in \"" << m_file_path << m_file_name << "\": "
			     << "\"" << mv_meshes[m].m_material_name << "\"" << endl;
		}
}

void ObjModel :: draw () const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMeshMaterial(m, mv_meshes[m].mp_material);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawShaders () const
{
#ifdef OBJ_LIBRARY_SHADER_DISPLAY

	const unsigned int VERTEXES_PER_TRIANGLE = 3;

	//
	//  Do not change these without changing the code to
	//    fill the arrays below!  Otherwise, the program
	//    will crash.
	//

	const unsigned int ELEMENTS_PER_INDEX = 1;
	const unsigned int ELEMENTS_PER_VERTEX = 3;
	const unsigned int ELEMENTS_PER_TEXTURE_COORDINATE = 2;
	const unsigned int ELEMENTS_PER_NORMAL = 3;

	const Vector2 DEFAULT_TEXTURE_COORDINATE = Vector2::ZERO;
	const Vector3 DEFAULT_NORMAL = Vector3::ZERO;

	assert(isValid());
	assert(!Material::isMaterialActive());

	// count the required triangles
	unsigned int total_triangle_count = 0;
	unsigned int* d_triangle_count_per_face = new unsigned int[getMeshCount()];
	unsigned int* d_first_triangle_for_face = new unsigned int[getMeshCount()];
	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		d_triangle_count_per_face[s1] = 0;
		d_first_triangle_for_face[s1] = total_triangle_count;
		for(unsigned int f = 0; f < getFaceCount(s1); f++)
		{
			unsigned int vertex_count = getFaceVertexCount(s1, f);
			assert(vertex_count >= 3);
			unsigned int triangle_count = vertex_count - 2;
			total_triangle_count += triangle_count;
			d_triangle_count_per_face[s1] += triangle_count;
		}
	}

	unsigned int total_vertex_count = total_triangle_count * VERTEXES_PER_TRIANGLE;
	unsigned int index_array_size              = total_vertex_count * ELEMENTS_PER_INDEX;
	unsigned int vertex_array_size             = total_vertex_count * ELEMENTS_PER_VERTEX;
	unsigned int texture_coordinate_array_size = total_vertex_count * ELEMENTS_PER_TEXTURE_COORDINATE;
	unsigned int normal_array_size             = total_vertex_count * ELEMENTS_PER_NORMAL;

	unsigned int* d_indexes     = new unsigned int[total_vertex_count];
	float* d_vertexes           = new float[vertex_array_size];
	float* d_texture_coordintes = new float[texture_coordinate_array_size];
	float* d_normals            = new float[normal_array_size];

	unsigned int next_index = 0;
	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		for(unsigned int f = 0; f < getFaceCount(s2); f++)
		{
			for(unsigned int v = 0; v < getFaceVertexCount(s2, f); v++)
			{
				unsigned int index_index              = next_index * ELEMENTS_PER_INDEX;
				unsigned int index_vertex             = next_index * ELEMENTS_PER_VERTEX;
				unsigned int index_texture_coordinate = next_index * ELEMENTS_PER_TEXTURE_COORDINATE;
				unsigned int index_normal             = next_index * ELEMENTS_PER_NORMAL;

				assert(index_index              < index_array_size);
				assert(index_vertex             < vertex_array_size);
				assert(index_texture_coordinate < texture_coordinate_array_size);
				assert(index_normal             < normal_array_size);

				assert(ELEMENTS_PER_INDEX == 1);
				assert(ELEMENTS_PER_VERTEX == 3);
				assert(ELEMENTS_PER_TEXTURE_COORDINATE == 2);
				assert(ELEMENTS_PER_NORMAL == 3);

				d_indexes[index_index] = next_index;

				unsigned int face_vertex_index = getFaceVertexIndex(s2, f, v);
				d_vertexes[index_vertex]     = getVertexPosition(face_vertex_index).x;
				d_vertexes[index_vertex + 1] = getVertexPosition(face_vertex_index).y;
				d_vertexes[index_vertex + 2] = getVertexPosition(face_vertex_index).z;

				unsigned int face_vertex_texture_coordinate = getFaceVertexTextureCoordinates(s2, f, v);
				if(face_vertex_texture_coordinate == NO_TEXTURE_COORDINATES)
				{
					d_texture_coordintes[index_texture_coordinate]     = DEFAULT_TEXTURE_COORDINATE.x;
					d_texture_coordintes[index_texture_coordinate + 1] = DEFAULT_TEXTURE_COORDINATE.y;
				}
				else
				{
					d_texture_coordintes[index_texture_coordinate]     = getTextureCoordinateU(face_vertex_texture_coordinate);
					d_texture_coordintes[index_texture_coordinate + 1] = getTextureCoordinateV(face_vertex_texture_coordinate);
				}

				unsigned int face_vertex_normal = getFaceVertexNormal(s2, f, v);
				if(face_vertex_normal == NO_NORMAL)
				{
					d_normals[index_normal]     = DEFAULT_NORMAL.x;
					d_normals[index_normal + 1] = DEFAULT_NORMAL.y;
					d_normals[index_normal + 2] = DEFAULT_NORMAL.z;
				}
				else
				{
					d_normals[index_normal]     = getNormalVector(face_vertex_normal).x;
					d_normals[index_normal + 1] = getNormalVector(face_vertex_normal).y;
					d_normals[index_normal + 2] = getNormalVector(face_vertex_normal).z;
				}

				next_index++;
			}
		}
	}

	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		if(mv_meshes[s3].mp_material != NULL)
			mv_meshes[s3].mp_material->activate();

		glBegin(GL_TRIANGLES);
			if(DEBUGGING_VERTEX_BUFFER)
				cout << s3 << ":\t" << d_first_triangle_for_face[s3] << " -> " << d_triangle_count_per_face[s3] << endl;
			for(unsigned int t = 0; t < d_triangle_count_per_face[s3]; t++)
			{
				if(DEBUGGING_VERTEX_BUFFER)
					cout << "  " << t << ":\t" << endl;
				for(unsigned int v = 0; v < VERTEXES_PER_TRIANGLE; v++)
				{
					unsigned int position = (d_first_triangle_for_face[s3] + t) * VERTEXES_PER_TRIANGLE + v;
					if(DEBUGGING_VERTEX_BUFFER)
					{
						cout << "    " << position << endl;
						cout << "      Vertex: (" << d_vertexes[position * ELEMENTS_PER_VERTEX]
						     << ", " << d_vertexes[position * ELEMENTS_PER_VERTEX + 1]
						     << ", " << d_vertexes[position * ELEMENTS_PER_VERTEX + 2] << ")" << endl;
						cout << "      TextureCoordinates: (" << d_texture_coordintes[position * ELEMENTS_PER_TEXTURE_COORDINATE]
						     << ", " << d_texture_coordintes[position * ELEMENTS_PER_TEXTURE_COORDINATE + 1] << ")" << endl;
						cout << "      Normal: (" << d_normals[position * ELEMENTS_PER_NORMAL]
						     << ", " << d_normals[position * ELEMENTS_PER_NORMAL + 1]
						     << ", " << d_normals[position * ELEMENTS_PER_NORMAL + 2] << ")" << endl;
					}
					glNormal3fv(d_normals + position * ELEMENTS_PER_NORMAL);
					glTexCoord2fv(d_texture_coordintes + position * ELEMENTS_PER_TEXTURE_COORDINATE);
					glVertex3fv(d_vertexes + position * ELEMENTS_PER_VERTEX);
				}
			}
		glEnd();

		if(mv_meshes[s3].mp_material != NULL)
			Material::deactivate();
	}

	delete[] d_indexes;
	delete[] d_vertexes;
	delete[] d_texture_coordintes;
	delete[] d_normals;

	delete[] d_triangle_count_per_face;
	delete[] d_first_triangle_for_face;

	assert(!Material::isMaterialActive());

#else

	// call non-shader version of draw
	draw();

#endif	// #ifdef OBJ_LIBRARY_SHADER_DISPLAY
}

void ObjModel :: drawMaterialNone () const
{
	assert(isValid());

	drawPointSets();
	drawPolylines();
	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMesh(m);
}

void ObjModel :: drawMaterial (const Material& material) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	material.activate();

	drawPointSets();
	drawPolylines();
	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMesh(m);

	Material::deactivate();

	if(material.isSeperateSpecular())
	{
		material.activateSeperateSpecular();
		for(unsigned int m = 0; m < getMeshCount(); m++)
			drawMesh(m);
		Material::deactivate();
	}

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterial (const char* a_name) const
{
	assert(isValid());
	assert(a_name != NULL);

	drawMaterial(string(a_name));
}

void ObjModel :: drawMaterial (const string& name) const
{
	assert(isValid());

	const Material* p_material = getMaterialByName(name);
	if(p_material != NULL)
		drawMaterial(*p_material);
	else
		drawMaterialNone();
}

void ObjModel :: drawMaterial (const char* a_material_name, const char* a_library_name) const
{
	assert(isValid());
	assert(a_material_name != NULL);
	assert(a_library_name != NULL);

	drawMaterial(string(a_material_name), string(a_library_name));
}

void ObjModel :: drawMaterial (const string& material_name, const string& library_name) const
{
	assert(isValid());

	const MtlLibrary& mtl_library = MtlLibraryManager::get(library_name);

	unsigned int index = mtl_library.getMaterialIndex(material_name);
	if(index != MtlLibrary::NO_SUCH_MATERIAL)
		drawMaterial(*(mtl_library.getMaterial(index)));
	else
		drawMaterialNone();
}

void ObjModel :: drawMaterials (const vector<Material*>& materials) const
{
	assert(isValid());
	assert(materials.size() >= getMeshCount());

	if(Material::isMaterialActive())
		Material::deactivate();
	assert(!Material::isMaterialActive());

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMeshMaterial(m, materials[m]);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterials (const vector<const char*>& va_names) const
{
	assert(isValid());
	assert(va_names.size() >= getMeshCount());

	if(Material::isMaterialActive())
		Material::deactivate();
	assert(!Material::isMaterialActive());

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		if(va_names[m] == NULL)
			drawMesh(m);
		else
			drawMeshMaterial(m, getMaterialByName(va_names[m]));
	}

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterials (const vector<string>& v_names) const
{
	assert(isValid());
	assert(v_names.size() >= getMeshCount());

	if(Material::isMaterialActive())
		Material::deactivate();
	assert(!Material::isMaterialActive());

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMeshMaterial(m, getMaterialByName(v_names[m]));

	assert(!Material::isMaterialActive());
}


void ObjModel :: drawMtlLibrary (const MtlLibrary& library) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		const Material* p_material = NULL;

		if(mv_meshes[m].m_material_name != "")
			p_material = library.getMaterial(mv_meshes[m].m_material_name);
		if(p_material == NULL)
			p_material = mv_meshes[m].mp_material;

		drawMeshMaterial(m, p_material);
	}

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMtlLibrary (const char* a_name) const
{
	assert(isValid());
	assert(a_name != NULL);

	drawMtlLibrary(MtlLibraryManager::get(a_name));
}

void ObjModel :: drawMtlLibrary (const string& name) const
{
	assert(isValid());

	drawMtlLibrary(MtlLibraryManager::get(name));
}

void ObjModel :: drawVertices (unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawVertices(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertices (int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawVertices(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertices (double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	glBegin(GL_POINTS);
		for(unsigned int v = 0; v < getVertexCount(); v++)
		{
			glVertex3d(mv_vertexes[v].x,
				   mv_vertexes[v].y,
				   mv_vertexes[v].z);
		}
	glEnd();

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawWireframe (unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawWireframe(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawWireframe (int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawWireframe(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawWireframe (double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		for(unsigned int f = 0; f < getFaceCount(m); f++)
		{
			glBegin(GL_LINE_LOOP);
				for(unsigned int v = 0; v < getFaceVertexCount(m, f); v++)
				{
					unsigned int vertex = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;

					glVertex3d(mv_vertexes[vertex].x,
						   mv_vertexes[vertex].y,
						   mv_vertexes[vertex].z);
				}
			glEnd();
		}

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawSolid (unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawSolid(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawSolid (int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawSolid(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawSolid (double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	drawPointSets();
	drawPolylines();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMesh(m);

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertexNormals (double length, unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);

	drawVertexNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertexNormals (double length, int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawVertexNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertexNormals (double length, double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	glBegin(GL_LINES);
		for(unsigned int m = 0; m < getMeshCount(); m++)
			for(unsigned int f = 0; f < getFaceCount(m); f++)
				for(unsigned int v = 0; v < getFaceVertexCount(m, f); v++)
				{
					unsigned int vertex = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;
					unsigned int normal = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_normal;

					if(normal != NO_NORMAL)
					{
						assert(vertex < getVertexCount());
						assert(normal < getNormalCount());

						glVertex3d(mv_vertexes[vertex].x,
							   mv_vertexes[vertex].y,
							   mv_vertexes[vertex].z);

						glVertex3d(mv_vertexes[vertex].x + mv_normals[normal].x * length,
							   mv_vertexes[vertex].y + mv_normals[normal].y * length,
							   mv_vertexes[vertex].z + mv_normals[normal].z * length);
					}
				}
	glEnd();

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawFaceNormals (double length, unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);

	drawFaceNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawFaceNormals (double length, int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawFaceNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawFaceNormals (double length, double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	glBegin(GL_LINES);
		for(unsigned int m = 0; m < getMeshCount(); m++)
			for(unsigned int f = 0; f < getFaceCount(m); f++)
			{
				Vector3 center = Vector3::ZERO;
				Vector3 face_normal = Vector3::ZERO;

				for(unsigned int v = 0; v < getFaceVertexCount(m, f); v++)
				{
					unsigned int vertex = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;
					unsigned int normal = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_normal;

					assert(vertex < getVertexCount());
					center += mv_vertexes[vertex];

					if(normal != NO_NORMAL)
					{
						assert(normal < getNormalCount());
						face_normal += mv_normals[normal];
					}
				}

				if(!face_normal.isZero())
				{
					center /= getFaceVertexCount(m, f);
					face_normal.setNorm(length);

					glVertex3d(center.x,
						   center.y,
						   center.z);

					glVertex3d(center.x + face_normal.x,
						   center.y + face_normal.y,
						   center.z + face_normal.z);
				}
			}
	glEnd();

	material.deactivate();

	assert(!Material::isMaterialActive());
}

DisplayList ObjModel :: getDisplayList () const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	//
	//  We are using a loop to load textures instead of
	//
	//      loadDisplayTextures();
	//
	//    so that we only load textures used by this model.
	//    Otherwise, any texture required by any material in
	//    an MTL file referenced by an ObjModel would be
	//    loaded, even if that material - and thus that
	//    texture - was never used.  This way, we avoid
	//    cluttering up video memory with textures we never
	//    use.
	//

	for(unsigned int i = 0; i < mv_meshes.size(); i++)
		if(mv_meshes[i].mp_material != NULL)
			mv_meshes[i].mp_material->loadDisplayTextures();

	DisplayList list;
	list.begin();
		draw();
	list.end();

	assert(!Material::isMaterialActive());
	return list;
}

DisplayList ObjModel :: getDisplayListMaterial (const Material& material) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	// ensure material and all textures are loaded
	material.activate();
	Material::deactivate();
	assert(!Material::isMaterialActive());

	DisplayList list;
	list.begin();
		drawMaterial(material);
	list.end();

	assert(!Material::isMaterialActive());
	return list;
}

DisplayList ObjModel :: getDisplayListMaterial (const char* a_name) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(a_name != NULL);

	const Material* p_material = getMaterialByName(a_name);
	if(p_material != NULL)
		return getDisplayListMaterial(*p_material);
	else
		return getDisplayListMaterialNone();
}

DisplayList ObjModel :: getDisplayListMaterial (const std::string& name) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	const Material* p_material = getMaterialByName(name);
	if(p_material != NULL)
		return getDisplayListMaterial(*p_material);
	else
		return getDisplayListMaterialNone();
}

DisplayList ObjModel :: getDisplayListMaterialNone () const
{
	assert(isValid());

	DisplayList list;

	list.begin();
		drawMaterialNone();
	list.end();

	return list;
}

void ObjModel ::  save (const string& filename) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	save(filename, cerr);

	assert(invariant());
}

void ObjModel ::  save (const string& filename, const string& logfile) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream;

	logstream.open(logfile.c_str());
	save(filename, logstream);
	logstream.close();

	assert(invariant());
}

void ObjModel ::  save (const string& filename, ostream& r_logstream) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	ofstream output_file;

	if(DEBUGGING)
		cout << "About to save " << filename << endl;

	output_file.open(filename.c_str());
	if(!output_file.is_open())
	{
		r_logstream << "ERROR: cannot write to file \"" << filename << "\" - ABORTING" << endl;
		return;
	}

	//
	//  Format of file:
	//
	//  Header
	//    -> how many of each element
	//  Material libraries
	//  Vertices
	//  Texture coordinate pairs
	//  Normals
	//  Point sets
	//  Polylines
	//  Faces
	//    -> Divided into meshes with associated materials
	//  Footer
	//
	//  Major sections are seperated by 3 blank lines.  Minor
	//    sections are seperated by 1 blank line.
	//

	output_file << "#" << endl;
	output_file << "# " << getFileNameWithPath() << endl;
	output_file << "#" << endl;
	if(mv_vertexes.size() > 0)
		output_file << "# " << getVertexCount() << " vertices" << endl;
	if(mv_texture_coordinates.size() > 0)
		output_file << "# " << getTextureCoordinateCount() << " texture coordinate pairs" << endl;
	if(mv_normals.size() > 0)
		output_file << "# " << getNormalCount() << " vertex normals" << endl;
	if(mv_point_sets.size() > 0)
		output_file << "# " << getPointSetCount() << " point sets" << endl;
	if(mv_polylines.size() > 0)
		output_file << "# " << getPolylineCount() << " polylines" << endl;
	if(mv_meshes.size() > 0)
		output_file << "# " << getFaceCountTotal() << " in " << getMeshCount() << " meshes" << endl;
	output_file << "#" << endl;
	output_file << endl;
	output_file << endl;
	output_file << endl;

	if(DEBUGGING)
		cout << "Wrote file header" << endl;

	if(mv_material_libraries.size() > 0)
	{
		output_file << "# " << mv_material_libraries.size() << " material libraries" << endl;
		output_file << "mtllib";
		for(unsigned int m = 0; m < mv_material_libraries.size(); m++)
			output_file << " " << mv_material_libraries[m].m_name;
		output_file << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote material libraries" << endl;
	}

	if(mv_vertexes.size() > 0)
	{
		output_file << "# " << getVertexCount() << " vertices" << endl;
		for(unsigned int v = 0; v < mv_vertexes.size(); v++)
			output_file << "v " << mv_vertexes[v].x << " " << mv_vertexes[v].y << " " << mv_vertexes[v].z << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote vertices" << endl;
	}

	if(mv_texture_coordinates.size() > 0)
	{
		output_file << "# " << getTextureCoordinateCount() << " texture coordinate pairs" << endl;
		for(unsigned int t = 0; t < mv_texture_coordinates.size(); t++)
			output_file << "vt " << mv_texture_coordinates[t].x << " " << mv_texture_coordinates[t].y << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote texture coordinates" << endl;
	}

	if(mv_normals.size() > 0)
	{
		output_file << "# " << getNormalCount() << " vertex normals" << endl;
		for(unsigned int n = 0; n < mv_normals.size(); n++)
			output_file << "vn " << mv_normals[n].x << " " << mv_normals[n].y << " " << mv_normals[n].z << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote normals" << endl;
	}

	if(mv_point_sets.size() > 0)
	{
		output_file << "# " << getPointSetCount() << " point sets" << endl;
		for(unsigned int p = 0; p < mv_point_sets.size(); p++)
		{
			output_file << "p";
			for(unsigned int i = 0; i < mv_point_sets[p].mv_vertexes.size(); i++)
				output_file << " " << (mv_point_sets[p].mv_vertexes[i] + 1);
			output_file << endl;
		}
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote point sets" << endl;
	}

	if(mv_polylines.size() > 0)
	{
		output_file << "# " << getPolylineCount() << " polylines" << endl;
		for(unsigned int l = 0; l < mv_polylines.size(); l++)
		{
			output_file << "l";
			for(unsigned int i = 0; i < mv_polylines[l].mv_vertexes.size(); i++)
			{
				output_file << " " << (mv_polylines[l].mv_vertexes[i].m_vertex + 1);

				if(mv_polylines[l].mv_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
					output_file << "/" << (mv_polylines[l].mv_vertexes[i].m_texture_coordinate + 1);
			}
			output_file << endl;
		}
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote polylines" << endl;
	}

	if(mv_meshes.size() > 0)
	{
		output_file << "# " << getMeshCount() << " meshes" << endl;
		output_file << endl;
		for(unsigned int s = 0; s < mv_meshes.size(); s++)
		{
			output_file << "# " << getFaceCount(s) << " faces" << endl;
			if(isMeshMaterial(s))
				output_file << "usemtl " << mv_meshes[s].m_material_name << endl;

			for(unsigned int f = 0; f < mv_meshes[s].mv_faces.size(); f++)
			{
				output_file << "f";
				for(unsigned int i = 0; i < mv_meshes[s].mv_faces[f].mv_vertexes.size(); i++)
				{
					output_file << " " << (mv_meshes[s].mv_faces[f].mv_vertexes[i].m_vertex + 1);

					if(mv_meshes[s].mv_faces[f].mv_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
					{
						output_file << "/" << (mv_meshes[s].mv_faces[f].mv_vertexes[i].m_texture_coordinate + 1);

						if(mv_meshes[s].mv_faces[f].mv_vertexes[i].m_normal != NO_NORMAL)
							output_file << "/" << (mv_meshes[s].mv_faces[f].mv_vertexes[i].m_normal + 1);
					}
					else if(mv_meshes[s].mv_faces[f].mv_vertexes[i].m_normal != NO_NORMAL)
						output_file << "//" << (mv_meshes[s].mv_faces[f].mv_vertexes[i].m_normal + 1);

				}
				output_file << endl;
			}
			output_file << endl;
		}
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING)
			cout << "Wrote faces" << endl;
	}

	output_file << "# End of " << getFileNameWithPath() << endl;
	output_file << endl;

	if(DEBUGGING)
		cout << "Wrote footer" << endl;

	output_file.close();
}



void ObjModel :: makeEmpty ()
{
	mv_vertexes.clear();
	mv_texture_coordinates.clear();
	mv_normals.clear();
	mv_point_sets.clear();
	mv_polylines.clear();
	mv_meshes.clear();

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	assert(isEmpty());
	assert(invariant());
}

void ObjModel :: load (const string& filename)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	load(filename, cerr);

	assert(invariant());
}

void ObjModel :: load (const string& filename, const string& logfile)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	load(filename, logstream);
	logstream.close();

	assert(invariant());
}

void ObjModel :: load (const string& filename, ostream& r_logstream)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	ifstream input_file;
	unsigned int line_count;

	if(DEBUGGING)
		cout << "About to remove any existing contents" << endl;

	makeEmpty();

	if(DEBUGGING)
		cout << "About to load " << filename << endl;

	setFileNameWithPath(filename);

	input_file.open(filename.c_str(), ios::in);
	if(!input_file.is_open())
	{
		r_logstream << "Error: File \"" << filename << "\" does not exist" << endl;
		input_file.close();

		m_file_load_success = false;

		assert(invariant());
		return;
	}

	//
	//  Format is available at
	//
	//  http://www.martinreddy.net/gfx/3d/OBJ.spec
	//

	line_count = 0;
	while(true)	// drops out at EOF below
	{
		string line;
		size_t length;
		bool valid;

		getline(input_file, line);
		if(input_file.eof())
			break;	// drop out after last line

		length = line.length();
		line_count++;

		if(length < 1 || line[0] == '#' || line[0] == '\r')
			continue;	// skip blank lines and comments

		valid = true;
		if(length < 1)
			valid = false;
		else
		{
			if(line.length() >= 7 && line.substr(0, 6) == "mtllib" && isspace(line[6]))
				valid = readMaterialLibrary(line.substr(7), r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "usemtl" && isspace(line[6]))
				valid = readMaterial(line.substr(7), r_logstream);
			else if(line.length() >= 2 && line[0] == 'v' && isspace(line[1]))
				valid = readVertex(line.substr(2), r_logstream);
			else if(line.length() >= 3 && line[0] == 'v' && line[1] == 't' && isspace(line[2]))
				valid = readTextureCoordinates(line.substr(3), r_logstream);
			else if(line.length() >= 3 && line[0] == 'v' && line[1] == 'n' && isspace(line[2]))
				valid = readNormal(line.substr(3), r_logstream);
			else if(line.length() >= 2 && line[0] == 'p' && isspace(line[1]))
				valid = readPointSet(line.substr(2), r_logstream);
			else if(line.length() >= 2 && line[0] == 'l' && isspace(line[1]))
				valid = readPolyline(line.substr(2), r_logstream);
			else if(line.length() >= 2 && line[0] == 'f' && isspace(line[1]))
				valid = readFace(line.substr(2), r_logstream);
			else if(line[0] == 'g' && (line.length() == 1 || isspace(line[1])))
			{
				if(DEBUGGING)
					r_logstream << "In file \"" << filename << "\": ignoring groupings \"" << line.substr(1) << "\"" << endl;
			}
			else if(line[0] == 's' && (line.length() == 1 || isspace(line[1])))
			{
				if(DEBUGGING)
					r_logstream << "In file \"" << filename << "\": ignoring smoothing group \"" << line.substr(1) << "\"" << endl;
			}
			else if(line[0] == 'o' && (line.length() == 1 || isspace(line[1])))
			{
				if(DEBUGGING)
					r_logstream << "In file \"" << filename << "\": ignoring object name \"" << line.substr(1) << "\"" << endl;
			}
			else
				valid = false;
		}

		if(!valid)
			r_logstream << "Line " << setw(6) << line_count << " of file \"" << filename << "\" is invalid: \"" << line << "\"" << endl;
	}

	input_file.close();

	validate();

	assert(invariant());
}



void ObjModel :: setFileName (const string& file_name)
{
	assert(file_name != "");

	m_file_name = file_name;

	assert(invariant());
}

void ObjModel :: setFilePath (const string& file_path)
{
	m_file_path = file_path;

	assert(invariant());
}

void ObjModel :: setFileNameWithPath (const string& filename)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	size_t last_slash = filename.find_last_of("/\\");
	if(last_slash != string::npos)
	{
		last_slash++;
		m_file_name = filename.substr(last_slash);
		m_file_path = filename.substr(0, last_slash);
	}
	else
	{
		m_file_name = filename;
		m_file_path = "";
	}

	assert(invariant());
}

void ObjModel :: setVertexCount (unsigned int count)
{
	if(count < getVertexCount())
	{
		m_valid = false;
		mv_vertexes.resize(count);
	}
	else if(count > getVertexCount())
		mv_vertexes.resize(count, Vector3::ZERO);

	assert(invariant());
}

void ObjModel :: setVertexX (unsigned int vertex, double x)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].x = x;

	assert(invariant());
}

void ObjModel :: setVertexY (unsigned int vertex, double y)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].y = y;

	assert(invariant());
}

void ObjModel :: setVertexZ (unsigned int vertex, double z)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].z = z;

	assert(invariant());
}

void ObjModel :: setVertexPosition (unsigned int vertex, double x, double y, double z)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].set(x, y, z);

	assert(invariant());
}

void ObjModel :: setVertexPosition (unsigned int vertex, const Vector3& position)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex] = position;

	assert(invariant());
}

void ObjModel :: setTextureCoordinateCount (unsigned int count)
{
	if(count < getTextureCoordinateCount())
	{
		m_valid = false;
		mv_texture_coordinates.resize(count);
	}
	else if(count > getTextureCoordinateCount())
		mv_texture_coordinates.resize(count, Vector2::ZERO);

	assert(invariant());
}

void ObjModel :: setTextureCoordinateU (unsigned int texture_coordinate, double u)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate].x = u;

	assert(invariant());
}
void ObjModel :: setTextureCoordinateV (unsigned int texture_coordinate, double v)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate].y = v;

	assert(invariant());
}

void ObjModel :: setTextureCoordinate (unsigned int texture_coordinate, double u, double v)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate].x = u;
	mv_texture_coordinates[texture_coordinate].y = v;

	assert(invariant());
}

void ObjModel :: setTextureCoordinate (unsigned int texture_coordinate, const Vector2& coordinates)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate] = coordinates;

	assert(invariant());
}

void ObjModel :: setNormalCount (unsigned int count)
{
	if(count < getNormalCount())
	{
		m_valid = false;
		mv_normals.resize(count);
	}
	else if(count > getNormalCount())
		mv_normals.resize(count, Vector3::UNIT_Z_PLUS);

	assert(invariant());
}

void ObjModel :: setNormalX (unsigned int normal, double x)
{
	assert(normal < getNormalCount());
	assert(x != 0.0 || getNormalY(normal) != 0.0 || getNormalZ(normal) != 0.0);

	mv_normals[normal].x = x;
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalY (unsigned int normal, double y)
{
	assert(normal < getNormalCount());
	assert(getNormalX(normal) != 0.0 || y != 0.0 || getNormalZ(normal) != 0.0);

	mv_normals[normal].y = y;
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalZ (unsigned int normal, double z)
{
	assert(normal < getNormalCount());
	assert(getNormalX(normal) != 0.0 || getNormalY(normal) != 0.0 || z != 0.0);

	mv_normals[normal].z = z;
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalVector (unsigned int normal, double x, double y, double z)
{
	assert(normal < getNormalCount());
	assert(x != 0.0 || y != 0.0 || z != 0.0);

	mv_normals[normal].set(x, y, z);
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalVector (unsigned int normal, const Vector3& vector)
{
	assert(normal < getNormalCount());
	assert(!vector.isZero());

	mv_normals[normal] = vector.getNormalized();

	assert(invariant());
}

void ObjModel :: setPointSetVertexIndex (unsigned int point_set, unsigned int vertex, unsigned int index)
{
	assert(point_set < getPointSetCount());
	assert(vertex < getPointSetVertexCount(point_set));

	mv_point_sets[point_set].mv_vertexes[vertex] = index;
	if(index >= getVertexCount())
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setPolylineVertexIndex (unsigned int polyline, unsigned int vertex, unsigned int index)
{
	assert(polyline < getPolylineCount());
	assert(vertex < getPolylineVertexCount(polyline));

	mv_polylines[polyline].mv_vertexes[vertex].m_vertex = index;
	if(index >= getVertexCount())
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setPolylineVertexTextureCoordinates (unsigned int polyline, unsigned int vertex, unsigned int index)
{
	assert(polyline < getPolylineCount());
	assert(vertex < getPolylineVertexCount(polyline));

	mv_polylines[polyline].mv_vertexes[vertex].m_texture_coordinate = index;
	if(index >= getTextureCoordinateCount() && index != NO_TEXTURE_COORDINATES)
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setMeshMaterial (unsigned int mesh, const string& material)
{
	assert(mesh < getMeshCount());
	assert(material != "");

	if(DEBUGGING)
		cout << "    Setting mesh " << mesh << " to use material " << material << endl;

	mv_meshes[mesh].m_material_name = material;

	mv_meshes[mesh].mp_material = NULL;
	for(unsigned int i = 0; i < mv_material_libraries.size(); i++)
	{
		if(mv_material_libraries[i].mp_mtl_library == NULL)
			continue;

		unsigned int index = mv_material_libraries[i].mp_mtl_library->getMaterialIndex(material);

		if(index == MtlLibrary::NO_SUCH_MATERIAL)
			continue;

		mv_meshes[mesh].mp_material = mv_material_libraries[i].mp_mtl_library->getMaterial(index);
	}

	assert(invariant());
}

void ObjModel :: setMeshMaterialNone (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	mv_meshes[mesh].m_material_name = "";
	mv_meshes[mesh].mp_material = NULL;

	assert(invariant());
}

void ObjModel :: setFaceVertexIndex (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_vertex = index;
	if(index >= getVertexCount())
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setFaceVertexTextureCoordinates (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_texture_coordinate = index;
	if(index >= getTextureCoordinateCount() && index != NO_TEXTURE_COORDINATES)
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setFaceVertexNormal (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_normal = index;
	if(index >= getVertexCount() && index != NO_NORMAL)
		m_valid = false;

	assert(invariant());
}



void ObjModel :: addMaterialLibrary (const string& library)
{
	assert(library != "");
	assert(library.find_last_of("/\\") == string::npos ||
	       library.find_last_of("/\\") + 1 < library.size());

	addMaterialLibrary(library, cerr);

	assert(invariant());
}

void ObjModel :: addMaterialLibrary (const string& library, const string& logfile)
{
	assert(library != "");
	assert(library.find_last_of("/\\") == string::npos ||
	       library.find_last_of("/\\") + 1 < library.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	addMaterialLibrary(library, logstream);
	logstream.close();

	assert(invariant());
}

void ObjModel :: addMaterialLibrary (const string& library, ostream& r_logstream)
{
	assert(library != "");
	assert(library.find_last_of("/\\") == string::npos ||
	       library.find_last_of("/\\") + 1 < library.size());

#ifdef OBJ_LIBRARY_PATH_PROPAGATION
	mv_material_libraries.push_back(MaterialLibrary(m_file_path, library, r_logstream));
#else
	mv_material_libraries.push_back(MaterialLibrary("", library, r_logstream));
#endif

	if(DEBUGGING)
	{
		unsigned int index = mv_material_libraries.size() - 1;
		cout << "Added Material Libary \"" << mv_material_libraries[index].m_name << "\"" << endl;
		if(mv_material_libraries[index].mp_mtl_library == NULL)
			cout << "But couldn't load it" << endl;
	}

	assert(invariant());
}

unsigned int ObjModel :: addVertex (double x, double y, double z)
{
	return addVertex(Vector3(x, y, z));
}

unsigned int ObjModel :: addVertex (const Vector3& position)
{
	unsigned int id = mv_vertexes.size();
	mv_vertexes.push_back(position);

	if(DEBUGGING)
		cout << "Added Vertex #" << (id + 1) << " " << position << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addTextureCoordinate (double u, double v)
{
	return addTextureCoordinate(Vector2(u, v));
}

unsigned int ObjModel :: addTextureCoordinate (const Vector2& texture_coordinates)
{
	unsigned int id = mv_texture_coordinates.size();
	mv_texture_coordinates.push_back(texture_coordinates);

	if(DEBUGGING)
		cout << "Added Texture Coordinate #" << (id + 1) << " " << texture_coordinates << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addNormal (double x, double y, double z)
{
	assert(x != 0.0 || y != 0.0 || z != 0.0);

	return addNormal(Vector3(x, y, z));
}

unsigned int ObjModel :: addNormal (const Vector3& normal)
{
	assert(!normal.isZero());

	unsigned int id = mv_normals.size();
	mv_normals.push_back(normal.getNormalized());

	if(DEBUGGING)
		cout << "Added Normal #" << (id + 1) << " " << normal << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPointSet ()
{
	unsigned int id = mv_point_sets.size();
	mv_point_sets.push_back(PointSet());
	m_valid = false;

	if(DEBUGGING)
		cout << "Added Point Set #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPointSetVertex (unsigned int point_set, unsigned int vertex)
{
	assert(point_set < getPointSetCount());

	unsigned int id = mv_point_sets[point_set].mv_vertexes.size();
	mv_point_sets[point_set].mv_vertexes.push_back(vertex);

	if(vertex >= getVertexCount())
		m_valid = false;

	if(DEBUGGING)
		cout << "    Added vertex #" << (id + 1) << " (" << (vertex + 1) << ")" << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPolyline ()
{
	unsigned int id = mv_polylines.size();
	mv_polylines.push_back(Polyline());
	m_valid = false;

	if(DEBUGGING)
		cout << "Added Polyline #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPolylineVertex (unsigned int polyline, unsigned int vertex, unsigned int texture_coordinates)
{
	assert(polyline < getPolylineCount());

	unsigned int id = mv_polylines[polyline].mv_vertexes.size();
	mv_polylines[polyline].mv_vertexes.push_back(PolylineVertex(vertex, texture_coordinates));

	if(vertex >= getVertexCount())
		m_valid = false;
	if(texture_coordinates != NO_TEXTURE_COORDINATES && texture_coordinates >= getTextureCoordinateCount())
		m_valid = false;

	if(DEBUGGING)
	{
		cout << "    Added vertex #" << (id + 1) << " (" << (vertex + 1) << "/";

		if(texture_coordinates == NO_TEXTURE_COORDINATES)
			cout << " )" << endl;
		else
			cout << (texture_coordinates + 1) << ")" << endl;
	}

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addMesh ()
{
	unsigned int id = mv_meshes.size();
	mv_meshes.push_back(Mesh());

	if(DEBUGGING)
		cout << "Added mesh #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addFace (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	unsigned int id = mv_meshes[mesh].mv_faces.size();
	mv_meshes[mesh].mv_faces.push_back(Face());
	m_valid = false;

	if(DEBUGGING)
		cout << "    Added face #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addFaceVertex (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int texture_coordinates, unsigned int normal)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	unsigned int id = mv_meshes[mesh].mv_faces[face].mv_vertexes.size();
	mv_meshes[mesh].mv_faces[face].mv_vertexes.push_back(FaceVertex(vertex, texture_coordinates, normal));

	if(vertex >= getVertexCount())
		m_valid = false;
	if(texture_coordinates != NO_TEXTURE_COORDINATES && texture_coordinates >= getTextureCoordinateCount())
		m_valid = false;
	if(normal != NO_NORMAL && normal >= getNormalCount())
		m_valid = false;
	if(id > 3)
		mv_meshes[mesh].m_all_triangles = false;

	if(DEBUGGING)
	{
		cout << "        Added vertex #" << (id + 1) << " (" << (vertex + 1) << "/";

		if(texture_coordinates == NO_TEXTURE_COORDINATES)
			cout << "/";
		else
			cout << (texture_coordinates + 1) << "/";

		if(normal == NO_NORMAL)
			cout << " )" << endl;
		else
			cout << (normal + 1) << ")" << endl;
	}

	assert(invariant());
	return id;
}

void ObjModel :: removePointSet (unsigned int point_set)
{
	assert(point_set < getPointSetCount());

	unsigned int point_set_count = mv_point_sets.size();
	for(unsigned int i = point_set + 1; i < point_set_count; i++)
	{
		assert(i >= 1);
		mv_point_sets[i - 1] = mv_point_sets[i];
	}

	mv_point_sets.pop_back();

	if(DEBUGGING)
		cout << "    Removed point set #" << (point_set + 1) << endl;

	assert(invariant());
}

void ObjModel :: removePointSetVertex (unsigned int point_set,
                                       unsigned int vertex)
{
	assert(point_set < getPointSetCount());
	assert(vertex < getPolylineVertexCount(point_set));

	vector<unsigned int>& rv_vertexes = mv_point_sets[point_set].mv_vertexes;
	unsigned int vertex_count = rv_vertexes.size();
	for(unsigned int i = vertex + 1; i < vertex_count; i++)
	{
		assert(i >= 1);
		rv_vertexes[i - 1] = rv_vertexes[i];
	}

	if(DEBUGGING)
	{
		cout << "    Removed point set #" << (point_set + 1)
		     <<              ", vertex #" << (vertex + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removePointSetVertexAll (unsigned int point_set)
{
	assert(point_set < getPointSetCount());

	mv_point_sets[point_set].mv_vertexes.clear();

	if(DEBUGGING)
		cout << "    Removed point set #" << (point_set + 1) << ", all vertexes" << endl;

	assert(invariant());
}

void ObjModel :: removePolyline (unsigned int polyline)
{
	assert(polyline < getPolylineCount());

	unsigned int polyline_count = mv_polylines.size();
	for(unsigned int i = polyline + 1; i < polyline_count; i++)
	{
		assert(i >= 1);
		mv_polylines[i - 1] = mv_polylines[i];
	}

	mv_polylines.pop_back();

	if(DEBUGGING)
		cout << "    Removed polyline #" << (polyline + 1) << endl;

	assert(invariant());
}

void ObjModel :: removePolylineVertex (unsigned int polyline,
                                       unsigned int vertex)
{
	assert(polyline < getPolylineCount());
	assert(vertex < getPolylineVertexCount(polyline));

	vector<PolylineVertex>& rv_vertexes = mv_polylines[polyline].mv_vertexes;
	unsigned int vertex_count = rv_vertexes.size();
	for(unsigned int i = vertex + 1; i < vertex_count; i++)
	{
		assert(i >= 1);
		rv_vertexes[i - 1] = rv_vertexes[i];
	}

	if(DEBUGGING)
	{
		cout << "    Removed polyline #" << (polyline + 1)
		     <<             ", vertex #" << (vertex + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removePolylineVertexAll (unsigned int polyline)
{
	assert(polyline < getPolylineCount());

	mv_polylines[polyline].mv_vertexes.clear();

	if(DEBUGGING)
		cout << "    Removed polyline #" << (polyline + 1) << ", all vertexes" << endl;

	assert(invariant());
}

void ObjModel :: removeMesh (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	unsigned int mesh_count = mv_meshes.size();
	for(unsigned int i = mesh + 1; i < mesh_count; i++)
	{
		assert(i >= 1);
		mv_meshes[i - 1] = mv_meshes[i];
	}

	mv_meshes.pop_back();

	if(DEBUGGING)
		cout << "    Removed mesh #" << (mesh + 1) << endl;

	assert(invariant());
}

void ObjModel :: removeFace (unsigned int mesh,
                             unsigned int face)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	vector<Face>& rv_faces = mv_meshes[mesh].mv_faces;
	unsigned int face_count = rv_faces.size();
	for(unsigned int i = face + 1; i < face_count; i++)
	{
		assert(i >= 1);
		rv_faces[i - 1] = rv_faces[i];
	}

	rv_faces.pop_back();

	if(DEBUGGING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<           ", face #" << (face + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removeFaceAll (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	mv_meshes[mesh].mv_faces.clear();
	mv_meshes[mesh].m_all_triangles = true;

	if(DEBUGGING)
		cout << "    Removed mesh #" << (mesh + 1) << ", all faces" << endl;

	assert(invariant());
}

void ObjModel :: removeFaceVertex (unsigned int mesh,
                                   unsigned int face,
                                   unsigned int vertex)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	vector<FaceVertex>& rv_vertexes = mv_meshes[mesh].mv_faces[face].mv_vertexes;
	unsigned int vertex_count = rv_vertexes.size();
	for(unsigned int i = vertex + 1; i < vertex_count; i++)
	{
		assert(i >= 1);
		rv_vertexes[i - 1] = rv_vertexes[i];
	}

	rv_vertexes.pop_back();
	m_valid = false;

	if(DEBUGGING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<           ", face #" << (face + 1)
		     <<         ", vertex #" << (vertex + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removeFaceVertexAll (unsigned int mesh,
                                      unsigned int face)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	mv_meshes[mesh].mv_faces[face].mv_vertexes.clear();
	m_valid = false;

	if(DEBUGGING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<           ", face #" << (face + 1) << ", all vertexes" << endl;
	}

	assert(invariant());
}



void ObjModel :: validate ()
{
	bool old_valid = m_valid;

	m_valid = true;

	if(DEBUGGING)
	{
		cout << "Vertices: " << getVertexCount() << endl;
		cout << "Texture Coordinates: " << getTextureCoordinateCount() << endl;
		cout << "Vertex Normals: " << getNormalCount() << endl;

		if(mv_material_libraries.size() > 0)
		{
			cout << "Material Libraries:" << endl;
			for(unsigned int i = 0; i < mv_material_libraries.size(); i++)
				cout << "    " << mv_material_libraries[i].m_name << endl;
		}
	}

	for(unsigned int p = 0; p < mv_point_sets.size(); p++)
	{
		if(mv_point_sets[p].mv_vertexes.size() < 1)
		{
			m_valid = false;

			assert(old_valid == false);
			assert(invariant());
			return;
		}

		for(unsigned int v = 0; v < mv_point_sets[p].mv_vertexes.size(); v++)
		{
			unsigned int vertex = mv_point_sets[p].mv_vertexes[v];

			if(vertex >= getVertexCount())
			{
				if(DEBUGGING)
					cout << "Invalid vertex: Point Set " << p << ", vertex " << v << " has vertex " << (vertex + 1) << endl;
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}
		}
	}

	for(unsigned int l = 0; l < mv_polylines.size(); l++)
	{
		if(mv_polylines[l].mv_vertexes.size() < 2)
		{
			m_valid = false;

			assert(old_valid == false);
			assert(invariant());
			return;
		}

		for(unsigned int v = 0; v < mv_polylines[l].mv_vertexes.size(); v++)
		{
			unsigned int vertex = mv_polylines[l].mv_vertexes[v].m_vertex;
			unsigned int texture_coordinates = mv_polylines[l].mv_vertexes[v].m_texture_coordinate;

			if(vertex >= getVertexCount())
			{
				if(DEBUGGING)
					cout << "Invalid vertex: Polyline " << l << ", vertex " << v << " has vertex " << (vertex + 1) << endl;
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}

			if(texture_coordinates >= getTextureCoordinateCount() && texture_coordinates != NO_TEXTURE_COORDINATES)
			{
				if(DEBUGGING)
					cout << "Invalid texture coordinates: Polyline " << l << ", vertex " << v << " has texture coordinates " << (texture_coordinates + 1) << endl;
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}
		}
	}

	for(unsigned int s = 0; s < mv_meshes.size(); s++)
	{
/*
		// bad materials cause too many crashes
		if(mv_meshes[s].m_material_name != "" && mv_meshes[s].mp_material == NULL)
		{
			if(DEBUGGING)
				cout << "Invalid material: Mesh " << s << " has non-existant material \"" << mv_meshes[s].m_material_name << "\"" << endl;
			m_valid = false;

			assert(old_valid == false);
			assert(invariant());
			return;
		}
*/
		mv_meshes[s].m_all_triangles = true;
		for(unsigned int f = 0; f < mv_meshes[s].mv_faces.size(); f++)
		{
			unsigned int face_vertex_count = mv_meshes[s].mv_faces[f].mv_vertexes.size();
			if(face_vertex_count < 3)
			{
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}
			else if(face_vertex_count > 3)
				mv_meshes[s].m_all_triangles = false;

			for(unsigned int v = 0; v < face_vertex_count; v++)
			{
				unsigned int vertex              = mv_meshes[s].mv_faces[f].mv_vertexes[v].m_vertex;
				unsigned int texture_coordinates = mv_meshes[s].mv_faces[f].mv_vertexes[v].m_texture_coordinate;
				unsigned int normal              = mv_meshes[s].mv_faces[f].mv_vertexes[v].m_normal;

				if(vertex >= getVertexCount())
				{
					if(DEBUGGING)
						cout << "Invalid vertex: Mesh " << s << ", face " << f << ", vertex " << v << " has vertex " << (vertex + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}

				if(texture_coordinates >= getTextureCoordinateCount() && texture_coordinates != NO_TEXTURE_COORDINATES)
				{
					if(DEBUGGING)
						cout << "Invalid texture coordinates: Mesh " << s << ", face " << f << ", vertex " << v << " has texture coordinates " << (texture_coordinates + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}

				if(normal >= getNormalCount() && normal != NO_NORMAL)
				{
					if(DEBUGGING)
						cout << "Invalid normal: Mesh " << s << ", face " << f << ", vertex " << v << " has normal " << (normal + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}
			}
		}
	}

	assert(invariant());
}



void ObjModel :: drawPointSets () const
{
	if(getPointSetCount() > 0)
	{
		glBegin(GL_POINTS);
			for(unsigned int p = 0; p < getPointSetCount(); p++)
				for(unsigned int v = 0; v < getPointSetVertexCount(p); v++)
				{
					unsigned int vertex = mv_point_sets[p].mv_vertexes[v];

					glVertex3d(mv_vertexes[vertex].x,
						   mv_vertexes[vertex].y,
						   mv_vertexes[vertex].z);
				}
		glEnd();
	}
}

void ObjModel :: drawPolylines () const
{
	for(unsigned int l = 0; l < getPolylineCount(); l++)
	{
		glBegin(GL_LINE_STRIP);
			for(unsigned int v = 0; v < getPolylineVertexCount(l); v++)
			{
				unsigned int vertex              = mv_polylines[l].mv_vertexes[v].m_vertex;
				unsigned int texture_coordinates = mv_polylines[l].mv_vertexes[v].m_texture_coordinate;

				if(texture_coordinates != NO_TEXTURE_COORDINATES)
				{
					glTexCoord2d(mv_texture_coordinates[texture_coordinates].x,
						     mv_texture_coordinates[texture_coordinates].y);
				}

				glVertex3d(mv_vertexes[vertex].x,
					   mv_vertexes[vertex].y,
					   mv_vertexes[vertex].z);
			}
		glEnd();
	}
}

void ObjModel :: drawMesh (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());

	bool is_all_triangles = mv_meshes[mesh].m_all_triangles;

	// if everything is triangles, draw everything as one triangle group
	if(is_all_triangles)
		glBegin(GL_TRIANGLES);

	for(unsigned int f = 0; f < getFaceCount(mesh); f++)
	{
		// otherwise, draw as lots of trinagle fans
		if(!is_all_triangles)
			glBegin(GL_TRIANGLE_FAN);

		for(unsigned int v = 0; v < getFaceVertexCount(mesh, f); v++)
		{
			unsigned int vertex              = mv_meshes[mesh].mv_faces[f].mv_vertexes[v].m_vertex;
			unsigned int texture_coordinates = mv_meshes[mesh].mv_faces[f].mv_vertexes[v].m_texture_coordinate;
			unsigned int normal              = mv_meshes[mesh].mv_faces[f].mv_vertexes[v].m_normal;

			if(normal != NO_NORMAL)
			{
				glNormal3d(mv_normals[normal].x,
					   mv_normals[normal].y,
					   mv_normals[normal].z);
			}

			if(texture_coordinates != NO_TEXTURE_COORDINATES)
			{
				// flip texture coordinates to match Maya <|>
				glTexCoord2d(      mv_texture_coordinates[texture_coordinates].x,
				             1.0 - mv_texture_coordinates[texture_coordinates].y);
			}

			glVertex3d(mv_vertexes[vertex].x,
				   mv_vertexes[vertex].y,
				   mv_vertexes[vertex].z);
		}

		// end of current trinagle fan
		if(!is_all_triangles)
			glEnd();
	}

	// end of trinagles
	if(is_all_triangles)
		glEnd();
}

void ObjModel :: drawMeshMaterial (unsigned int mesh, const Material* p_material) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(!Material::isMaterialActive());

	if(p_material == NULL)
		drawMesh(mesh);
	else
	{
		p_material->activate();
		drawMesh(mesh);
		Material::deactivate();

		if(p_material->isSeperateSpecular())
		{
			p_material->activateSeperateSpecular();
			drawMesh(mesh);
			Material::deactivate();
		}
	}

	assert(!Material::isMaterialActive());
}

const Material* ObjModel :: getMaterialByName (const string& name) const
{
	assert(isValid());

	for(unsigned int i = 0; i < mv_material_libraries.size(); i++)
	{
		if(mv_material_libraries[i].mp_mtl_library == NULL)
			continue;

		unsigned int index = mv_material_libraries[i].mp_mtl_library->getMaterialIndex(name);
		if(index != MtlLibrary::NO_SUCH_MATERIAL)
			return mv_material_libraries[i].mp_mtl_library->getMaterial(index);
	}

	return NULL;
}

bool ObjModel :: readMaterialLibrary (const string& str, ostream& r_logstream)
{
	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		string library;

		size_t token_length = getTokenLength(str, token_index);

		if(token_length == 0)
			return false;

		library = str.substr(token_index, token_length);

		//
		//  Should we add on the current path? <|>
		//
		//  Thoughts:
		//    - Harder when saving Obj models
		//      - would have to be adjusted to compensate
		//      - strip leading path portion
		//    - Simply prepending it would break currently-working files with duplicated paths
		//    - Try both positions?
		//      - have to only try later ones if first did not exist
		//      - loaded-correctly field in MtlLibrary class
		//

		assert(library != "");
		assert(library.find_last_of("/\\") == string::npos ||
		       library.find_last_of("/\\") + 1 < library.size());
		addMaterialLibrary(library, r_logstream);
	}

	// if we get here, all tokens were valid
	return true;
}

bool ObjModel :: readMaterial (const string& str, ostream& r_logstream)
{
	string material;
	unsigned int mesh_index;

	size_t length;
	size_t start_index;
	size_t end_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	length = str.length();
	for(end_index = start_index; end_index < length && !isspace(str[end_index]); end_index++)
		; // do nothing

	if(end_index < length)
		material = str.substr(start_index, end_index - start_index);
	else
		material = str.substr(start_index);

	mesh_index = addMesh();
	setMeshMaterial(mesh_index, material);
	return true;
}

bool ObjModel :: readVertex (const string& str, ostream& r_logstream)
{
	double x;
	double y;
	double z;

	size_t index;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	x = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	y = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	z = atof(str.c_str() + index);

	addVertex(x, y, z);
	return true;
}

bool ObjModel :: readTextureCoordinates (const string& str, ostream& r_logstream)
{
	double u;
	double v;

	size_t index;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	u = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	v = atof(str.c_str() + index);

	addTextureCoordinate(u, v);
	return true;
}

bool ObjModel :: readNormal (const string& str, ostream& r_logstream)
{
	double x;
	double y;
	double z;

	size_t index;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	x = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	y = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	z = atof(str.c_str() + index);

	if(x == 0.0 && y == 0.0 && z == 0.0)
	{
		r_logstream << "Found a bad normal: #" << getNormalCount() << ", using (0, 0, 1)" << endl;
		addNormal(0.0, 0.0, 1.0);
	}
	else
		addNormal(x, y, z);
	return true;
}

bool ObjModel :: readPointSet (const string& str, ostream& r_logstream)
{
	const unsigned int NO_POINT_SET = ~0u;

	unsigned int point_set_index = NO_POINT_SET;
	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		int vertex;

		vertex = atoi(str.c_str() + token_index);
		if(vertex < 0)
			vertex += getVertexCount() + 1;
		if(vertex <= 0)
		{
			if(point_set_index != NO_POINT_SET)
				removeLastPointSet();
			return false;
		}

		if(point_set_index == NO_POINT_SET)
			point_set_index = addPointSet();

		// reduce indexes to start at 0 instead of 1
		assert(vertex >= 1);
		vertex--;

		addPointSetVertex(point_set_index, vertex);
	}

	// if we get here, all tokens were valid
	return true;
}

bool ObjModel :: readPolyline (const string& str, ostream& r_logstream)
{
	//
	//  This function reads a polyline of vertices in the
	//    model, not a line of the input file.
	//

	const unsigned int NO_LINE = ~0u;

	unsigned int polyline_index = NO_LINE;
	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		size_t number_index;

		int vertex;
		int texture_coordinates;

		number_index = token_index;

		vertex = atoi(str.c_str() + number_index);
		if(vertex < 0)
			vertex += getVertexCount() + 1;
		if(vertex <= 0)
		{
			if(polyline_index != NO_LINE)
				removeLastPolyline();
			return false;
		}

		number_index = nextSlashInToken(str, number_index);
		if(number_index == string::npos)
		{
			texture_coordinates = NO_TEXTURE_COORDINATES;
		}
		else
		{
			number_index++;

			if(isspace(str[number_index]))
				texture_coordinates = NO_TEXTURE_COORDINATES;
			else
			{
				texture_coordinates = atoi(str.c_str() + number_index);
				if(texture_coordinates < 0)
					texture_coordinates += getTextureCoordinateCount() + 1;
				if(texture_coordinates <= 0)
					return false;
			}
		}

		if(polyline_index == NO_LINE)
			polyline_index = addPolyline();

		// reduce indexes to start at 0 instead of 1
		assert(vertex >= 1);
		vertex--;

		if(texture_coordinates != NO_TEXTURE_COORDINATES)
		{
			assert(texture_coordinates >= 1);
			texture_coordinates--;
		}

		addPolylineVertex(polyline_index, vertex, texture_coordinates);
	}

	// if we get here, all tokens were valid
	return true;
}

bool ObjModel :: readFace (const string& str, ostream& r_logstream)
{
	const unsigned int NO_FACE = ~0u;

	unsigned int face_index = NO_FACE;
	unsigned int mesh_index;

	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	if(mv_meshes.empty())
		mesh_index = addMesh();
	else
		mesh_index = mv_meshes.size() - 1;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		size_t number_index;

		int vertex;
		int texture_coordinates;
		int normal;

		number_index = token_index;

		vertex = atoi(str.c_str() + number_index);
		if(vertex < 0)
			vertex += getVertexCount() + 1;
		if(vertex <= 0)
		{
			if(face_index != NO_FACE)
				removeLastFace(mesh_index);
			return false;
		}

		number_index = nextSlashInToken(str, number_index);
		if(number_index == string::npos)
		{
			texture_coordinates = NO_TEXTURE_COORDINATES;
			normal = NO_NORMAL;
		}
		else
		{
			number_index++;

			if(str[number_index] == '/')
				texture_coordinates = NO_TEXTURE_COORDINATES;
			else
			{
				texture_coordinates = atoi(str.c_str() + number_index);
				if(texture_coordinates < 0)
					texture_coordinates += getTextureCoordinateCount() + 1;
				if(texture_coordinates <= 0)
					return false;
			}

			number_index = nextSlashInToken(str, number_index);
			if(number_index == string::npos)
				normal = NO_NORMAL;
			else
			{
				number_index++;

				if(isspace(str[number_index]))
					normal = NO_NORMAL;
				else
				{
					normal = atoi(str.c_str() + number_index);
					if(normal < 0)
						normal += getNormalCount() + 1;
					if(normal <= 0)
						return false;
				}
			}
		}

		if(face_index == NO_FACE)
			face_index = addFace(mesh_index);

		// reduce indexes to start at 0 instead of 1
		assert(vertex >= 1);
		vertex--;

		if(texture_coordinates != NO_TEXTURE_COORDINATES)
		{
			assert(texture_coordinates >= 1);
			texture_coordinates--;
		}

		if(normal != NO_NORMAL)
		{
			assert(normal >= 1);
			normal--;
		}

		addFaceVertex(mesh_index, face_index, vertex, texture_coordinates, normal);
	}

	// if we get here, all tokens were valid
	return true;
}

void ObjModel :: removeLastPointSet ()
{
	assert(getPointSetCount() >= 1);

	mv_point_sets.pop_back();
	m_valid = false;
}

void ObjModel :: removeLastPolyline ()
{
	assert(getPolylineCount() >= 1);

	mv_polylines.pop_back();
	m_valid = false;
}

void ObjModel :: removeLastFace (unsigned int mesh)
{
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) >= 1);

	mv_meshes[mesh].mv_faces.pop_back();
	m_valid = false;
}

bool ObjModel :: invariant () const
{
	if(m_file_name == "") return false;
	return true;
}





ObjModel :: MaterialLibrary :: MaterialLibrary ()
{
	m_name         = "";
	mp_mtl_library = NULL;
}

ObjModel :: MaterialLibrary :: MaterialLibrary (const string& path, const string& name, ostream& r_logstream)
{
	m_name         = name;
	mp_mtl_library = &(MtlLibraryManager::get(path + name, r_logstream));
}

ObjModel :: MaterialLibrary :: MaterialLibrary (const ObjModel :: MaterialLibrary& original)
{
	m_name         = original.m_name;
	mp_mtl_library = original.mp_mtl_library;
}

ObjModel :: MaterialLibrary& ObjModel :: MaterialLibrary :: operator= (const ObjModel :: MaterialLibrary& original)
{
	if(&original != this)
	{
		m_name         = original.m_name;
		mp_mtl_library = original.mp_mtl_library;
	}

	return *this;
}





ObjModel :: PolylineVertex :: PolylineVertex ()
{
	m_vertex             = 0;
	m_texture_coordinate = NO_TEXTURE_COORDINATES;
}

ObjModel :: PolylineVertex :: PolylineVertex (unsigned int vertex, unsigned int texture_coordinates)
{
	m_vertex             = vertex;
	m_texture_coordinate = texture_coordinates;
}

ObjModel :: PolylineVertex :: PolylineVertex (const ObjModel :: PolylineVertex& original)
{
	m_vertex             = original.m_vertex;
	m_texture_coordinate = original.m_texture_coordinate;
}

ObjModel :: PolylineVertex& ObjModel :: PolylineVertex :: operator= (const ObjModel :: PolylineVertex& original)
{
	if(&original != this)
	{
		m_vertex             = original.m_vertex;
		m_texture_coordinate = original.m_texture_coordinate;
	}

	return *this;
}





ObjModel :: FaceVertex :: FaceVertex ()
{
	m_vertex             = 0;
	m_texture_coordinate = NO_TEXTURE_COORDINATES;
	m_normal             = NO_NORMAL;
}

ObjModel :: FaceVertex :: FaceVertex (unsigned int vertex, unsigned int texture_coordinates, unsigned int normal)
{
	m_vertex             = vertex;
	m_texture_coordinate = texture_coordinates;
	m_normal             = normal;
}

ObjModel :: FaceVertex :: FaceVertex (const ObjModel :: FaceVertex& original)
{
	m_vertex             = original.m_vertex;
	m_texture_coordinate = original.m_texture_coordinate;
	m_normal             = original.m_normal;
}

ObjModel :: FaceVertex& ObjModel :: FaceVertex :: operator= (const ObjModel :: FaceVertex& original)
{
	if(&original != this)
	{
		m_vertex             = original.m_vertex;
		m_texture_coordinate = original.m_texture_coordinate;
		m_normal             = original.m_normal;
	}

	return *this;
}




ObjModel :: Mesh :: Mesh () : mv_faces()
{
	m_material_name = "";
	mp_material     = NULL;
	m_all_triangles = true;
}

ObjModel :: Mesh :: Mesh (const string& material_name, Material* p_material) : mv_faces()
{
	m_material_name = material_name;
	mp_material     = p_material;
	m_all_triangles = true;
}

ObjModel :: Mesh :: Mesh (const ObjModel :: Mesh& original) : mv_faces(original.mv_faces)
{
	m_material_name = original.m_material_name;
	mp_material     = original.mp_material;
	m_all_triangles = original.m_all_triangles;
}

ObjModel :: Mesh& ObjModel :: Mesh :: operator= (const ObjModel :: Mesh& original)
{
	if(&original != NULL)
	{
		m_material_name = original.m_material_name;
		mp_material     = original.mp_material;
		mv_faces        = original.mv_faces;
		m_all_triangles = original.m_all_triangles;
	}

	return *this;
}
