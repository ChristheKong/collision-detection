//
//  NumericTests.cpp
//

#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "ObjLibrary/Vector3.h"

#include "GeometricCollisions.h"
#include "NumericTests.h"

using namespace std;
using namespace NumericTests;
namespace
{
	const int MAX_GRADE = 8;

	const bool   GENERATING_RESULTS = false;
	const string RESULT_IDENTIFIER  = "result:\t";

	const bool DEBUGGING_POINT_VS_SPHERE = false;
}



void NumericTests :: run (const string& filename)
{
	assert(filename != "");

	ifstream fin(filename.c_str());

	// set up cout formatting for printing table rows
	streamsize default_precision = std::cout.precision();
	cout << right << fixed << setprecision(2);

	// perform tests
	double average_grade = 0.0;
	average_grade += testPointVsSphere (fin);
	average_grade += testSphereVsSphere(fin);
	average_grade += testPointVsCuboid (fin);
	average_grade += testCuboidVsCuboid(fin);
	average_grade += testSphereVsCuboid(fin) * 2.0;
	average_grade /= 6.0;

	cout << "+=============+=========+=======+" << endl;
	cout << "| Total Grade | "
	     << setw(6) << (average_grade * 100.0) << "% | "
	     << (int)(average_grade * MAX_GRADE) << " / "
	     << MAX_GRADE << " |" << endl;
	cout << "+=============+=========+=======+" << endl;
	cout << endl;

	// disable special cout formatting for table
	cout.unsetf(ios_base::floatfield | ios_base::adjustfield);
	cout << setprecision(default_precision);

	fin.close();
}

double NumericTests :: testPointVsSphere (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	// print table header
	printTableHeader("Point vs. Sphere");

	// read test count
	int trial_count = readTrialCount(r_fin);
	if(DEBUGGING_POINT_VS_SPHERE)
		cout << "Running " << trial_count << " point vs. sphere tests" << endl;

	// perform the trials
	double grade_average = 0.0;
	for(int trial = 0; trial < trial_count; trial++)
	{
		if(DEBUGGING_POINT_VS_SPHERE)
			cout << "Trial " << trial << ":" << endl;

		// read first position
		Vector3 pos1 = readPosition(r_fin);
		if(DEBUGGING_POINT_VS_SPHERE)
			cout << "\tpos1:\t" << pos1 << endl;

		// no first dimenions to read
		readDimensionsPoint(r_fin, "size1:");
		if(DEBUGGING_POINT_VS_SPHERE)
			cout << "\tsize1:\tN/A" << endl;

		// read second position range
		PositionRange range2 = readPositionRange(r_fin);
		if(DEBUGGING_POINT_VS_SPHERE)
		{
			Vector3 pos_max = calculatePosition(range2.m_min,
			                                    range2.m_inc,
			                                    range2.m_count_x - 1,
			                                    range2.m_count_y - 1,
			                                    range2.m_count_z - 1);

			cout << "\tx2:\t" << range2.m_min.x
			     << "\tto "   << pos_max.x
			     << "\tby "   << range2.m_inc.x << endl;
			cout << "\ty2:\t" << range2.m_min.y
			     << "\tto "   << pos_max.y
			     << "\tby "   << range2.m_inc.y << endl;
			cout << "\tz2:\t" << range2.m_min.z
			     << "\tto "   << pos_max.z
			     << "\tby "   << range2.m_inc.z << endl;
		}

		// read second dimenions
		double radius2 = readDimensionsSphere(r_fin, "size2:");
		if(DEBUGGING_POINT_VS_SPHERE)
			cout << "\tsize2:\t" << radius2 << endl;

		// perform tests
		if(GENERATING_RESULTS)
			cout << RESULT_IDENTIFIER;
		if(DEBUGGING_POINT_VS_SPHERE)
			cout << "\tresults:\t(detected vs. read form file)" << endl;

		TestResults results;
		string read_results = readResults(r_fin);
		unsigned int result_index = 0;

		for(int z = 0; z < range2.m_count_z; z++)
			for(int y = 0; y < range2.m_count_y; y++)
				for(int x = 0; x < range2.m_count_x; x++)
				{
					Vector3 pos2           = calculatePosition(range2.m_min, range2.m_inc, x, y, z);
					bool is_detected       = GeometricCollisions::pointVsSphere(pos1, pos2, radius2);
					bool is_read_from_file = (read_results[result_index] == '1');
					result_index++;

					if(DEBUGGING_POINT_VS_SPHERE)
					{
						cout << "\t\t" << pos2 << ":\t"
						     << (is_detected       ? '1' : '0') << " vs. "
						     << (is_read_from_file ? '1' : '0') << endl;
					}

					updateResults(is_detected, is_read_from_file, results);

					if(GENERATING_RESULTS)
						cout << (is_detected ? '1' : '0');
				}

		if(GENERATING_RESULTS)
			cout << endl;

		// print table row
		grade_average += printTableRow(trial, results);
	}
	assert(trial_count != 0);
	grade_average /= trial_count;

	// print table footer
	printTableFooter(grade_average);

	assert(r_fin.is_open());
	assert(r_fin.good());
	return grade_average;
}

