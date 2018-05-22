//
//  TestGeometricCollisions.cpp
//
//  A program to test collision detection between geometric
//    shapes.
//

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "GetGlut.h"

#include "Sleep.h"
#include "ObjLibrary/Vector2.h"
#include "ObjLibrary/Vector3.h"

#include "NumericTests.h"
#include "GeometricCollisions.h"

using namespace std;
namespace
{
	const int WINDOW_WIDTH_DEFAULT  = 800;
	const int WINDOW_HEIGHT_DEFAULT = 600;
	int g_window_width  = WINDOW_WIDTH_DEFAULT;
	int g_window_height = WINDOW_HEIGHT_DEFAULT;

	const double FRAME_SLEEP = 1.0 / 40.0;  // 40 fps

	const unsigned int TARGET_FPS = 40;
	double g_fps = TARGET_FPS;
	int g_next_frame_time;

	const int VIEW_BORDER_THICKNESS_HALF = 1;
	const int VIEW_SPACING_HALF          = 3;

	const double  WORLD_SIZE = 100.0;
	const Vector3 WORLD_SIZE3(WORLD_SIZE, WORLD_SIZE, WORLD_SIZE);

	const double  WORLD_DISPLAY_SIZE_MIN = WORLD_SIZE * 1.2;
	const double  EYE_FOV_MAX            = 60.0;
	const Vector3 EYE_POS(WORLD_SIZE * -0.6, WORLD_SIZE * -1.2, WORLD_SIZE * 0.8);



	struct WorldSolid
	{
		Vector3 m_center;
		bool m_is_intersection_sphere;
		bool m_is_intersection_cuboid;
	};

	struct WorldPoint : public WorldSolid
	{ };

	struct WorldSphere : public WorldSolid
	{
		double m_radius;
	};

	struct WorldCuboid : public WorldSolid
	{
		Vector3 m_size;
	};

	const unsigned int WORLD_POINT_COUNT = 40;
	WorldPoint ga_world_points[WORLD_POINT_COUNT];

	const unsigned int WORLD_SPHERE_COUNT  = 10;
	const double WORLD_SPHERE_RADIUS_MIN   = 5.0;
	const double WORLD_SPHERE_RADIUS_MAX   = 30.0;
	const double WORLD_SPHERE_RADIUS_RANGE = WORLD_SPHERE_RADIUS_MAX - WORLD_SPHERE_RADIUS_MIN;
	WorldSphere ga_world_spheres[WORLD_SPHERE_COUNT];

	const unsigned int WORLD_CUBOID_COUNT = 20;
	const Vector3 WORLD_CUBOID_SIZE_MIN( 5.0,  5.0,  5.0);
	const Vector3 WORLD_CUBOID_SIZE_MAX(30.0, 30.0, 30.0);
	WorldCuboid ga_world_cuboids[WORLD_CUBOID_COUNT];

	const Vector3 TEST_SPHERE_POSITION_DEFAULT = WORLD_SIZE3 *  0.5;
	const double  TEST_SPHERE_RADIUS_MIN       =  1.0;
	const double  TEST_SPHERE_RADIUS_MAX       = 50.0;
	const double  TEST_SPHERE_RADIUS_DEFAULT   = 25.0;
	const double  TEST_SPHERE_RADIUS_INCREMENT =  1.0;
	Vector3 g_test_sphere_position;
	Vector3 g_test_sphere_direction;
	double  g_test_sphere_radius;

	const Vector3 TEST_CUBOID_POSITION_DEFAULT = WORLD_SIZE3 * -0.5;
	const Vector3 TEST_CUBOID_SIZE(10.0, 25.0, 40.0);
	Vector3 g_test_cuboid_position;
	Vector3 g_test_cuboid_direction;

	const double TEST_OBJECT_SPEED = 1.0;  // per frame
	const bool IS_TEST_OBJECTS_MOVING_DEFAULT = true;
	bool g_is_test_objects_moving;

