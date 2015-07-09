#include <ctime>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <cmath>
#include <SDLRenderCanvas.h>
#include <SDL/SDL.h>
#include <Sampler.h>
#include "Chroma.h"
#include "Scene.h"


//*******************

Image* sdlImage;
bool stop = false;
bool closeChroma = false;
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


Vector3 movement(0, 0, 0);
bool mouseLeft = false;
bool keysHeld[323] = {false};


void SDLRenderCanvas::HandleEvents() {

    bool consumed = false;

    while (SDL_PollEvent(&m_Event)) {

        for (SDLEventConsumerVector::iterator i = m_EventConsumer.begin(); i != m_EventConsumer.end(); i++) {
            consumed = (*i)->Consume(m_Event);
            if (consumed) return;// At the moment do not handle consumed events any further
        }

        switch (m_Event.type) {

            case SDL_QUIT:
                closeChroma = true;
                stop = true;
                break;

            case SDL_KEYUP:
                switch (m_Event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        stop = true;
                        closeChroma = true;
                        break;
                    case SDLK_UP:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_DOWN:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_PAGEDOWN:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_PAGEUP:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_LEFT:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_RIGHT:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_LSHIFT:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_LCTRL:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_PLUS:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                    case SDLK_MINUS:
                        keysHeld[m_Event.key.keysym.sym] = false;
                        break;
                        //case SDLK_c: cout << cam->pos << " | " << cam->rot[0] << " | " << cam->rot[1] << " | " << cam->rot[2] << endl; break;

                    case SDLK_F10:
                        //hdrScreenshot(*image);
                        break;

                    case SDLK_F11:
                        char filename[32];
                        //snprintf(filename,32,"sensor%d.tga",sensor->acc);
                        //image->saveTonemapped(filename);
                        std::cout << "LDR screeshot saved to " << filename << std::endl;
                        break;


                    case SDLK_F1:
                        //switchRender(1);
                        break;
                    case SDLK_F2:
                        //switchRender(2);
                        break;
                    case SDLK_F3:
                        //switchRender(3);
                        break;
                    case SDLK_F4:
                        //switchRender(4);
                        break;
                    case SDLK_F5:
                        //switchCam(1);
                        break;
                    case SDLK_F6:
                        //switchCam(2);
                        break;
//                    case SDLK_F7:
//                        if (pupilMode) {
//                            method = 9;
//                            sensor->clear();
//                        }
//                        break;

//                    case SDLK_KP_MINUS:
//                        cam->superSample(0.5f);
//                        sensor->clear();
//                        break;
//                    case SDLK_KP_PLUS:
//                        cam->superSample(2.0f);
//                        sensor->clear();
//                        break;
                    default:
                        break;
                }

                break;

            case SDL_KEYDOWN:
                switch (m_Event.key.keysym.sym) {
                    case SDLK_UP:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_DOWN:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_LEFT:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_LSHIFT:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_LCTRL:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_RIGHT:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_PAGEDOWN:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_PAGEUP:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_PLUS:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
                    case SDLK_MINUS:
                        keysHeld[m_Event.key.keysym.sym] = true;
                        break;
//                    case SDLK_SPACE:
//                        SDL_WarpMouse(xres / 2, yres / 2);
//                        cam->reset();
//                        sensor->clear();
//                        break;

                    case SDLK_y:
                        mirrorY = !mirrorY;
                        break;
                    case SDLK_x:
                        mirrorX = !mirrorX;
                        break;
//                    case SDLK_p:
//                        textSwitch = 2;
//                        flushText();
//                        break;
//                    case SDLK_o:
//                        textSwitch = 1;
//                        flushText();
//                        break;
//                    case SDLK_l:
//                        textSwitch = 3;
//                        flushText();
//                        break;
//                    case SDLK_q:
//                        cam->stopUp();
//                        sensor->clear();
//                        break;
//                    case SDLK_a:
//                        cam->stopDown();
//                        sensor->clear();
//                        break;
//                    case SDLK_w:
//                        if (keysHeld[SDLK_LSHIFT]) cam->sensorZPos += 0.1f;
//                        else cam->sensorZPos += 0.5f;
//                        sensor->clear();
//                        break;
//                    case SDLK_s:
//                        if (keysHeld[SDLK_LSHIFT]) cam->sensorZPos -= 0.1f;
//                        else cam->sensorZPos -= 0.5f;
//                        sensor->clear();
//                        break;
//                    case SDLK_e:
//                        cam->sensitivity *= 2.0f;
//                        sensor->clear();
//                        break;
//                    case SDLK_d:
//                        if (cam->sensitivity > EPS * 100.0f) cam->sensitivity *= 0.5f;
//                        sensor->clear();
//                        break;
//                    case SDLK_r:
//                        cam->zoomRel(2.0f);
//                        sensor->clear();
//                        break;
//                    case SDLK_f:
//                        cam->zoomRel(0.5f);
//                        sensor->clear();
//                        break;

//                    case SDLK_F8:
//                        if (pupilMode) {
//                            pupilMode = false;
//                            realCam->switchToLensMode();
//                        }
//                        else {
//                            pupilMode = true;
//                            realCam->switchToPupilMode();
//                        }
//                        break;
//                    case SDLK_F9: {
//                        autofocus();
//                        break;
//                    }

                    default:
                        break;
                }
                break;

//            case SDL_MOUSEMOTION:
//                if (mouseLeft) {
//                    sensor->flush();
//                    int diff_x = -m_Event.motion.xrel;
//                    int diff_y = -m_Event.motion.yrel;
//                    diff_x = abs(diff_x) > 30 ? 30 * (abs(diff_x) / diff_x) : diff_x; //clamp
//                    diff_y = abs(diff_y) > 30 ? 30 * (abs(diff_y) / diff_y) : diff_y; //clamp
//                    cam->rotate(diff_x, diff_y);
//                }
//                break;

//            case SDL_MOUSEBUTTONDOWN:
//                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
//                    mouseLeft = true;
//                }
//
//
//                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3)) {
//
//                    if (keysHeld[SDLK_LSHIFT]) {
//
//                        int x = ((int) m_Event.motion.x);
//                        int y = yres - ((int) m_Event.motion.y);
//                        Ray ray;
//
//
//                        if (cam->getDebugRay(x, y, ray) > -1) {
//                            float SPEresult;
//                            ray.lambda = 555;
//
//                            switch (method) {
//                                case 2:
//                                    rTornado->pathtrace(ray, SPEresult, pathDepth, globalEnv);
//                                    break;
//                                case 3:
//                                    rTornado->pathtraceDL(ray, SPEresult, pathDepth, globalEnv);
//                                    break;
//                                case 4:
//                                    cout << "no path construction from camera in lighttracer!" << endl;
//                                    break;
//                                default:
//                                    rTornado->rasterize(ray, SPEresult, globalEnv, 2, true);
//                                    break;
//                                    cout << "sampled p: " << SPEresult << "at lambda: 555nm" << endl;
//                            }
//                        }
//                        cout << "clicked at pixel x: " << x << " y: " << y << endl;
//
//                    }
//
//                    else {
//                        Vector3 &ref = (*image)[xres * (int) m_Event.motion.y + (int) m_Event.motion.x];
//                        cout << "pixel color: R " << ref[0] << " | G " << ref[1] << " | B " << ref[2] << endl;
//                    }
//                }
//                break;

//            case SDL_MOUSEBUTTONUP:
//                mouseLeft = false;
//                break;

            default:
                break;
        }
    }

    float stepsize = keysHeld[SDLK_LSHIFT] ? MOVEINTERVAL2 : MOVEINTERVAL;
    stepsize = keysHeld[SDLK_LCTRL] ? MOVEINTERVAL3 : stepsize;

//    if (keysHeld[SDLK_UP]) {
//        movement[2] -= stepsize;
//        sensor->flush();
//    }
//    if (keysHeld[SDLK_DOWN]) {
//        movement[2] += stepsize;
//        sensor->flush();
//    }
//    if (keysHeld[SDLK_LEFT]) {
//        movement[0] -= stepsize;
//        sensor->flush();
//    }
//    if (keysHeld[SDLK_RIGHT]) {
//        movement[0] += stepsize;
//        sensor->flush();
//    }
//    if (keysHeld[SDLK_PAGEUP]) {
//        movement[1] += stepsize;
//        sensor->flush();
//    }
//    if (keysHeld[SDLK_PAGEDOWN]) {
//        movement[1] -= stepsize;
//        sensor->flush();
//    }
//
//    cam->move(movement);
    movement = Vector3(0, 0, 0);
}


