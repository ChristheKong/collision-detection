//
//  NumericTests.h
//
//  A module to perform mathematically-determined tests on the
//    GeometricCollisions module and display the results as
//    text.
//

#ifndef NUMERIC_TESTS_H
#define NUMERIC_TESTS_H

#include <string>
#include <fstream>



//
//  NumericTests
//
//  A namespace to contain the functions that perform systematic
//    mathematical tests on the GeometricCollisions module.
//
//  The input file is divided into 5 parts: one for each
//    function in GeometricCollisions to test.  Each part is
//    structured as follows:
//    -> The number of trails to run between solids of these
//       types
//    -> For each trial:
//      -> The position of the first solid
//      -> The dimensions of the first solid
//        -> A blank line if the object is a point
//        -> 1 value for radius if the solid is a sphere
//        -> 3 values for half sizes if the solid is a cuboid
//      -> A range of positions for the second object
//        -> For each axis
//        -> A minimum, increment, and count
//      -> The dimensions of the second solid
//      -> The correct results of the tests
//        -> '1' for intersection
//        -> '0' for no intersection
//        -> All together in a long string
//           -> no spaces or line breaks
//


namespace NumericTests
{
//
//  PositionRange
//
//  A record to keep track of the range of positions to check
//    for the second solid.
//

struct PositionRange
{
	Vector3 m_min;
	Vector3 m_inc;
	int m_count_x;
	int m_count_y;
	int m_count_z;
};



//
//  TestResults
//
//  A record to keep track of the accuracy of the test results.
//

struct TestResults
{
	unsigned int m_true_positive_count;
	unsigned int m_false_positive_count;
	unsigned int m_true_negative_count;
	unsigned int m_false_negative_count;

	TestResults()
			: m_true_positive_count(0),
			  m_false_positive_count(0),
			  m_true_negative_count(0),
			  m_false_negative_count(0)
	{
	}
};



//
//  run
//
//  Purpose: To perform the systematic tests and display the
//           results for all the functions.
//  Parameter(s):
//    <1> filename: The file to read the test information from
//  Precondition(s):
//    <1> filename != ""
//  Returns: N/A
//  Side Effect: The numerical tests indicated in file filename
//               are performed.  The results are printed to
//               standard output.
//

	void run (const std::string& filename);

//
//  testPointVsSphere
//  testSphereVsSphere
//  testPointVsCuboid
//  testCuboidVsCuboid
//  testSphereVsCuboid
//
//  Purpose: To perform the systematic tests and display the
//           results for the indicated function.
//  Parameter(s):
//    <1> r_fin: A reference to the file to read the test
//               information from
//  Precondition(s):
//    <1> r_fin.is_open()
//    <2> r_fin.good()
//  Returns: The average grade for the trials conducted.
//  Side Effect: The numerical tests indicated in file filename
//               are performed.  The results are printed to
//               standard output.  The read position for r_fin
//               is advanced as the values are read.
//

	double testPointVsSphere  (std::ifstream& r_fin);
	double testSphereVsSphere (std::ifstream& r_fin);
	double testPointVsCuboid  (std::ifstream& r_fin);
	double testCuboidVsCuboid (std::ifstream& r_fin);
	double testSphereVsCuboid (std::ifstream& r_fin);

//
//  readTrialCount
//
//  Purpose: To read in and validate the number of trials for
//           this combination of solids.
//  Parameter(s):
//    <1> r_fin: A reference to the file to read the test
//               information from
//  Precondition(s):
//    <1> r_fin.is_open()
//    <2> r_fin.good()
//  Returns: The number of trials.
//  Side Effect: The number of trials is read from r_fin.
//               Assertions are used to ensure that the value
//               read is appropriate.
//

	int readTrialCount (std::ifstream& r_fin);

//
//  readPosition
//
//  Purpose: To read in and validate the position for the first
//           solid.
//  Parameter(s):
//    <1> r_fin: A reference to the file to read the test
//               information from
//  Precondition(s):
//    <1> r_fin.is_open()
//    <2> r_fin.good()
//  Returns: The posiiton for the solid.
//  Side Effect: The values for the dimensions are read from
//               r_fin.  Assertions are used to ensure that the
//               values read are appropriate.
//