	const Vector3 COLOUR_VIEW_BORDER       (1.0, 1.0, 1.0);
	const Vector3 COLOUR_VIEW_BORDER_ACTIVE(1.0, 0.0, 0.0);
	const Vector3 COLOUR_WORLD_BOUNDS      (0.0, 0.0, 1.0);
	const Vector3 COLOUR_TEST_SPHERE       (1.0, 0.0, 0.0);
	const Vector3 COLOUR_TEST_CUBOID       (0.0, 1.0, 0.0);
	const Vector3 COLOUR_COLLIDE_NEITHER   (1.0, 1.0, 1.0);
	const Vector3 COLOUR_COLLIDE_SPHERE    (1.0, 0.0, 1.0);
	const Vector3 COLOUR_COLLIDE_CUBOID    (0.0, 1.0, 1.0);
	const Vector3 COLOUR_COLLIDE_BOTH      (1.0, 1.0, 0.0);

	const unsigned int TEST_MODE_COUNT   = 4;
	const unsigned int TEST_MODE_XY      = 0;
	const unsigned int TEST_MODE_XZ      = 1;
	const unsigned int TEST_MODE_YZ      = 2;
	const unsigned int TEST_MODE_FULL    = 3;
	const unsigned int TEST_MODE_DEFAULT = TEST_MODE_FULL;
	unsigned int g_test_mode;



	const bool DEBUGGING_ACTIVE_VIEW = false;

}  // end of anonymous namespace



int main (int argc, char* argv[]);
void initDisplay ();
void initWorld ();

void keyboard (unsigned char key, int x, int y);
void mouseClicked (int button, int state, int x, int y);

void idle ();
void moveTestObjects ();
void bounceTestObject (Vector3& position,
                       Vector3& direction,
                       const Vector3& size);
void checkCollisionsPoints ();
void checkCollisionsSpheres ();
void checkCollisionsCuboids ();

void reshape (int w, int h);
void display ();
void drawViewBorders ();
void setColourForViewBorder (unsigned int view_mode);
void drawView ();
void drawTestSphere ();
void drawWireCylinderZ (double radius, double half_height, unsigned int slices);
void drawTestCuboid ();
void drawWorldPoints ();
void drawWorldSpheres ();
void drawWorldCuboids ();
void setColourForSolid (const WorldSolid& solid);
unsigned int getSlicesForRadius (double radius);



int main (int argc, char* argv[])
{
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH_DEFAULT, WINDOW_HEIGHT_DEFAULT);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Geometric Collisions Program");

	glutKeyboardFunc(&keyboard);
	glutMouseFunc(&mouseClicked);
	glutIdleFunc(&idle);
	glutReshapeFunc(&reshape);
	glutDisplayFunc(&display);

	initDisplay();
	initWorld();

	glutMainLoop();

	return 1;
}

void initDisplay ()
{
	GLfloat a_fog_color[4] = {0.0, 0.0, 0.0, 1.0};

	glClearColor (0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0);

	glFogi(GL_FOG_MODE,   GL_LINEAR);
	glFogf(GL_FOG_START,  150.0);
	glFogf(GL_FOG_END,    400.0);
	glFogfv(GL_FOG_COLOR, a_fog_color);

	glutPostRedisplay();
}

