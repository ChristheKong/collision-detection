//
//  MtlLibrary.cpp
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
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <vector>

#include "ObjSettings.h"
#include "ObjStringParsing.h"
#include "Material.h"
#include "MtlLibrary.h"

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
	const char* DEFAULT_FILE_NAME = "unnamed.mtl";
	const char* DEFAULT_FILE_PATH = "";

	const unsigned int COLOUR_TARGET_TYPES    = 3;
	const unsigned int COLOUR_TARGET_AMBIENT  = 0;
	const unsigned int COLOUR_TARGET_DIFFUSE  = 1;
	const unsigned int COLOUR_TARGET_SPECULAR = 2;

	const unsigned int CHANNEL_TARGET_TYPES             = 5;
	const unsigned int CHANNEL_TARGET_SPECULAR_EXPONENT = 0;
	const unsigned int CHANNEL_TARGET_TRANSPARENCY      = 1;
	const unsigned int CHANNEL_TARGET_DECAL             = 2;
	const unsigned int CHANNEL_TARGET_DISPLACEMENT      = 3;
	const unsigned int CHANNEL_TARGET_BUMP              = 4;

}  // end of anonymous namespace



const unsigned int MtlLibrary :: NO_SUCH_MATERIAL = ~0u;



bool MtlLibrary :: isMaterialActive ()
{
	return Material::isMaterialActive();
}

void MtlLibrary :: deactivate ()
{
	Material::deactivate();
}



MtlLibrary :: MtlLibrary ()
		: m_file_name(DEFAULT_FILE_NAME),
		  m_file_path(DEFAULT_FILE_PATH),
		  m_is_loaded_successfully(true),
		  mvp_materials ()
{
	makeEmpty();

	assert(invariant());
}

MtlLibrary :: MtlLibrary (const string& filename)
		: m_file_name(DEFAULT_FILE_NAME),
		  m_file_path(DEFAULT_FILE_PATH),
		  m_is_loaded_successfully(true),
		  mvp_materials ()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	load(filename);

	assert(invariant());
}

MtlLibrary :: MtlLibrary (const string& filename, const string& logfile)
		: m_file_name(DEFAULT_FILE_NAME),
		  m_file_path(DEFAULT_FILE_PATH),
		  m_is_loaded_successfully(true),
		  mvp_materials ()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	load(filename, logfile);

	assert(invariant());
}

MtlLibrary :: MtlLibrary (const string& filename, ostream& r_logstream)
		: m_file_name(DEFAULT_FILE_NAME),
		  m_file_path(DEFAULT_FILE_PATH),
		  m_is_loaded_successfully(true),
		  mvp_materials ()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	load(filename, r_logstream);

	assert(invariant());
}

MtlLibrary :: MtlLibrary (const MtlLibrary& original)
		: m_file_name(original.m_file_name),
		  m_file_path(original.m_file_path),
		  m_is_loaded_successfully(original.m_is_loaded_successfully),
		  mvp_materials ()
{
	copy(original);

	assert(invariant());
}

MtlLibrary& MtlLibrary :: operator= (const MtlLibrary& original)
{
	if(&original != this)
	{
		removeAll();

		m_file_name = original.m_file_name;
		m_file_path = original.m_file_path;
		m_is_loaded_successfully = original.m_is_loaded_successfully;

		copy(original);
	}

	assert(invariant());
	return *this;
}

MtlLibrary :: ~MtlLibrary ()
{
	removeAll();
}



const string& MtlLibrary :: getFileName () const
{
	return m_file_name;
}

const string& MtlLibrary :: getFilePath () const
{
	return m_file_path;
}

string MtlLibrary :: getFileNameWithPath () const
{
	return m_file_path + m_file_name;
}

bool MtlLibrary :: isLoadedSuccessfully () const
{
	return m_is_loaded_successfully;
}

bool MtlLibrary :: isEmpty () const
{
	return mvp_materials.empty();
}

unsigned int MtlLibrary :: getMaterialCount () const
{
	return mvp_materials.size();
}