double NumericTests :: testSphereVsSphere (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	printTableHeader("Sphere vs. Sphere");

	int trial_count = readTrialCount(r_fin);
	double grade_average = 0.0;
	for(int trial = 0; trial < trial_count; trial++)
	{
		Vector3       pos1    = readPosition(r_fin);
		double        radius1 = readDimensionsSphere(r_fin, "size1:");
		PositionRange range2  = readPositionRange(r_fin);
		double        radius2 = readDimensionsSphere(r_fin, "size2:");

		if(GENERATING_RESULTS)
			cout << RESULT_IDENTIFIER;

		TestResults results;
		string read_results = readResults(r_fin);
		unsigned int result_index = 0;

		for(int z = 0; z < range2.m_count_z; z++)
			for(int y = 0; y < range2.m_count_y; y++)
				for(int x = 0; x < range2.m_count_x; x++)
				{
					Vector3 pos2           = calculatePosition(range2.m_min, range2.m_inc, x, y, z);
					bool is_detected       = GeometricCollisions::sphereVsSphere(pos1, radius1, pos2, radius2);
					bool is_read_from_file = (read_results[result_index] == '1');
					result_index++;

					updateResults(is_detected, is_read_from_file, results);
					if(GENERATING_RESULTS)
						cout << (is_detected ? '1' : '0');
				}

		if(GENERATING_RESULTS)
			cout << endl;

		grade_average += printTableRow(trial, results);
	}
	assert(trial_count != 0);
	grade_average /= trial_count;

	printTableFooter(grade_average);

	assert(r_fin.is_open());
	assert(r_fin.good());
	return grade_average;
}

double NumericTests :: testPointVsCuboid (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	printTableHeader("Point vs. Cuboid");

	int trial_count = readTrialCount(r_fin);
	double grade_average = 0.0;
	for(int trial = 0; trial < trial_count; trial++)
	{
		Vector3       pos1       = readPosition(r_fin);
		                           readDimensionsPoint(r_fin, "size1:");
		PositionRange range2     = readPositionRange(r_fin);
		Vector3       half_size2 = readDimensionsCuboid(r_fin, "size2:");

		if(GENERATING_RESULTS)
			cout << RESULT_IDENTIFIER;

		TestResults results;
		string read_results = readResults(r_fin);
		unsigned int result_index = 0;

		for(int z = 0; z < range2.m_count_z; z++)
			for(int y = 0; y < range2.m_count_y; y++)
				for(int x = 0; x < range2.m_count_x; x++)
				{
					Vector3 pos2           = calculatePosition(range2.m_min, range2.m_inc, x, y, z);
					bool is_detected       = GeometricCollisions::pointVsCuboid(pos1, pos2, half_size2);
					bool is_read_from_file = (read_results[result_index] == '1');
					result_index++;

					updateResults(is_detected, is_read_from_file, results);
					if(GENERATING_RESULTS)
						cout << (is_detected ? '1' : '0');
				}

		if(GENERATING_RESULTS)
			cout << endl;

		grade_average += printTableRow(trial, results);
	}
	assert(trial_count != 0);
	grade_average /= trial_count;

	printTableFooter(grade_average);

	assert(r_fin.is_open());
	assert(r_fin.good());
	return grade_average;
}