void initWorld ()
{
	for(unsigned int i = 0; i < WORLD_POINT_COUNT; i++)
	{
		ga_world_points[i].m_center = Vector3::getRandomInRangeInclusive(-WORLD_SIZE3, WORLD_SIZE3);
		ga_world_points[i].m_is_intersection_sphere = false;
		ga_world_points[i].m_is_intersection_cuboid = false;
	}

	for(unsigned int i = 0; i < WORLD_SPHERE_COUNT; i++)
	{
		double radius = WORLD_SPHERE_RADIUS_MIN +
		                (rand() / (double)(RAND_MAX)) * WORLD_SPHERE_RADIUS_RANGE;
		double bound = WORLD_SIZE + radius;

		ga_world_spheres[i].m_center = Vector3::getRandomInRangeInclusive(Vector3(-bound, -bound, -bound),
		                                                                  Vector3( bound,  bound,  bound));
		ga_world_spheres[i].m_radius = radius;
		ga_world_spheres[i].m_is_intersection_sphere = false;
		ga_world_spheres[i].m_is_intersection_cuboid = false;
	}

	for(unsigned int i = 0; i < WORLD_CUBOID_COUNT; i++)
	{
		Vector3 size = Vector3::getRandomInRangeInclusive(WORLD_CUBOID_SIZE_MIN,
		                                                  WORLD_CUBOID_SIZE_MAX);

		ga_world_cuboids[i].m_center = Vector3::getRandomInRangeInclusive(-WORLD_SIZE3 - size,
		                                                                   WORLD_SIZE3 + size);
		ga_world_cuboids[i].m_size   = size;
		ga_world_cuboids[i].m_is_intersection_sphere = false;
		ga_world_cuboids[i].m_is_intersection_cuboid = false;
	}

	g_test_sphere_position  = TEST_SPHERE_POSITION_DEFAULT;
	g_test_sphere_direction = Vector3::getRandomUnitVector();
	g_test_sphere_radius    = TEST_SPHERE_RADIUS_DEFAULT;

	g_test_cuboid_position  = TEST_CUBOID_POSITION_DEFAULT;
	g_test_cuboid_direction = Vector3::getRandomUnitVector();

	g_is_test_objects_moving = IS_TEST_OBJECTS_MOVING_DEFAULT;

	g_test_mode = TEST_MODE_DEFAULT;
}



void keyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	case ' ':
		g_is_test_objects_moving = !g_is_test_objects_moving;
		break;
	case '+':
	case '=':
		g_test_sphere_radius += TEST_SPHERE_RADIUS_INCREMENT;
		if(g_test_sphere_radius > TEST_SPHERE_RADIUS_MAX)
			g_test_sphere_radius = TEST_SPHERE_RADIUS_MAX;
		assert(g_test_sphere_radius >= TEST_SPHERE_RADIUS_MIN);
		assert(g_test_sphere_radius <= TEST_SPHERE_RADIUS_MAX);
		break;
	case '-':
	case '_':
		g_test_sphere_radius -= TEST_SPHERE_RADIUS_INCREMENT;
		if(g_test_sphere_radius < TEST_SPHERE_RADIUS_MIN)
			g_test_sphere_radius = TEST_SPHERE_RADIUS_MIN;
		assert(g_test_sphere_radius >= TEST_SPHERE_RADIUS_MIN);
		assert(g_test_sphere_radius <= TEST_SPHERE_RADIUS_MAX);
		break;
	case 'r':
	case 'R':
		initWorld();
		break;
	case 't':
	case 'T':
		NumericTests::run("numeric_tests.txt");
		break;
	}
}

void mouseClicked (int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		if(DEBUGGING_ACTIVE_VIEW)
		{
			cout << "Clicked (" << x << " / " << g_window_width
			     <<        ", " << y << " / " << g_window_height << ")" << endl;
		}

		if(x < g_window_width / 2)
		{
			if(y < g_window_height / 2)
				g_test_mode = TEST_MODE_XY;
			else
				g_test_mode = TEST_MODE_XZ;
		}
		else
		{
			if(y < g_window_height / 2)
				g_test_mode = TEST_MODE_FULL;
			else
				g_test_mode = TEST_MODE_YZ;
		}

		if(DEBUGGING_ACTIVE_VIEW)
			cout << "\tTest mode: " << g_test_mode << endl;
	}
}



void idle ()
{
	if(g_is_test_objects_moving)
		moveTestObjects();
	checkCollisionsPoints();
	checkCollisionsSpheres();
	checkCollisionsCuboids();

	sleep(FRAME_SLEEP);
	glutPostRedisplay();
}