	Vector3 readPosition (std::ifstream& r_fin);

//
//  readDimensionsPoint
//  readDimensionsSphere
//  readDimensionsCuboid
//
//  Purpose: To read in and validate the dimensions for a
//           point/sphere/cuboid.
//  Parameter(s):
//    <1> r_fin: A reference to the file to read the test
//               information from
//    <2> identifier: The identifier token that must be at the
//                    beginning of the dimension data
//  Precondition(s):
//    <1> r_fin.is_open()
//    <2> r_fin.good()
//    <3> identifier != ""
//  Returns: Nothing for a point.
//           The radius for a sphere.
//           The hgalf-sizes for a cuboid.
//  Side Effect: The values for the dimensions are read from
//               r_fin.  Assertions are used to ensure that the
//               values read are appropriate.
//

	void    readDimensionsPoint (std::ifstream& r_fin,
	                             const std::string& identifier);
	double  readDimensionsSphere(std::ifstream& r_fin,
	                             const std::string& identifier);
	Vector3 readDimensionsCuboid(std::ifstream& r_fin,
	                             const std::string& identifier);

//
//  readPositionRange
//
//  Purpose: To read in and validate the range of positions for
//           the second solid.
//  Parameter(s):
//    <1> r_fin: A reference to the file to read the test
//               information from
//  Precondition(s):
//    <1> r_fin.is_open()
//    <2> r_fin.good()
//  Returns: The range of positions for the second solid to
//           test.
//  Side Effect: The values for the range of positions to test
//               are read from r_fin.  Assertions are used to
//               ensure that the values read are appropriate.
//

	PositionRange readPositionRange (std::ifstream& r_fin);

//
//  readResults
//
//  Purpose: To read in and validate the results string.
//  Parameter(s):
//    <1> r_fin: A reference to the file to read the test
//               information from
//  Precondition(s):
//    <1> r_fin.is_open()
//    <2> r_fin.good()
//  Returns: A strig containing the results for the tests.
//  Side Effect: The results line is read from r_fin.
//               Assertions are used to ensure that the values
//               read are appropriate.
//

	std::string readResults (std::ifstream& r_fin);

//
//  calculatePosition
//
//  Purpose: To calculate the position of the second object for
//           the specified test.
//  Parameter(s):
//    <1> pos_min: The minimum test position for the solid
//    <2> pos_inc: The move increment for the solid
//    <3> move_x
//    <4> move_y
//    <5> move_z: The number of increments to move along the
//                Z-/Y-/Z-axis
//  Precondition(s):
//    <1> pos_inc.isAllComponentsNonZero()
//  Returns: The position of the second object.
//  Side Effect: N/A
//

	Vector3 calculatePosition (const Vector3& pos_min,
	                           const Vector3& pos_inc,
	                           int move_x,
	                           int move_y,
	                           int move_z);

//
//  updateResults
//
//  Purpose: To update the appropriate counter for the test
//           result values.
//  Parameter(s):
//    <1> is_detected: Whether a collision was detected
//    <2> is_read_from_file: Whetehr the results in the file
//                           indicate a collision
//    <3> r_results: A reference to the TestResults object to
//                   update
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The appropriate field of r_results is
//               incremented.
//

	void updateResults (bool is_detected,
	                    bool is_read_from_file,
	                    TestResults& results);

//
//  printTableHeader
//
//  Purpose: To display the header for the output table.  This
//           is all the lines at the top of the table above the
//           data, includig the table title.
//  Parameter(s):
//    <1> title: The title to print at the top of the table
//  Precondition(s):
//    <1> title != ""
//  Returns: N/A
//  Side Effect: The header for the output table is printed to
//               standard output.
//

	void printTableHeader (const std::string& title);

//
//  printTableRow
//
//  Purpose: To display a row of results in the output table.
//           This function also calculates the grade that would
//           be awarded based on this trial.
//  Parameter(s):
//    <1> trial: Which trial this is
//    <2> results: Counts of the test ressult accuracy
//  Precondition(s): N/A
//  Returns: The grade that would be assigned for this trial.
//           This is a value in the range [0.0, 1.0].
//  Side Effect: The data, including the grade, is printed to
//               the output table using standard output.
//

	double printTableRow (unsigned int trial,
	                      const TestResults& results);

//
//  printTableFooter
//
//  Purpose: To display the footer for the output table.  These
//           are the lines at the bottom of the table, including
//           the average grade.
//  Parameter(s):
//    <1> average: The average grade for the trials
//  Precondition(s):
//    <1> average >= 0.0
//    <2> average <= 1.0
//  Returns: N/A
//  Side Effect: The header for the output table is printed to
//               standard output.
//

	void printTableFooter (double average);

}	// end of namespace NumericTests



#endif