double NumericTests :: testCuboidVsCuboid (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	printTableHeader("Cuboid vs. Cuboid");

	int trial_count = readTrialCount(r_fin);
	double grade_average = 0.0;
	for(int trial = 0; trial < trial_count; trial++)
	{
		Vector3       pos1       = readPosition(r_fin);
		Vector3       half_size1 = readDimensionsCuboid(r_fin, "size1:");
		PositionRange range2     = readPositionRange(r_fin);
		Vector3       half_size2 = readDimensionsCuboid(r_fin, "size2:");

		if(GENERATING_RESULTS)
			cout << RESULT_IDENTIFIER;

		TestResults results;
		string read_results = readResults(r_fin);
		unsigned int result_index = 0;

		for(int z = 0; z < range2.m_count_z; z++)
			for(int y = 0; y < range2.m_count_y; y++)
				for(int x = 0; x < range2.m_count_x; x++)
				{
					Vector3 pos2           = calculatePosition(range2.m_min, range2.m_inc, x, y, z);
					bool is_detected       = GeometricCollisions::cuboidVsCuboid(pos1, half_size1, pos2, half_size2);
					bool is_read_from_file = (read_results[result_index] == '1');
					result_index++;

					updateResults(is_detected, is_read_from_file, results);
					if(GENERATING_RESULTS)
						cout << (is_detected ? '1' : '0');
				}

		if(GENERATING_RESULTS)
			cout << endl;

		grade_average += printTableRow(trial, results);
	}
	assert(trial_count != 0);
	grade_average /= trial_count;

	printTableFooter(grade_average);

	assert(r_fin.is_open());
	assert(r_fin.good());
	return grade_average;
}

double NumericTests :: testSphereVsCuboid (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	printTableHeader("Sphere vs. Cuboid (x2 Grade Weight)");

	int trial_count = readTrialCount(r_fin);
	double grade_average = 0.0;
	for(int trial = 0; trial < trial_count; trial++)
	{
		Vector3       pos1       = readPosition(r_fin);
		double        radius1    = readDimensionsSphere(r_fin, "size1:");
		PositionRange range2     = readPositionRange(r_fin);
		Vector3       half_size2 = readDimensionsCuboid(r_fin, "size2:");

		if(GENERATING_RESULTS)
			cout << RESULT_IDENTIFIER;

		TestResults results;
		string read_results = readResults(r_fin);
		unsigned int result_index = 0;

		for(int z = 0; z < range2.m_count_z; z++)
			for(int y = 0; y < range2.m_count_y; y++)
				for(int x = 0; x < range2.m_count_x; x++)
				{
					Vector3 pos2           = calculatePosition(range2.m_min, range2.m_inc, x, y, z);
					bool is_detected       = GeometricCollisions::sphereVsCuboid(pos1, radius1, pos2, half_size2);
					bool is_read_from_file = (read_results[result_index] == '1');
					result_index++;

					updateResults(is_detected, is_read_from_file, results);
					if(GENERATING_RESULTS)
						cout << (is_detected ? '1' : '0');
				}

		if(GENERATING_RESULTS)
			cout << endl;

		grade_average += printTableRow(trial, results);
	}
	assert(trial_count != 0);
	grade_average /= trial_count;

	printTableFooter(grade_average);

	assert(r_fin.is_open());
	assert(r_fin.good());
	return grade_average;
}



int NumericTests :: readTrialCount (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	int trial_count;
	r_fin >> trial_count;
	assert(r_fin.good());
	assert(trial_count >= 0);

	return trial_count;
}

Vector3 NumericTests :: readPosition (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	string dummy;
	Vector3 position;

	r_fin >> dummy >> position.x >> position.y >> position.z;
	assert(r_fin.good());
	assert(dummy == "pos1:");

	return position;
}

void NumericTests :: readDimensionsPoint (ifstream& r_fin,
                                          const string& identifier)
{
	assert(r_fin.is_open());
	assert(r_fin.good());
	assert(identifier != "");

	string dummy;

	r_fin >> dummy;
	assert(r_fin.good());
	assert(dummy == identifier);
}

double NumericTests :: readDimensionsSphere (ifstream& r_fin,
                                             const string& identifier)
{
	assert(r_fin.is_open());
	assert(r_fin.good());
	assert(identifier != "");

	string dummy;
	double sphere_radius;

	r_fin >> dummy >> sphere_radius;
	assert(r_fin.good());
	assert(dummy == identifier);
	assert(sphere_radius >= 0);

	return sphere_radius;
}

Vector3 NumericTests :: readDimensionsCuboid (ifstream& r_fin,
                                              const string& identifier)
{
	assert(r_fin.is_open());
	assert(r_fin.good());
	assert(identifier != "");

	string dummy;
	Vector3 half_sizes;

	r_fin >> dummy >> half_sizes.x >> half_sizes.y >> half_sizes.z;
	assert(r_fin.good());
	assert(dummy == identifier);
	assert(half_sizes.isAllComponentsNonNegative());

	return half_sizes;
}