void moveTestObjects ()
{
	g_test_sphere_position += g_test_sphere_direction * TEST_OBJECT_SPEED;
	bounceTestObject(g_test_sphere_position,
	                 g_test_sphere_direction,
	                 Vector3(g_test_sphere_radius, g_test_sphere_radius, g_test_sphere_radius));

	g_test_cuboid_position += g_test_cuboid_direction * TEST_OBJECT_SPEED;
	bounceTestObject(g_test_cuboid_position,
	                 g_test_cuboid_direction,
	                 TEST_CUBOID_SIZE);
}

void bounceTestObject (Vector3& r_position,
                       Vector3& r_direction,
                       const Vector3& size)
{
	Vector3 world_edge_padded = WORLD_SIZE3 - size;

	if(r_position.x < -world_edge_padded.x)
	{
		double over   = -world_edge_padded.x - r_position.x;
		r_position.x  = -world_edge_padded.x + over;
		r_direction.x =  fabs(r_direction.x);
	}
	if(r_position.x > world_edge_padded.x)
	{
		double over   =  r_position.x - world_edge_padded.x;
		r_position.x  =  world_edge_padded.x - over;
		r_direction.x = -fabs(r_direction.x);
	}

	if(r_position.y < -world_edge_padded.y)
	{
		double over   = -world_edge_padded.y - r_position.y;
		r_position.y  = -world_edge_padded.y + over;
		r_direction.y =  fabs(r_direction.y);
	}
	if(r_position.y > world_edge_padded.y)
	{
		double over   =  r_position.y - world_edge_padded.y;
		r_position.y  =  world_edge_padded.y - over;
		r_direction.y = -fabs(r_direction.y);
	}

	if(r_position.z < -world_edge_padded.z)
	{
		double over   = -world_edge_padded.z - r_position.z;
		r_position.z  = -world_edge_padded.z + over;
		r_direction.z =  fabs(r_direction.z);
	}
	if(r_position.z > world_edge_padded.z)
	{
		double over   =  r_position.z - world_edge_padded.z;
		r_position.z  =  world_edge_padded.z - over;
		r_direction.z = -fabs(r_direction.z);
	}
}

void checkCollisionsPoints ()
{

	for(unsigned int i = 0; i < WORLD_POINT_COUNT; i++)
	{
		WorldPoint& r_point = ga_world_points[i];
		Vector3 point_center = r_point.m_center;

		switch(g_test_mode)
		{
		case TEST_MODE_XY:  point_center.z = g_test_sphere_position.z;  break;
		case TEST_MODE_XZ:  point_center.y = g_test_sphere_position.y;  break;
		case TEST_MODE_YZ:  point_center.x = g_test_sphere_position.x;  break;
		}

		r_point.m_is_intersection_sphere = GeometricCollisions::pointVsSphere(point_center,
		                                                                      g_test_sphere_position,
		                                                                      g_test_sphere_radius);

		switch(g_test_mode)
		{
		case TEST_MODE_XY:  point_center.z = g_test_cuboid_position.z;  break;
		case TEST_MODE_XZ:  point_center.y = g_test_cuboid_position.y;  break;
		case TEST_MODE_YZ:  point_center.x = g_test_cuboid_position.x;  break;
		}

		r_point.m_is_intersection_cuboid = GeometricCollisions::pointVsCuboid(point_center,
		                                                                      g_test_cuboid_position,
		                                                                      TEST_CUBOID_SIZE);
	}
}

void checkCollisionsSpheres ()
{
	for(unsigned int i = 0; i < WORLD_SPHERE_COUNT; i++)
	{
		WorldSphere& r_sphere = ga_world_spheres[i];
		Vector3 sphere_center = r_sphere.m_center;

		switch(g_test_mode)
		{
		case TEST_MODE_XY:  sphere_center.z = g_test_sphere_position.z;  break;
		case TEST_MODE_XZ:  sphere_center.y = g_test_sphere_position.y;  break;
		case TEST_MODE_YZ:  sphere_center.x = g_test_sphere_position.x;  break;
		}

		r_sphere.m_is_intersection_sphere = GeometricCollisions::sphereVsSphere(sphere_center,
		                                                                        r_sphere.m_radius,
		                                                                        g_test_sphere_position,
		                                                                        g_test_sphere_radius);

		switch(g_test_mode)
		{
		case TEST_MODE_XY:  sphere_center.z = g_test_cuboid_position.z;  break;
		case TEST_MODE_XZ:  sphere_center.y = g_test_cuboid_position.y;  break;
		case TEST_MODE_YZ:  sphere_center.x = g_test_cuboid_position.x;  break;
		}

		r_sphere.m_is_intersection_cuboid = GeometricCollisions::sphereVsCuboid(sphere_center,
		                                                                        r_sphere.m_radius,
		                                                                        g_test_cuboid_position,
		                                                                        TEST_CUBOID_SIZE);
	}
}