bool MtlLibrary :: isMaterial (const string& name) const
{
	assert(name != "");

	string lower = toLowercase(name);

	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		if(mvp_materials[i]->getName() == lower)
			return true;

	return false;
}

unsigned int MtlLibrary :: getMaterialIndex (const string& name) const
{
	assert(name != "");

	string lower = toLowercase(name);

	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		if(mvp_materials[i]->getName() == lower)
			return i;

	return NO_SUCH_MATERIAL;
}

const string& MtlLibrary :: getMaterialName (unsigned int index) const
{
	assert(index < getMaterialCount());

	return mvp_materials[index]->getName();
}

bool MtlLibrary :: isMaterialSeperateSpecular (unsigned int index) const
{
	assert(index < getMaterialCount());

	return mvp_materials[index]->isSeperateSpecular();
}

Material* MtlLibrary :: getMaterial (unsigned int index)
{
	assert(index < getMaterialCount());

	return mvp_materials[index];
}

const Material* MtlLibrary :: getMaterial (unsigned int index) const
{
	assert(index < getMaterialCount());

	return mvp_materials[index];
}

Material* MtlLibrary :: getMaterial (const string& name)
{
	assert(name != "");

	unsigned int index = getMaterialIndex(name);

	if(index == NO_SUCH_MATERIAL)
		return NULL;
	else
		return mvp_materials[index];
}

const Material* MtlLibrary :: getMaterial (const string& name) const
{
	assert(name != "");

	unsigned int index = getMaterialIndex(name);

	if(index == NO_SUCH_MATERIAL)
		return NULL;
	else
		return mvp_materials[index];
}

void MtlLibrary :: activateMaterial (const std::string& name) const
{
	assert(name != "");

	unsigned int index = getMaterialIndex(name);
	if(index != NO_SUCH_MATERIAL)
		mvp_materials[index]->activate();
}

void MtlLibrary :: activateMaterialSeperateSpecular (const std::string& name) const
{
	assert(name != "");

	unsigned int index = getMaterialIndex(name);
	if(index != NO_SUCH_MATERIAL)
		mvp_materials[index]->activateSeperateSpecular();
}

void MtlLibrary :: print () const
{
	print(cout);
}

void MtlLibrary :: print (ostream& r_outstream) const
{
	r_outstream << getFileNameWithPath() << ":" << endl;

	for(unsigned int i = 0; i < mvp_materials.size(); i++)
	{
		mvp_materials[i]->print();
		r_outstream << endl;
	}
}

void MtlLibrary :: save (const string& filename) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	save(filename, cerr);
}

void MtlLibrary :: save (const string& filename, const string& logfile) const
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
}

void MtlLibrary :: save (const string& filename, ostream& r_logstream) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	ofstream output_file;

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
	//    -> how many materials
	//  Materials
	//  Footer
	//
	//  Sections are seperated by 3 blank lines.
	//

	output_file << "#" << endl;
	output_file << "# " << getFileName() << endl;
	output_file << "#" << endl;
	output_file << "# " << getMaterialCount() << " materials" << endl;
	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		output_file << "#   " << mvp_materials[i]->getName() << endl;
	output_file << "#" << endl;
	output_file << endl;
	output_file << endl;
	output_file << endl;

	for(unsigned int i2 = 0; i2 < mvp_materials.size(); i2++)
	{
		mvp_materials[i2]->save(output_file);
		output_file << endl;
		output_file << endl;
	}

	output_file << "# End of " << getFileName() << endl;
	output_file << endl;

	output_file.close();
}



void MtlLibrary :: makeEmpty ()
{
	m_file_name = DEFAULT_FILE_NAME;
	m_file_path = DEFAULT_FILE_PATH;
	m_is_loaded_successfully = true;

	removeAll();

	assert(invariant());
}

void MtlLibrary :: load (const string& filename)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	load(filename, cerr);

	assert(invariant());
}

void MtlLibrary :: load (const string& filename, const string& logfile)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream;

	logstream.open(logfile.c_str());
	load(filename, logstream);
	logstream.close();

	assert(invariant());
}

