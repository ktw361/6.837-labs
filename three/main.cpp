#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include <GL/glut.h>
#include <vecmath.h>
#include "camera.h"

///DONE: include more headers if necessary

#include "config.h"
#include "TimeStepper.hpp"
#include "simpleSystem.h"
#include "pendulumSystem.h"
#include "ClothSystem.h"

using namespace std;

// Globals here.
namespace
{
    class SystemCollections {
    public:
        SystemCollections(): cloth_ind(-1) {};
        void setup(int vis_index) {
            addSys(new SimpleSystem());
            addSys(new PendulumSystem(PENDSYS_NUM_PARTICLES, vis_index));
            addSys(new ClothSystem(HEIGHT, WIDTH), true);
        }
        void addSys(ParticleSystem *sys, bool is_cloth=false) { 
            if (is_cloth)
                cloth_ind = sys_list.size();
            sys_list.push_back(sys); 
        }
        void draw() const {
            for (size_t i = 0; i != sys_list.size(); ++i)
                sys_list[i]->draw();
        }
        ClothSystem * const getClothSys() const {
            if (!hasCloth()) return NULL;
            return dynamic_cast<ClothSystem*>(sys_list[cloth_ind]);
        }
        bool hasCloth() const {
            if (cloth_ind == -1) return false;
            else return true;
        }
        void sysStep(TimeStepper *stepper, float stepSize) {
            for (size_t i = 0; i != sys_list.size(); ++i)
                stepper->takeStep(sys_list[i], stepSize);
        }
        void clear() {
            for (size_t i = 0; i != sys_list.size(); ++i)
                delete sys_list[i];
            sys_list.clear();
        }
    private:
        vector<ParticleSystem*> sys_list;
        int cloth_ind;
    };

    SystemCollections sys_collections;
    TimeStepper * timeStepper;
    int vis_index = -1;
    float stepSize = 0.04f;
    // Cloth System
    bool render = true;
    bool wind = false;
    bool swing = false;

  // initialize your particle systems
  ///DONE: read argv here. set timestepper , step size etc
  void initSystem(int argc, char * argv[])
  {
    // seed the random number generator with the current time
    srand( time( NULL ) );
    if (argc == 1) {
        cout << "Use RK4 by default" << endl;
        return;
    }

    string method(argv[1]);
    if (method == "e") {
        cout << "Integrator: EULER" << endl;
        timeStepper = new ForwardEuler();
    }
    else if (method == "t") {
        cout << "Integrator: TRAPEZOIDAL" << endl;
        timeStepper = new Trapzoidal();
    }
    else if (method == "r") {
        cout << "Integrator: RK4" << endl;
        timeStepper = new RK4();
    }
    else if (method == "mr") {
        cout << "Integrator: MyRK4" << endl;
        timeStepper = new RK4();
    }
    else {
        cout << "Use RK4 by default" << endl;
        timeStepper = new RK4();
    }
    if (argc > 2) {
        stepSize = std::atof(argv[2]);
        cout << "Step size: " << stepSize << endl;

    }
    if (argc > 3) {
        vis_index = std::atoi(argv[3]);
        if (vis_index != -1)
            cout << "Visualize particle index: " << vis_index << endl;
    }

    sys_collections.setup(vis_index);
  }

  // Take a step forward for the particle shower
  ///DONE: Optional. modify this function to display various particle systems
  ///and switch between different timeSteppers
  void stepSystem()
  {
      ///DONE The stepsize should change according to commandline arguments
    if(timeStepper!=0){
        sys_collections.sysStep(timeStepper, stepSize);
    }
  }

  // Draw the current particle positions
  void drawSystem()
  {
    
    // Base material colors (they don't change)
    GLfloat particleColor[] = {0.4f, 0.7f, 1.0f, 1.0f};
    GLfloat floorColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, particleColor);
    
    glutSolidSphere(0.1f,10.0f,10.0f);
    