void checkCollisionsCuboids ()
{
	for(unsigned int i = 0; i < WORLD_CUBOID_COUNT; i++)
	{
		WorldCuboid& r_cuboid = ga_world_cuboids[i];
		Vector3 cuboid_center = r_cuboid.m_center;

		switch(g_test_mode)
		{
		case TEST_MODE_XY:  cuboid_center.z = g_test_sphere_position.z;  break;
		case TEST_MODE_XZ:  cuboid_center.y = g_test_sphere_position.y;  break;
		case TEST_MODE_YZ:  cuboid_center.x = g_test_sphere_position.x;  break;
		}

		r_cuboid.m_is_intersection_sphere = GeometricCollisions::sphereVsCuboid(g_test_sphere_position,
		                                                                        g_test_sphere_radius,
		                                                                        cuboid_center,
		                                                                        r_cuboid.m_size);

		switch(g_test_mode)
		{
		case TEST_MODE_XY:  cuboid_center.z = g_test_cuboid_position.z;  break;
		case TEST_MODE_XZ:  cuboid_center.y = g_test_cuboid_position.y;  break;
		case TEST_MODE_YZ:  cuboid_center.x = g_test_cuboid_position.x;  break;
		}

		r_cuboid.m_is_intersection_cuboid = GeometricCollisions::cuboidVsCuboid(cuboid_center,
		                                                                        r_cuboid.m_size,
		                                                                        g_test_cuboid_position,
		                                                                        TEST_CUBOID_SIZE);
	}
}



void reshape (int w, int h)
{
	glViewport (0, 0, w, h);

	g_window_width  = w;
	g_window_height = h;

	// matrixes are reset each frame, so we don't set them here

	glutPostRedisplay();
}