void MtlLibrary :: load (const string& filename, ostream& r_logstream)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	ifstream input_file;
	unsigned int line_count;

	removeAll();

	m_is_loaded_successfully = true;
	setFileNameWithPath(filename);

	input_file.open(filename.c_str());
	if(!input_file.is_open())
	{
		r_logstream << "Error: File \"" << filename << "\" does not exist" << endl;
		input_file.close();
		m_is_loaded_successfully = false;

		assert(invariant());
		return;
	}

	//
	//  Format is available at
	//
	//  http://paulbourke.net/dataformats/mtl/
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

		if(isspace(line[0]))
			line = line.substr(nextToken(line, 0));

		valid = true;
		if(length < 3)
			valid = false;
		else
		{
			if(line.length() >= 7 && line.substr(0, 6) == "newmtl" && isspace(line[6]))
				valid = readMaterialStart(line.substr(7), r_logstream);
			else if(line.length() >= 6 && line.substr(0, 5) == "illum" && isspace(line[5]))
				valid = readIlluminationMode(line.substr(6), r_logstream);
			else if(line[0] == 'K' && line[1] == 'a' && isspace(line[2]))
				valid = readColour(line.substr(3), COLOUR_TARGET_AMBIENT, r_logstream);
			else if(line[0] == 'K' && line[1] == 'd' && isspace(line[2]))
				valid = readColour(line.substr(3), COLOUR_TARGET_DIFFUSE, r_logstream);
			else if(line[0] == 'K' && line[1] == 's' && isspace(line[2]))
				valid = readColour(line.substr(3), COLOUR_TARGET_SPECULAR, r_logstream);
			else if(line[0] == 'N' && line[1] == 's' && isspace(line[2]))
				valid = readSpecularExponent(line.substr(3), r_logstream);
			else if(line[0] == 'd' && isspace(line[1]))
				valid = readTransparency(line.substr(2), r_logstream);
			else if(line[0] == 'T' && line[1] == 'r' && isspace(line[2]))
				valid = readTransparency(line.substr(3), r_logstream);
			else if(line[0] == 'N' && line[1] == 'i' && isspace(line[2]))
				valid = readOpticalDensity(line.substr(3), r_logstream);
			else if(line[0] == 'T' && line[1] == 'f' && isspace(line[2]))
				valid = readTransmissionFilter(line.substr(3), r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "map_Ka" && isspace(line[6]))
				valid = readMapColour(line.substr(7), COLOUR_TARGET_AMBIENT, r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "map_Kd" && isspace(line[6]))
				valid = readMapColour(line.substr(7), COLOUR_TARGET_DIFFUSE, r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "map_Ks" && isspace(line[6]))
				valid = readMapColour(line.substr(7), COLOUR_TARGET_SPECULAR, r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "map_Ns" && isspace(line[6]))
				valid = readMapChannel(line.substr(7), CHANNEL_TARGET_SPECULAR_EXPONENT, r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "map_Ns" && isspace(line[6]))
				valid = readMapChannel(line.substr(7), CHANNEL_TARGET_SPECULAR_EXPONENT, r_logstream);
			else if(line.length() >= 6 && line.substr(0, 5) == "map_d" && isspace(line[5]))
				valid = readMapChannel(line.substr(6), CHANNEL_TARGET_TRANSPARENCY, r_logstream);
			else if(line.length() >= 7 && line.substr(0, 6) == "map_Tr" && isspace(line[6]))
				valid = readMapChannel(line.substr(7), CHANNEL_TARGET_TRANSPARENCY, r_logstream);
			else if(line.length() >= 6 && line.substr(0, 5) == "decal" && isspace(line[5]))
				valid = readMapChannel(line.substr(6), CHANNEL_TARGET_DECAL, r_logstream);
			else if(line.length() >= 5 && line.substr(0, 4) == "disp" && isspace(line[4]))
				valid = readMapChannel(line.substr(5), CHANNEL_TARGET_DISPLACEMENT, r_logstream);
			else if(line.length() >= 5 && line.substr(0, 4) == "bump" && isspace(line[4]))
				valid = readMapChannel(line.substr(5), CHANNEL_TARGET_BUMP, r_logstream);
			else if(line[0] == 'K' && line[1] == 'm' && isspace(line[2]))	// non-standard - is this what it does?
				valid = readBumpMapMultiplier(line.substr(3), r_logstream);
			else
				valid = false;
		}

		if(!valid)
			r_logstream << "Line " << setw(6) << line_count << " of file \"" << filename << "\" is invalid: \"" << line << "\"" << endl;
	}

	warnIfLastMaterialIsInvisible(r_logstream);
	input_file.close();

	assert(invariant());
}

