#include <ctime>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <cmath>
#include <SDL/SDL.h>

#include "Chroma.h"
#include "Scene.h"


//*******************

bool stop = false;
bool finished = false;
bool pupilMode = false;
int method = 1; // numbered by F-keys
int camType = 1;

/*SDL, CAMERA AND CONTROL*/
int xres = 384;
int yres = 288;
bool mirrorX = false;
bool mirrorY = false;
float sensitivity = 1.0f;
float fl = 0.0f; //mm!
int stopChange = 0;

Scene *scene = 0;

std::string sceneName = "";
std::string lensName = "";

/**
 * pathDepth is equivalent to nr of scene interactions
 * 0: direct light from sources only!
 * INT_MAX: pure Russian Roulette
 * 100: fair compromise, but biased...
 **/

int pathDepth = 100;
int heuristic = 2;
bool spectral = true;

//Vector3 movement(0,0,0);
//bool mouseLeft = false;
//bool keysHeld[323] = {false};
///*DEBUG VARS*/
//int dbg = 0;
//int param = 4;
//char * title = new char[TEXTOUTPUTLEN];
//char * titlePart = new  char[30];
//char * text1 = new char[TEXTOUTPUTLEN];
//char * text2 = new char[TEXTOUTPUTLEN];
//char * text3 = new char[TEXTOUTPUTLEN];
//char * text4 = new char[TEXTOUTPUTLEN];
//char * text5 = new char[TEXTOUTPUTLEN];
//char * logLine = new char[256];
//long t0,t;
//time_t start, last, actual;
//int frames = 0;
//float fps = 0.0f;
//int saveLimit=1000;
//int pupilLimit = 250;
//int textSwitch = 1;
//int debug = 0;
//float lensTracePercent = 0.0f;
//float shadingPercent = 0.0f;
//float traversalPercent = 0.0f;


void parseArgs(int argc, char **argv) {
    int c;
    while ((c = getopt(argc, argv, "pc:x:y:m:b:d:s:r:f:l:t:S:")) != -1) {
        switch (c) {

            case 'c':
                camType = atoi(optarg);
                break;
            case 'p':
                pupilMode = true;
                break;
            case 'x':
                xres = atoi(optarg);
                break;
            case 'y':
                yres = atoi(optarg);
                break;
            case 'm':
                method = atoi(optarg);
                break;
            case 'b':
                heuristic = atoi(optarg);
                break;
            case 'd':
                pathDepth = atoi(optarg);
                break;
            case 's':
                sceneName = std::string(strndup(optarg, NAMELENGTH));
                break;
            case 'S':
                stopChange = atoi(optarg);
                break;
            case 'l':
                lensName = std::string(strndup(optarg, NAMELENGTH));
                break;
            case 'r':
                sensitivity = atof(optarg);
                break;
            case 'f':
                fl = atof(optarg);
                break;
            default:
                printf("unknown option %c", c);
                break;
        }
    }

    if (optind < argc) {
        std::cerr << "unknown option " << argv[optind] << std::endl;
        abort();
    }
}


int main(int argc, char **argv) {

    /*read input*/
    //sceneName = "plq";
    sceneName = "cornelldiamond";
    //sceneName = "chess";
    //sceneName = "dispersionprism";
    //sceneName = "conference";
    //sceneName = "chess";

    //lensName = "kimura_wideangle";
    //lensName = "mitsuaki_fisheye";
    //lensName = "miyamoto_fisheye";
    //lensName = "muller_fisheye";
    lensName = "tessar_brendel";
    //lensName = "momiyama_tele";
    //lensName = "momiyama_wideangle";
    //lensName = "mori_wideangle";
    //lensName = "pinhole";
    //lensName = "petzval";
    //lensName = "single_frontAP";
    //lensName = "rosier_gauss";

    xres = 512;
    yres = 384;
    camType = 2;
    method = 1;
    //fl=10.0f;
    //pathDepth=0;
    //pupilMode=true;

    parseArgs(argc, argv);

    /*Scene Setup*/
    scene = new Scene(sceneName);
    scene->setupAccStruct(heuristic, 0);

    /*Kernel Setup*/
    // TODO

    /*SDL Setup*/
    // TODO

    /*Main loop*/
    // TODO

    return 0;
}