    sys_collections.draw();
    
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorColor);
    glPushMatrix();
    glTranslatef(0.0f,-5.0f,0.0f);
    glScaled(50.0f,0.01f,50.0f);
    glutSolidCube(1);
    glPopMatrix();
    
  }
        

    //-------------------------------------------------------------------
    
        
    // This is the camera
    Camera camera;

    // These are state variables for the UI
    bool g_mousePressed = false;

    // Declarations of functions whose implementations occur later.
    void arcballRotation(int endX, int endY);
    void keyboardFunc( unsigned char key, int x, int y);
    void specialFunc( int key, int x, int y );
    void mouseFunc(int button, int state, int x, int y);
    void motionFunc(int x, int y);
    void reshapeFunc(int w, int h);
    void drawScene(void);
    void initRendering();

    // This function is called whenever a "Normal" key press is
    // received.
    void keyboardFunc( unsigned char key, int x, int y )
    {
        switch ( key )
        {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
        {
            Matrix4f eye = Matrix4f::identity();
            camera.SetRotation( eye );
            camera.SetCenter( Vector3f::ZERO );
            break;
        }

        case 'r':
        {
            sys_collections.clear();
            sys_collections.setup(vis_index);
            break;
        }

        case 'w':
        {
            render = !render;
            if (sys_collections.hasCloth())
                sys_collections.getClothSys()->set_render(render);
            break;
        }

        case 'd':
        {
            wind = !wind;
            if (sys_collections.hasCloth())
                sys_collections.getClothSys()->set_wind(wind);
            if (wind) 
                cout << "wind on" << endl;
            else 
                cout << "wind off" << endl;
            break;
        }

        case 's':
        {
            swing = !swing;
            if (sys_collections.hasCloth())
                sys_collections.getClothSys()->set_swing(swing);
            if (swing) cout << "swing on" << endl;
            else cout << "swing off" << endl;
            break;
        }

        default:
            cout << "Unhandled key press " << key << "." << endl;        
        }

        glutPostRedisplay();
    }

    // This function is called whenever a "Special" key press is
    // received.  Right now, it's handling the arrow keys.
    void specialFunc( int key, int x, int y )
    {
        switch ( key )
        {

        }
        //glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {
        if (state == GLUT_DOWN)
        {
            g_mousePressed = true;
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                camera.MouseClick(Camera::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
            g_mousePressed = false;
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);        
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);

        camera.SetPerspective(50);
        glLoadMatrixf( camera.projectionMatrix() );
    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        glEnable(GL_NORMALIZE);

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

        // Clear to black
        glClearColor(0,0,0,1);
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();              

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        GLfloat Lt0pos[] = {3.0,3.0,5.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        glLoadMatrixf( camera.viewMatrix() );

        // THIS IS WHERE THE DRAW CODE GOES.

        drawSystem();

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if( g_mousePressed )
        {
            glPushMatrix();
            Vector3f eye = camera.GetCenter();
            glTranslatef( eye[0], eye[1], eye[2] );

            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3f(0,0,0); glVertex3f(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3f(0,0,0); glVertex3f(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3f(0,0,0); glVertex3f(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3f(0,0,0); glVertex3f(-1,0,0);
            glVertex3f(0,0,0); glVertex3f(0,-1,0);
            glVertex3f(0,0,0); glVertex3f(0,0,-1);

            glEnd();
            glPopMatrix();

            glPopAttrib();
            glPopMatrix();
        }
                 
        // Dump the image to the screen.
        glutSwapBuffers();
    }

    void timerFunc(int t)
    {
        stepSystem();

        glutPostRedisplay();

        glutTimerFunc(t, &timerFunc, t);
    }

    

    
    
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char* argv[] )
{
    glutInit( &argc, argv );

    // We're going to animate it, so double buffer 
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 600, 600 );
    
    camera.SetDimensions( 600, 600 );

    camera.SetDistance( 10 );
    camera.SetCenter( Vector3f::ZERO );
    
    glutCreateWindow("Assignment 4");

    // Initialize OpenGL parameters.
    initRendering();

    // Setup particle system
    initSystem(argc,argv);

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
    glutTimerFunc(20, timerFunc, 20);

        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return 0;	// This line is never reached.
}