void MtlLibrary :: setFileName (const string& file_name)
{
	assert(file_name != "");

	m_file_name = toLowercase(file_name);

	assert(invariant());
}

void MtlLibrary :: setFilePath (const string& file_path)
{
	m_file_path = toLowercase(file_path);

	assert(invariant());
}

void MtlLibrary :: setFileNameWithPath (const string& filename)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	string lowercase = toLowercase(filename);

	size_t last_slash = lowercase.find_last_of("/\\");
	if(last_slash != string::npos)
	{
		last_slash++;
		m_file_name = lowercase.substr(last_slash);
		m_file_path = lowercase.substr(0, last_slash);
	}
	else
	{
		m_file_name = lowercase;
		m_file_path = "";
	}

	assert(invariant());
}

unsigned int MtlLibrary :: add (Material* p_material)
{
	assert(p_material != NULL);
	assert(!isMaterial(p_material->getName()));

	unsigned int index = mvp_materials.size();
	mvp_materials.push_back(p_material);

	assert(invariant());
	return index;
}

void MtlLibrary :: removeAll ()
{
	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		delete mvp_materials[i];
	mvp_materials.clear();

	assert(mvp_materials.size() == 0);
	assert(invariant());
}

void MtlLibrary :: loadDisplayTextures ()
{
#ifdef OBJ_LIBRARY_PATH_PROPAGATION
	string propagated_path = m_file_path;
#else
	string propagated_path = "";
#endif

	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		mvp_materials[i]->loadDisplayTextures(propagated_path);

	assert(invariant());
}

void MtlLibrary :: loadAllTextures ()
{
#ifdef OBJ_LIBRARY_PATH_PROPAGATION
	string propagated_path = m_file_path;
#else
	string propagated_path = "";
#endif

	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		mvp_materials[i]->loadAllTextures(propagated_path);

	assert(invariant());
}



void MtlLibrary :: warnIfLastMaterialIsInvisible (ostream& r_outstream) const
{
	if(mvp_materials.empty())
		return;

	unsigned int current_material;

	assert(mvp_materials.size() >= 1);
	current_material = mvp_materials.size() - 1;
	if(mvp_materials[current_material]->getTransparency() == 0.0)
	{
		r_outstream << "Material " << mvp_materials[current_material]->getName()
		            << " in file " << getFileNameWithPath() << " is entirely transprent" << endl;
	}
}

bool MtlLibrary :: readMaterialStart (const string& str, ostream& r_logstream)
{
	size_t start;
	size_t length;
	string name;

	warnIfLastMaterialIsInvisible(r_logstream);

	if(isspace(str[0]))
		start = nextToken(str, 0);
	else
		start = 0;

	length = getTokenLength(str, start);
	name = str.substr(start, length);

	if(name == "" || isMaterial(name))
		return false;

#ifdef OBJ_LIBRARY_PATH_PROPAGATION
	string propagated_path = m_file_path;
#else
	string propagated_path = "";
#endif

	add(new Material(toLowercase(name), propagated_path));
	return true;
}