void display ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//
	//  Step 1: Set up view positioning values and general options
	//

	Vector2 window_size(g_window_width, g_window_height);
	double  view_aspect_ratio = window_size.x / window_size.y;

	Vector2 view_size   = window_size * 0.5 - Vector2(VIEW_SPACING_HALF, VIEW_SPACING_HALF);
	Vector2 view_offset = window_size - view_size;
	Vector2 view_scale  = view_size.getComponentRatio(window_size);

	Vector2 view_offset_fraction = view_offset.getComponentRatio(window_size);
	double  view_size_min        = (view_size.x < view_size.y) ? view_size.x : view_size.y;
	Vector2 view_size_fraction   = view_size / view_size_min;
	double  view_scale_min       = (view_scale.x < view_scale.y) ? view_scale.x : view_scale.y;

	glEnable(GL_SCISSOR_TEST);

	//
	//  Step 2: Draw 3 orthogonal views
	//

	Vector2 world_display_size(WORLD_DISPLAY_SIZE_MIN, WORLD_DISPLAY_SIZE_MIN);
	if(g_window_width > g_window_height)
		world_display_size.x *= view_aspect_ratio;
	else
		world_display_size.y /= view_aspect_ratio;
	Vector2 world_offset = world_display_size * 0.5;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-world_display_size.x, world_display_size.x,
	        -world_display_size.y, world_display_size.y,
	        -1000.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw bottom left view - XZ
	glPushMatrix();
		// position camera
		glRotated(90.0, -1.0, 0.0, 0.0);

		// move view to corner
		glTranslated(-world_offset.x, 0.0, -world_offset.y);
		glScaled(view_scale.x, 1.0, view_scale.y);

		// only draw to part of the screen
		glScissor(0, 0, view_size.x, view_size.y);

		drawView();
	glPopMatrix();

	// draw top left view - XY
	glPushMatrix();
		// move view to corner
		glTranslated(-world_offset.x, world_offset.y, 0.0);
		glScaled(view_scale.x, view_scale.y, 1.0);

		// only draw to part of the screen
		glScissor(0, view_offset.y, view_size.x, view_size.y);

		drawView();
	glPopMatrix();

	// draw bottom left view - YZ
	glPushMatrix();
		// position camera
		glRotated(90.0, -1.0, 0.0, 0.0);
		glRotated(90.0,  0.0, 0.0, 1.0);

		// move view to corner
		glTranslated(0.0, -world_offset.x, -world_offset.y);
		glScaled(1.0, view_scale.x, view_scale.y);

		// only draw to part of the screen
		glScissor(view_offset.x, 0, view_size.x, view_size.y);

		drawView();
	glPopMatrix();

	//
	//  Step 3: Draw the perspective view
	//

	glEnable(GL_FOG);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslated(view_offset_fraction.x, view_offset_fraction.y, 0.0);

	glFrustum(-view_size_fraction.x, view_size_fraction.x,
	          -view_size_fraction.y, view_size_fraction.y,
	          1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(EYE_POS.x, EYE_POS.y, EYE_POS.z,
	          0.0,       0.0,       0.0,
	          0.0,       0.0,       1.0);

	glScaled(view_scale_min, view_scale_min, view_scale_min);

	// only draw to part of the screen
	glScissor(view_offset.x, view_offset.y, view_size.x, view_size.y);

	drawView();

	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_FOG);

	//
	//  Step 3: Draw view outlines
	//

	glDisable(GL_SCISSOR_TEST);
	drawViewBorders();

	glutSwapBuffers();
}

void drawViewBorders ()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, g_window_width, 0, g_window_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(VIEW_BORDER_THICKNESS_HALF * 2.0);
	glColor3d(COLOUR_VIEW_BORDER.x, COLOUR_VIEW_BORDER.y, COLOUR_VIEW_BORDER.z);

	if(DEBUGGING_ACTIVE_VIEW)
		cout << "Test mode: " << g_test_mode << endl;
	for(unsigned int y = 0; y <= 1; y++)
	{
		int y_half = g_window_height / 2;
		int y_outer = (y == 0) ? (                  VIEW_BORDER_THICKNESS_HALF)
		                       : (g_window_height - VIEW_BORDER_THICKNESS_HALF);
		int y_inner = (y == 0) ? (y_half          - VIEW_BORDER_THICKNESS_HALF - VIEW_SPACING_HALF)
		                       : (y_half          + VIEW_BORDER_THICKNESS_HALF + VIEW_SPACING_HALF);

		for(unsigned int x = 0; x <= 1; x++)
		{
			int x_half = g_window_width / 2;
			int x_outer = (x == 0) ? (                 VIEW_BORDER_THICKNESS_HALF)
			                       : (g_window_width - VIEW_BORDER_THICKNESS_HALF);
			int x_inner = (x == 0) ? (x_half         - VIEW_BORDER_THICKNESS_HALF - VIEW_SPACING_HALF)
			                       : (x_half         + VIEW_BORDER_THICKNESS_HALF + VIEW_SPACING_HALF);

			unsigned int view_mode = (y == 1) ? ((x == 0) ? (TEST_MODE_XY) : (TEST_MODE_FULL))
			                                  : ((x == 0) ? (TEST_MODE_XZ) : (TEST_MODE_YZ));
			if(DEBUGGING_ACTIVE_VIEW)
			{
				cout << "Drawing view at (" << x << ", " << y
				     <<          ")\t=>  (" << x_outer << ", " << y_outer << ")" << endl;
				cout << "\tView mode: " <<   view_mode << endl;
			}
			setColourForViewBorder(view_mode);

			glBegin(GL_LINE_LOOP);
				glVertex2i(x_outer, y_outer);
				glVertex2i(x_outer, y_inner);
				glVertex2i(x_inner, y_inner);
				glVertex2i(x_inner, y_outer);
			glEnd();
		}
	}
}