PositionRange NumericTests :: readPositionRange (ifstream& r_fin)
{
	assert(r_fin.is_open());
	assert(r_fin.good());

	string dummy;
	PositionRange result;

	r_fin >> dummy >> result.m_min.x >> result.m_inc.x >> result.m_count_x;
	assert(r_fin.good());
	assert(dummy == "x2:");
	assert(result.m_count_x > 0);

	r_fin >> dummy >> result.m_min.y >> result.m_inc.y >> result.m_count_y;
	assert(r_fin.good());
	assert(dummy == "y2:");
	assert(result.m_count_y > 0);

	r_fin >> dummy >> result.m_min.z >> result.m_inc.z >> result.m_count_z;
	assert(r_fin.good());
	assert(dummy == "z2:");
	assert(result.m_count_z > 0);

	assert(result.m_inc.isAllComponentsNonZero());
	return result;
}

string NumericTests :: readResults (ifstream& r_fin)
{
	string dummy;
	string results;

	r_fin >> dummy >> results;
	assert(r_fin.good());
	assert(dummy == "result:");
	unsigned int results_count = results.length();
	for(unsigned int i = 0; i < results_count; i++)
		assert(results[i] == '1' || results[i] == '0');

	return results;
}

Vector3 NumericTests :: calculatePosition (const Vector3& pos_min,
                                           const Vector3& pos_inc,
                                           int move_x,
                                           int move_y,
                                           int move_z)
{
	assert(pos_inc.isAllComponentsNonZero());

	Vector3 pos = pos_min;
	pos.x += pos_inc.x * move_x;
	pos.y += pos_inc.y * move_y;
	pos.z += pos_inc.z * move_z;
	return pos;
}

void NumericTests :: updateResults (bool is_detected,
                                    bool is_read_from_file,
                                    TestResults& results)
{
	if(is_detected)
	{
		if(is_read_from_file)
			results.m_true_positive_count++;
		else
			results.m_false_positive_count++;
	}
	else
	{
		if(is_read_from_file)
			results.m_false_negative_count++;
		else
			results.m_true_negative_count++;
	}
}



void NumericTests :: printTableHeader (const string& title)
{
	assert(title != "");

	cout << "+-------------------------------------------------------------+---------+" << endl;
	cout << "| " << left << setw(69) << title << right << " |" << endl;
	cout << "+---------+-------------------------+-------------------------+---------+" << endl;
	cout << "|         |        Positives        |         Negatives       |         |" << endl;
	cout << "|  Trial  +-------------------------+-------------------------+  Grade  |" << endl;
	cout << "|         | True  | False | Percent | True  | False | Percent |         |" << endl;
	cout << "+---------+-------+-------+---------+-------+-------+---------+---------+" << endl;
}

double NumericTests :: printTableRow (unsigned int trial,
                                      const TestResults& results)
{
	unsigned int total_positive_count = results.m_true_positive_count +
	                                    results.m_false_positive_count;
	unsigned int total_negative_count = results.m_true_negative_count +
	                                    results.m_false_negative_count;

	double grade = 1.0;

	cout <<  "| " << setw(7) << trial;
	cout << " | " << setw(5) << results.m_true_positive_count;
	cout << " | " << setw(5) << results.m_false_positive_count;

	if(total_positive_count > 0)
	{
		double true_positive_fraction = results.m_true_positive_count / (double)(total_positive_count);
		assert(true_positive_fraction >= 0.0);
		assert(true_positive_fraction <= 1.0);

		grade *= true_positive_fraction;

		cout << " | " << setw(6) << ( true_positive_fraction * 100.0) << "%";
	}
	else
		cout << " |    N/A ";

	cout << " | " << setw(5) << results.m_true_negative_count;
	cout << " | " << setw(5) << results.m_false_negative_count;

	if(total_negative_count > 0)
	{
		double true_negative_fraction = results.m_true_negative_count / (double)(total_negative_count);
		assert(true_negative_fraction >= 0.0);
		assert(true_negative_fraction <= 1.0);

		grade *= true_negative_fraction;

		cout << " | " << setw(6) << ( true_negative_fraction * 100.0) << "%";
	}
	else
		cout << " |    N/A ";

	cout << " | " << setw(6) << (grade * 100.0) << "%";
	cout << " |"  << endl;

	return grade;
}

void NumericTests :: printTableFooter (double average)
{
	assert(average >= 0.0);
	assert(average <= 1.0);

	cout << "+---------+-------+-------+---------+-------+-------+---------+---------+" << endl;
	cout << "| Average |                         |                         | "
	     << setw(6) << (average * 100.0) << "% |" << endl;
	cout << "+---------+-------------------------+-------------------------+---------+" << endl;
	cout << endl;
}