bool MtlLibrary :: readIlluminationMode (const string& str, ostream& r_logstream)
{
	size_t index;

	unsigned int illumination_mode;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	switch(atoi(str.c_str() + index))
	{
	case 0: illumination_mode = Material::ILLUMINATION_CONSTANT; break;
	case 1: illumination_mode = Material::ILLUMINATION_PHONG_NO_SPECULAR; break;
	case 2: illumination_mode = Material::ILLUMINATION_PHONG; break;
	case 3: illumination_mode = Material::ILLUMINATION_RAY_REFLECTIVE; break;
	case 4: illumination_mode = Material::ILLUMINATION_RAY_GLASS; break;
	case 5: illumination_mode = Material::ILLUMINATION_RAY_FRESNEL; break;
	case 6: illumination_mode = Material::ILLUMINATION_RAY_REFRACTION; break;
	case 7: illumination_mode = Material::ILLUMINATION_RAY_FRESNEL_REFRACTION; break;
	case 8: illumination_mode = Material::ILLUMINATION_REFLECTIVE; break;
	case 9: illumination_mode = Material::ILLUMINATION_GLASS; break;
	case 10: illumination_mode = Material::ILLUMINATION_RAY_INVISIBLE_SHADOWS; break;
	default: return false;
	}

	mvp_materials[current_material]->setIlluminationMode(illumination_mode);
	return true;
}

bool MtlLibrary :: readColour (const string& str, unsigned int target, ostream& r_logstream)
{
	assert(target < COLOUR_TARGET_TYPES);

	double red;
	double green;
	double blue;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	red = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
	{
		//  Providing only one value for a colour is legal.
		//    Greyscale is assumed.

		switch(target)
		{
		case COLOUR_TARGET_AMBIENT:
			mvp_materials[current_material]->setAmbientColour(red, red, red);
			break;
		case COLOUR_TARGET_DIFFUSE:
			mvp_materials[current_material]->setDiffuseColour(red, red, red);
			break;
		case COLOUR_TARGET_SPECULAR:
			mvp_materials[current_material]->setSpecularColour(red, red, red);
			break;
		}

		return true;
	}

	green = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	blue = atof(str.c_str() + index);

	switch(target)
	{
	case COLOUR_TARGET_AMBIENT:
		mvp_materials[current_material]->setAmbientColour(red, green, blue);
		break;
	case COLOUR_TARGET_DIFFUSE:
		mvp_materials[current_material]->setDiffuseColour(red, green, blue);
		break;
	case COLOUR_TARGET_SPECULAR:
		mvp_materials[current_material]->setSpecularColour(red, green, blue);
		break;
	}

	return true;
}

bool MtlLibrary :: readSpecularExponent (const string& str, ostream& r_logstream)
{
	double exponent;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	exponent = atof(str.c_str() + index);

	mvp_materials[current_material]->setSpecularExponent(exponent);
	return true;
}

bool MtlLibrary :: readTransparency (const string& str, ostream& r_logstream)
{
	double transparency;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	transparency = atof(str.c_str() + index);
	if(transparency < 0.0)
		return false;
	if(transparency > 1.0)
		return false;

	mvp_materials[current_material]->setTransparency(transparency);
	return true;
}

bool MtlLibrary :: readOpticalDensity (const std::string& str, std::ostream& r_logstream)
{
	double optical_density;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	optical_density = atof(str.c_str() + index);

	mvp_materials[current_material]->setOpticalDensity(optical_density);
	return true;
}

bool MtlLibrary :: readTransmissionFilter (const string& str, ostream& r_logstream)
{
	double red;
	double green;
	double blue;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	red = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
	{
		//  Providing only one value for a colour is legal.
		//    Greyscale is assumed.

		mvp_materials[current_material]->setTransmissionFilter(red, red, red);
		return true;
	}

	green = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	blue = atof(str.c_str() + index);

	mvp_materials[current_material]->setTransmissionFilter(red, green, blue);
	return true;
}

bool MtlLibrary :: readBumpMapMultiplier (const std::string& str, std::ostream& r_logstream)
{
	double multiplier;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	multiplier = atof(str.c_str() + index);

	mvp_materials[current_material]->setBumpMapMultiplier(multiplier);
	return true;
}