void setColourForViewBorder (unsigned int view_mode)
{
	if(view_mode == g_test_mode)
		glColor3d(COLOUR_VIEW_BORDER_ACTIVE.x, COLOUR_VIEW_BORDER_ACTIVE.y, COLOUR_VIEW_BORDER_ACTIVE.z);
	else
		glColor3d(COLOUR_VIEW_BORDER.x,        COLOUR_VIEW_BORDER.y,        COLOUR_VIEW_BORDER.z);
}

void drawView ()
{
	// draw bounds cube
	glLineWidth(2.0);
	glColor3d(COLOUR_WORLD_BOUNDS.x, COLOUR_WORLD_BOUNDS.y, COLOUR_WORLD_BOUNDS.z);
	glutWireCube(WORLD_SIZE * 2.0);

	// draw test objects
	glLineWidth(2.0);
	drawTestSphere();
	drawTestCuboid();

	// draw world objects
	glLineWidth(1.0);
	drawWorldPoints();
	drawWorldSpheres();
	drawWorldCuboids();
}

void drawTestSphere ()
{
	unsigned int slice_count = getSlicesForRadius(g_test_sphere_radius);

	glPushMatrix();
		glColor3d(COLOUR_TEST_SPHERE.x, COLOUR_TEST_SPHERE.y, COLOUR_TEST_SPHERE.z);
		switch(g_test_mode)
		{
		case TEST_MODE_FULL:
			glTranslated(g_test_sphere_position.x, g_test_sphere_position.y, g_test_sphere_position.z);
			glutWireSphere(g_test_sphere_radius, slice_count, (slice_count * 3) / 4);
			break;
		case TEST_MODE_XY:
			glTranslated(g_test_sphere_position.x, g_test_sphere_position.y, 0.0);
			drawWireCylinderZ(g_test_sphere_radius, WORLD_SIZE, slice_count);
			break;
		case TEST_MODE_XZ:
			glTranslated(g_test_sphere_position.x, 0.0, g_test_sphere_position.z);
			glRotated(90.0, 1.0, 0.0, 0.0);
			drawWireCylinderZ(g_test_sphere_radius, WORLD_SIZE, slice_count);
			break;
		case TEST_MODE_YZ:
			glTranslated(0.0, g_test_sphere_position.y, g_test_sphere_position.z);
			glRotated(90.0, 0.0, 1.0, 0.0);
			drawWireCylinderZ(g_test_sphere_radius, WORLD_SIZE, slice_count);
			break;
	}
	glPopMatrix();
}

void drawWireCylinderZ (double radius, double half_height, unsigned int slices)
{
	assert(radius >= 0.0);
	assert(half_height >= 0.0);
	assert(slices >= 3);

	static const double TWO_PI = 6.283185307179586476925286766559;

	Vector2* d_position_xy = new Vector2[slices];
	for(unsigned int i = 0; i < slices; i++)
	{
		double radians = i * TWO_PI / slices;
		d_position_xy[i].set(cos(radians) * radius,
		                     sin(radians) * radius);
	}

	// draw top
	glBegin(GL_LINE_LOOP);
		for(unsigned int i = 0; i < slices; i++)
			glVertex3d(d_position_xy[i].x, d_position_xy[i].y, half_height);
	glEnd();

	if(half_height > 0.0)
	{
		// draw sides
		glBegin(GL_LINES);
			for(unsigned int i = 0; i < slices; i++)
			{
				glVertex3d(d_position_xy[i].x, d_position_xy[i].y,  half_height);
				glVertex3d(d_position_xy[i].x, d_position_xy[i].y, -half_height);
			}
		glEnd();

		// draw bottom
		glBegin(GL_LINE_LOOP);
			for(unsigned int i = 0; i < slices; i++)
				glVertex3d(d_position_xy[i].x, d_position_xy[i].y, -half_height);
		glEnd();
	}

	delete[] d_position_xy;
}