int main(int argc, char **argv) {

    Sampler* globalSampler = new Sampler();
    globalSampler->init(13499);

    /*read input*/
    sceneName = "cornelldiamond";

    xres = 1600;
    yres = 1050;

    parseArgs(argc, argv);

    /*Scene Setup*/
    scene = new Scene(sceneName);
    scene->setupAccStruct(heuristic, 0);

    /*Camera Setup*/
    //thCam = new ThinLensCamera(sceneName,xres,yres,sensitivity);
    //thCam->setStop((thCam->focalDist/2.8)*0.5f);


    /*Kernel Setup*/
    // TODO

    #ifndef NO_SDL
    /*SDL Setup*/
    sdlImage = new Image(xres,yres);
    sdlImage->pixels[2000] = Vector3(1.f,0.f,0.f);
    SDLRenderCanvas canvas = SDLRenderCanvas(xres, yres + 80, false, true);
    canvas.Initialize();
    #endif

    /*Main loop*/
    while (!closeChroma) {
        #ifndef NO_SDL
        canvas.EnterRenderMode();
        canvas.HandleEvents();

        canvas.blitImage(*sdlImage);

        canvas.renderString("text1", 0, yres + 19);
        canvas.renderString("text2", 0, yres + 34);
        canvas.renderString("text3", 0, yres + 49);
        canvas.renderString("text4", 0, yres + 64);
        canvas.renderString("text5", 0, yres + 79);
        canvas.LeaveRenderMode("Chroma Renderer");
        #endif
    }

    return 0;
}