bool MtlLibrary :: readMapColour (const string& str, unsigned int target, ostream& r_logstream)
{
	assert(target < COLOUR_TARGET_TYPES);

	string filename;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	filename = str.substr(index, getTokenLength(str, index));
	if(filename == "" || isMaterial(filename))
		return false;

	switch(target)
	{
	case COLOUR_TARGET_AMBIENT:
		mvp_materials[current_material]->setAmbientMap(filename);
		break;
	case COLOUR_TARGET_DIFFUSE:
		mvp_materials[current_material]->setDiffuseMap(filename);
		break;
	case COLOUR_TARGET_SPECULAR:
		mvp_materials[current_material]->setSpecularMap(filename);
		break;
	}

	return true;
}

bool MtlLibrary :: readMapChannel (const string& str, unsigned int target, ostream& r_logstream)
{
	assert(target < CHANNEL_TARGET_TYPES);

	string filename;
	unsigned char channel;
	double bump_multiplier = 1.0;

	size_t index;
	unsigned int current_material;

	current_material = mvp_materials.size() - 1;

	if(target == CHANNEL_TARGET_DECAL)
		channel = Material::CHANNEL_MATTE;
	else
		channel = Material::CHANNEL_LUMINANCE;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	filename = str.substr(index, getTokenLength(str, index));
	if(filename == "" || isMaterial(filename))
		return false;

	index = nextToken(str, index);
	while(index != string::npos)
	{
		string token;

		token = str.substr(index, getTokenLength(str, index));
		if(token == "-imfchan")
		{
			index = nextToken(str, index);
			if(index == string::npos)
				return false;

			token = str.substr(index, getTokenLength(str, index));
			if(token == "r")
				channel = Material::CHANNEL_RED;
			else if(token == "g")
				channel = Material::CHANNEL_GREEN;
			else if(token == "b")
				channel = Material::CHANNEL_BLUE;
			else if(token == "m")
				channel = Material::CHANNEL_MATTE;
			else if(token == "l")
				channel = Material::CHANNEL_LUMINANCE;
			else if(token == "z")
				channel = Material::CHANNEL_Z_DEPTH;
			else
				return false;
		}
		else if(token == "-bm")
		{
			if(channel != CHANNEL_TARGET_BUMP)
				return false;

			index = nextToken(str, index);
			if(index == string::npos)
				return false;

			bump_multiplier = atof(str.c_str() + index);
		}

		index = nextToken(str, index);
	}

	switch(target)
	{
	case CHANNEL_TARGET_SPECULAR_EXPONENT:
		mvp_materials[current_material]->setSpecularExponentMap(filename, channel);
		break;
	case CHANNEL_TARGET_TRANSPARENCY:
		mvp_materials[current_material]->setTransparencyMap(filename, channel);
		break;
	case CHANNEL_TARGET_DECAL:
		mvp_materials[current_material]->setDecalMap(filename, channel);
		break;
	case CHANNEL_TARGET_DISPLACEMENT:
		mvp_materials[current_material]->setDisplacementMap(filename, channel);
		break;
	case CHANNEL_TARGET_BUMP:
		mvp_materials[current_material]->setBumpMap(filename, channel, bump_multiplier);
		break;
	}

	return true;
}

void MtlLibrary :: copy (const MtlLibrary& original)
{
	assert(mvp_materials.size() == 0);

	unsigned int material_count = original.mvp_materials.size();
	mvp_materials.resize(material_count);
	for(unsigned int i = 0; i < material_count; i++)
	{
		assert(i < mvp_materials.size());
		assert(i < original.mvp_materials.size());
		assert(original.mvp_materials[i] != NULL);
		mvp_materials[i] = new Material(*(original.mvp_materials[i]));
	}

	assert(invariant());
}

bool MtlLibrary :: invariant () const
{
	if(m_file_name == "") return false;

	for(unsigned int i = 0; i < mvp_materials.size(); i++)
		if(mvp_materials[i] == NULL)
			return false;

	return true;
}