void drawTestCuboid ()
{
	Vector3 test_cuboid_center = g_test_cuboid_position;
	Vector3 test_cuboid_size   = TEST_CUBOID_SIZE;

	switch(g_test_mode)
	{
	case TEST_MODE_XY:  test_cuboid_center.z = 0.0;  test_cuboid_size.z = WORLD_SIZE;  break;
	case TEST_MODE_XZ:  test_cuboid_center.y = 0.0;  test_cuboid_size.y = WORLD_SIZE;  break;
	case TEST_MODE_YZ:  test_cuboid_center.x = 0.0;  test_cuboid_size.x = WORLD_SIZE;  break;
	}

	glPushMatrix();
		glTranslated(test_cuboid_center.x, test_cuboid_center.y, test_cuboid_center.z);
		glScaled(test_cuboid_size.x, test_cuboid_size.y, test_cuboid_size.z);

		glColor3d(COLOUR_TEST_CUBOID.x, COLOUR_TEST_CUBOID.y, COLOUR_TEST_CUBOID.z);
		glutWireCube(2.0);
	glPopMatrix();
}

void drawWorldPoints ()
{
	glBegin(GL_POINTS);
		for(unsigned int i = 0; i < WORLD_POINT_COUNT; i++)
		{
			const WorldPoint& point = ga_world_points[i];
			setColourForSolid(point);
			glVertex3d(point.m_center.x, point.m_center.y, point.m_center.z);
		}
	glEnd();
}

void drawWorldSpheres ()
{
	for(unsigned int i = 0; i < WORLD_SPHERE_COUNT; i++)
	{
		const WorldSphere& sphere = ga_world_spheres[i];
		unsigned int slice_count = getSlicesForRadius(sphere.m_radius);

		glPushMatrix();
			glTranslated(sphere.m_center.x, sphere.m_center.y, sphere.m_center.z);

			setColourForSolid(sphere);
			glutWireSphere(sphere.m_radius, slice_count, (slice_count * 3) / 4);
		glPopMatrix();
	}
}

void drawWorldCuboids ()
{
	for(unsigned int i = 0; i < WORLD_CUBOID_COUNT; i++)
	{
		const WorldCuboid& cuboid = ga_world_cuboids[i];

		glPushMatrix();
			glTranslated(cuboid.m_center.x, cuboid.m_center.y, cuboid.m_center.z);
			glScaled    (cuboid.m_size.x,   cuboid.m_size.y,   cuboid.m_size.z);

			setColourForSolid(cuboid);
			glutWireCube(2.0);
		glPopMatrix();
	}
}

void setColourForSolid (const WorldSolid& solid)
{
	if(solid.m_is_intersection_cuboid)
	{
		if(solid.m_is_intersection_sphere)
			glColor3d(COLOUR_COLLIDE_BOTH.x,    COLOUR_COLLIDE_BOTH.y,    COLOUR_COLLIDE_BOTH.z);
		else
			glColor3d(COLOUR_COLLIDE_CUBOID.x,  COLOUR_COLLIDE_CUBOID.y,  COLOUR_COLLIDE_CUBOID.z);
	}
	else
	{
		if(solid.m_is_intersection_sphere)
			glColor3d(COLOUR_COLLIDE_SPHERE.x,  COLOUR_COLLIDE_SPHERE.y,  COLOUR_COLLIDE_SPHERE.z);
		else
			glColor3d(COLOUR_COLLIDE_NEITHER.x, COLOUR_COLLIDE_NEITHER.y, COLOUR_COLLIDE_NEITHER.z);
	}
}

unsigned int getSlicesForRadius (double radius)
{
	unsigned int slice_count = (unsigned int)(radius * 0.6);
	if(slice_count < 8)
		return 8;
	else
		return (slice_count / 2) * 2; // always even
}
