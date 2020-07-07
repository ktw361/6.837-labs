#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "vecmath.h"
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
using std::size_t;
const size_t MAXCOLORS = 4;
const size_t MAX_BUFFER_SIZE = 64;
size_t color_selector = 0;
GLfloat light_pos_0 = 1.0f; 
GLfloat light_pos_1 = 1.0f;

// extra credits
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
/* GLfloat initialFoV = 45.0f; */
/* GLfloat speed = 3.0f; // 3 units / second */
/* GLfloat mouseSpeed = 0.005f; */
GLfloat camera_obj_dist;
GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

Vector3f camera_pos(0.0, 0.0, 5.0);
Vector3f looking_at(0.0, 0.0, 0.0);
Vector3f up_direction(0.0, 1.0, 0.0);

void display_modelview(GLfloat mat[16])
{
    for (int i = 0; i != 4; ++i) {
        for (int j = 0; j != 4; ++j)
            cout << mat[i*4+j] << " ";
        cout << endl;
    }
}

GLfloat get_dist(Vector3f v1, Vector3f v2) 
{
    Vector3f diff = v1 - v2;
    return sqrt(Vector3f::dot(diff, diff));
}


// Forward declarations
void drawScene(void);


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        color_selector = ++color_selector >= MAXCOLORS ? 0 : color_selector;
        cout << "Changing to color " << color_selector << "." << endl;        
        break;
    case 'd':
        // Debug mode
        camera_obj_dist = get_dist(looking_at, camera_pos);
        cout << camera_obj_dist << endl;
        rotate_x += 0.5;
        /* rotate_y += 0.5; */
        /* rotate_z += 0.5; */

        GLfloat matf[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, matf);
        display_modelview(matf);
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        light_pos_1 += 0.5;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        light_pos_1 -= 0.5;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        light_pos_0 -= 0.5;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        light_pos_0 += 0.5;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is called whenever a Mouse movement is received.
void mouseFunc( int button, int state, int x, int y)
{
    //The button parameter is one of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON.
    switch (button) {
    case 3: // scroll upward
        camera_pos = looking_at + 0.99 * (camera_pos - looking_at);
        break;
    case 4: // scroll downward
        camera_pos = looking_at + 1.01 * (camera_pos - looking_at);
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(camera_pos.x(), camera_pos.y(), camera_pos.z(),
              looking_at.x(), looking_at.y(), looking_at.z(),
              up_direction.x(), up_direction.y(), up_direction.z());

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[MAXCOLORS][4] = { {0.5, 0.5, 0.9, 1.0},
                                         {0.9, 0.5, 0.5, 1.0},
                                         {0.5, 0.9, 0.3, 1.0},
                                         {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[color_selector]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {light_pos_0, light_pos_1, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	// glutSolidTeapot(1.0);
    glBegin(GL_TRIANGLES);
    for (size_t ind = 0; ind < vecf.size(); ++ind) {
        unsigned a = vecf[ind][0], c = vecf[ind][2], d = vecf[ind][3],
                 f = vecf[ind][5], g = vecf[ind][6], i = vecf[ind][8];
        glNormal(vecn[c-1]);
        glVertex(vecv[a-1]);
        glNormal(vecn[f-1]);
        glVertex(vecv[d-1]);
        glNormal(vecn[i-1]);
        glVertex(vecv[g-1]);
    }
    glEnd();
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	// load the OBJ file here
    string buf;
    while(getline(cin, buf)) {
        stringstream ss(buf);
        string s;
        Vector3f v;
        ss >> s;
        if (s == "v") {
            ss >> v[0] >> v[1] >> v[2];
            vecv.push_back(v);
        } else if (s == "vn") {
            ss >> v[0] >> v[1] >> v[2];
            vecn.push_back(v);
        } else if (s == "f") {
            string token;
            vector<unsigned> f;
            for (size_t i = 0; i < 3; ++i) {
                ss >> token;
                stringstream ss2(token);
                for (size_t j = 0; j < 3; ++j) {
                    getline(ss2, token, '/');
                    f.push_back(atoi(token.c_str()));
                } // end for
            } // end for
            vecf.push_back(f);
        } // end if 
    }
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys
    glutMouseFunc(mouseFunc);       // Handles "mouse"

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
