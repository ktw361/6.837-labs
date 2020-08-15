#ifdef WIN32
#include <windows.h>
#endif

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>

#ifdef WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
#include <vecmath.h>

#include "modelerapp.h"
#include "ModelerView.h"

using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " PREFIX" << endl;
		cout << "For example, if you're trying to load data/cheb.skel, data/cheb.obj, and data/cheb.attach, run with: " 
            << argv[ 0 ] << " data/cheb" << endl;
		return -1;
	}

    // Initialize the controls.  You have to define a ModelerControl
    // for every variable name that you define in the enumeration.

    // The constructor for a ModelerControl takes the following arguments:
    // - text label in user interface
    // - minimum slider value
    // - maximum slider value
    // - step size for slider
    // - initial slider value

	const int NUM_JOINTS = 18;
    int numModels = argc - 1;

    ModelerControl *controls = new ModelerControl[numModels*NUM_JOINTS*3];
	string jointNames[NUM_JOINTS] ={ 
        "Root", "Chest", "Waist", "Neck", "Right hip", "Right leg", "Right knee", 
        "Right foot", "Left hip", "Left leg", "Left knee", "Left foot", "Right collarbone", 
        "Right shoulder", "Right elbow", "Left collarbone", "Left shoulder", "Left elbow" 
    };
    for (size_t i = 0; i < numModels; ++i) {
        for(size_t j = 0; j < NUM_JOINTS; ++j) {
            char buf[255];
            sprintf(buf, "m[%u] %s X", i, jointNames[j].c_str());
            controls[i*NUM_JOINTS*3 + j*3] = ModelerControl(buf, -M_PI, M_PI, 0.1f, 0);
            sprintf(buf, "m[%u] %s Y", i, jointNames[j].c_str());
            controls[i*NUM_JOINTS*3 + j*3+1] = ModelerControl(buf, -M_PI, M_PI, 0.1f, 0);
            sprintf(buf, "m[%u] %s Z", i, jointNames[j].c_str());
            controls[i*NUM_JOINTS*3 + j*3+2] = ModelerControl(buf, -M_PI, M_PI, 0.1f, 0);
        }
    }

    ModelerApplication::Instance()->Init
	(
		argc, argv,
		controls,
		numModels*NUM_JOINTS*3
	);

    // Run the modeler application.
    int ret = ModelerApplication::Instance()->Run();

    // This line is reached when you close the program.
    delete ModelerApplication::Instance();
    delete[] controls;

    return ret;
}
