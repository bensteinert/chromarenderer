#include <ctime>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <cmath>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <Renderer.h>
#include <Structs.h>

#include "SDLRenderCanvas.h"
#include "Sampler.h"
#include "ThinLensCamera.h"
#include "Chroma.h"
#include "Scene.h"
#include "AccumulationBuffer.h"
#include "ChromaCIE.h"


//*******************

Image *rgbImage;
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
Camera *camera = 0;
AccumulationBuffer *sensor = 0;
SDLRenderCanvas *canvas = 0;
Sampler *globalSampler;

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

Vector3 movement(0, 0, 0);
bool mouseLeft = false;
bool keysHeld[323] = {false};
///*DEBUG VARS*/
//int dbg = 0;
//int param = 4;
//char * title = new char[TEXTOUTPUTLEN];
//char * titlePart = new  char[30];
int textSwitch = 1;
char *text1 = new char[TEXTOUTPUTLEN];
char *text2 = new char[TEXTOUTPUTLEN];
char *text3 = new char[TEXTOUTPUTLEN];
char *text4 = new char[TEXTOUTPUTLEN];
char *text5 = new char[TEXTOUTPUTLEN];

//char * logLine = new char[256];
long t0, t;
time_t start, last, actual;
int frames = 0;
float fps = 0.0f;
//int saveLimit=1000;
//int pupilLimit = 250;

//int debug = 0;
//float lensTracePercent = 0.0f;
//float shadingPercent = 0.0f;
//float traversalPercent = 0.0f;

float fpsMeter() {
    frames++;
    actual = clock();
    float elapsed = (float) (actual - last) / CLOCKS_PER_SEC;

    if (elapsed >= 5.0f) {
        fps = (float) frames / elapsed;
        frames = 0;
        last = clock();
    }
    return elapsed;
}


void hdrScreenShot(const Image &img) {

    time_t actual;
    struct tm *timeinfo;
    time(&actual);
    timeinfo = localtime(&actual);
    std::string path = "./shots/" + sceneName;
    char suffix[64];
    snprintf(suffix, 16, "%d_%d%d%d", sensor->acc, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);


    struct stat stBuf;
    if (stat(path.c_str(), &stBuf) != 0) {
        // directory does not exist!
        mkdir(path.c_str(), 0774);
    }

    path = path + "/" + sceneName + "_" + suffix;
    std::string::size_type pathLength = path.length();

    path.append(".hdr");
    img.saveHDR(path.c_str());
    std::cout << "HDR screenshot saved to " << path << std::endl;

    path.replace(pathLength, 4, ".txt");
    std::ofstream fs(path.c_str(), ios::out);

    if (fs) {
        double elapsed = difftime(actual, start) / 60.0;
        fs << "Date of Render:\t\t" << asctime(timeinfo);
        fs << "Elapsed time:\t\t" << int(elapsed) << "minutes" << std::endl;
        fs << "Render Passes:\t\t" << sensor->acc << std::endl;;
        fs << "Color Mode:\t\t\t" << (spectral ? "Spectral 10" : "RGB") << std::endl;
        fs << "Render Method:\t\t";
        switch (method) {
            case 1:
                fs << "Rasterization";
                break;
            case 2:
                fs << "Path Tracing";
                break;
            case 3:
                fs << "Path Tracing with Direct Light Estimation";
                break;
            case 4:
                fs << "Light Tracing with Direct Light Estimation";
                break;
        }
        fs << std::endl;


        fs << "avg brightness:\t\t" << img.L1Norm() << std::endl;
        fs << "avg RGB brightness:\t" << img.L1NormRGB() << std::endl;
        fs << std::endl << "Camera Stats:" << std::endl;
        fs << *camera << std::endl;

    }
    fs.close();
}


void writelog(const char *line) {
    FILE *logfile = fopen("logfile.txt", "at");
    if (!logfile) {
        cerr << "Logfile not open!" << std::endl;
        return;
    }
    else fwrite(line, 1, strlen(line), logfile);
    fclose(logfile);
}


inline void mapDirToDebevecUv(const Vector3 &dir, float &u, float &v) {
    float r = (1 / PI) * acosf(dir[2]) / sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
    u = dir[0] * r;
    v = dir[1] * r;
    u = (u + 1.0) / 2.0;
    v = (v + 1.0) / 2.0;
}


inline void flushText() {
    memset(text1, ' ', TEXTOUTPUTLEN);
    memset(text2, ' ', TEXTOUTPUTLEN);
    memset(text3, ' ', TEXTOUTPUTLEN);
    memset(text4, ' ', TEXTOUTPUTLEN);
    memset(text5, ' ', TEXTOUTPUTLEN);
}


inline void lensStats() {
    //snprintf(text1,64,"apertureBlockCount: %d",globalEnv.apertureBlockedRayCount);
    //snprintf(text2,64,"lensBodyBlockCount: %d",globalEnv.lensBodyHitCount);
    //snprintf(text1, 64, "positives: %d", globalEnv.positives);
    //snprintf(text2, 64, "negatives: %d", globalEnv.negatives);
    //snprintf(text3, 64, "innerReflections:   %d", globalEnv.innerReflectionCount);
    //snprintf(text4, 64, "total LensRays:   %d", globalEnv.lensRayCount);
    //snprintf(text5,64,"per frame");
    //snprintf(text5, 64, "diffracted positives: %d", globalEnv.diffracted);
}


inline void camStats() {
    camera->stats(text1, text2, text3, text4, text5, TEXTOUTPUTLEN);
}


inline void renderStats(double inv_elapsed_t) {
    //snprintf(text1, 64, "rays/sec:   %.1f", globalEnv.rayCount * inv_elapsed_t);
    //snprintf(text2, 64, "visTests/sec:   %.1f", globalEnv.visCount * inv_elapsed_t);
    snprintf(text4, 64, "L1: %.4f", rgbImage->L1Norm());
}


inline void SDLBlitAndHandle(const Image &img) {
    switch (textSwitch) {
        case 1:
            renderStats(1000.0 / ((double) (SDL_GetTicks() - t0)));
            break;
        case 2:
            camStats();
            break;
        case 3:
            lensStats();
            break;
        default:
            break;
    }

    canvas->EnterRenderMode();
    canvas->HandleEvents();

    if (mirrorX) {
        if (mirrorY) {
            canvas->blitImage_mirroredXY(img);
        }
        else {
            canvas->blitImage_mirroredX(img);
        }
    }
    else {
        if (mirrorY) {
            canvas->blitImage_mirroredY(img);
        }
        else {
            canvas->blitImage(img);
        }
    }

    canvas->renderString(text1, 0, yres + 15);
    canvas->renderString(text2, 0, yres + 30);
    canvas->renderString(text3, 0, yres + 45);
    canvas->renderString(text4, 0, yres + 60);
    canvas->renderString(text5, 0, yres + 75);
    canvas->LeaveRenderMode("Chroma Renderer");
}


void writeCommands() {
    printf("F1: rasterize || F2: PT || F3: PTDL || F4:LTDL || F12: Spectral/RGB\n"
                   "q/a: Stop Up/Down\n"
                   "w/s: shift sensor backward(+z) / forward\n"
                   "e/d: increase/decrease EV\n"
                   "r/f: zoom in/out\n");
}


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

// focus to distance of hit geometry in the centre of the image
void autoFocus(){
    Vector3 viewDir = camera->dirInWorldCoords(Vector3(0, 0, -1));
    Ray ray = Ray(camera->pos + viewDir * 0.1f, viewDir, 0.0f, FLT_MAX, 555);
    Hitpoint p;

    scene->aStruct->intersect(ray, p);
    if (p.hit) {
        camera->focus(p.dist * 1000.0f);
    }
    sensor->clear();
}


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
                    case SDLK_c:
                        std::cout << camera->pos << " | " << camera->rot[0] << " | " << camera->rot[1] << " | " <<
                        camera->rot[2] << std::endl;
                        break;

                    case SDLK_F10:
                        hdrScreenShot(*rgbImage);
                        break;

                    case SDLK_F11:
                        char filename[32];
                        snprintf(filename, 32, "sensor%d.tga", sensor->acc);
                        rgbImage->saveTonemapped(filename);
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

                    case SDLK_KP_MINUS:
                        camera->superSample(0.5f);
                        sensor->clear();
                        break;
                    case SDLK_KP_PLUS:
                        camera->superSample(2.0f);
                        sensor->clear();
                        break;
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
                    case SDLK_SPACE:
                        SDL_WarpMouse(xres / 2, yres / 2);
                        camera->reset();
                        sensor->clear();
                        break;

                    case SDLK_y:
                        mirrorY = !mirrorY;
                        break;
                    case SDLK_x:
                        mirrorX = !mirrorX;
                        break;
                    case SDLK_p:
                        textSwitch = 2;
                        flushText();
                        break;
                    case SDLK_o:
                        textSwitch = 1;
                        flushText();
                        break;
                    case SDLK_l:
                        textSwitch = 3;
                        flushText();
                        break;
                    case SDLK_q:
                        camera->stopUp();
                        sensor->clear();
                        break;
                    case SDLK_a:
                        camera->stopDown();
                        sensor->clear();
                        break;
                    case SDLK_w:
                        if (keysHeld[SDLK_LSHIFT]) camera->sensorZPos += 0.1f;
                        else camera->sensorZPos += 0.5f;
                        sensor->clear();
                        break;
                    case SDLK_s:
                        if (keysHeld[SDLK_LSHIFT]) camera->sensorZPos -= 0.1f;
                        else camera->sensorZPos -= 0.5f;
                        sensor->clear();
                        break;
                    case SDLK_e:
                        camera->sensitivity *= 2.0f;
                        sensor->clear();
                        break;
                    case SDLK_d:
                        if (camera->sensitivity > EPS * 100.0f) camera->sensitivity *= 0.5f;
                        sensor->clear();
                        break;
                    case SDLK_r:
                        camera->zoomRel(2.0f);
                        sensor->clear();
                        break;
                    case SDLK_f:
                        camera->zoomRel(0.5f);
                        sensor->clear();
                        break;
//
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
                    case SDLK_F9: {
                        autoFocus();
                    }

                    default:
                        break;
                }
                break;

            case SDL_MOUSEMOTION:
                if (mouseLeft) {
                    sensor->flush();
                    int diff_x = -m_Event.motion.xrel;
                    int diff_y = -m_Event.motion.yrel;
                    diff_x = abs(diff_x) > 30 ? 30 * (abs(diff_x) / diff_x) : diff_x; //clamp
                    diff_y = abs(diff_y) > 30 ? 30 * (abs(diff_y) / diff_y) : diff_y; //clamp
                    camera->rotate(diff_x, diff_y);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
                    mouseLeft = true;
                }

                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3)) {
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
//                                    cout << "no path construction from camera in lighttracer!" << std::endl;
//                                    break;
//                                default:
//                                    rTornado->rasterize(ray, SPEresult, globalEnv, 2, true);
//                                    break;
//                                    cout << "sampled p: " << SPEresult << "at lambda: 555nm" << std::endl;
//                            }
//                        }
//                        cout << "clicked at pixel x: " << x << " y: " << y << std::endl;
//
//                    }

                    //else {
                    Vector3 &ref = (*rgbImage)[xres * (int) m_Event.motion.y + (int) m_Event.motion.x];
                    std::cout << "pixel color: R " << ref[0] << " | G " << ref[1] << " | B " << ref[2] << std::endl;
                    //}
                }
                break;

            case SDL_MOUSEBUTTONUP:
                mouseLeft = false;
                break;

            default:
                break;
        }
    }

    float stepsize = keysHeld[SDLK_LSHIFT] ? MOVEINTERVAL2 : MOVEINTERVAL;
    stepsize = keysHeld[SDLK_LCTRL] ? MOVEINTERVAL3 : stepsize;

    if (keysHeld[SDLK_UP]) {
        movement[2] -= stepsize;
        sensor->flush();
    }
    if (keysHeld[SDLK_DOWN]) {
        movement[2] += stepsize;
        sensor->flush();
    }
    if (keysHeld[SDLK_LEFT]) {
        movement[0] -= stepsize;
        sensor->flush();
    }
    if (keysHeld[SDLK_RIGHT]) {
        movement[0] += stepsize;
        sensor->flush();
    }
    if (keysHeld[SDLK_PAGEUP]) {
        movement[1] += stepsize;
        sensor->flush();
    }
    if (keysHeld[SDLK_PAGEDOWN]) {
        movement[1] -= stepsize;
        sensor->flush();
    }

    camera->move(movement);
    movement = Vector3(0, 0, 0);
}


int main(int argc, char **argv) {

    globalSampler = new Sampler();
    globalSampler->init(13499);

    /*read input*/
    sceneName = "cornelldiamond";

    xres = 1200;
    yres = 900;

    parseArgs(argc, argv);

    /*Scene Setup*/
    scene = new Scene(sceneName);
    scene->setupAccStruct(heuristic, 0);

    /*Camera Setup*/
    camera = new ThinLensCamera(sceneName, xres, yres, sensitivity);
    camera->setStopNumber(2.8);
    sensor = new AccumulationBuffer(xres, yres);

    /*Kernel Setup*/
    Renderer *renderer = new Renderer(scene, camera);

    // Global for the moment...
    ThreadEnv tEnv;
    tEnv.sampler = *globalSampler;

#ifndef NO_SDL
    /*SDL Setup*/
    rgbImage = new Image(xres, yres);
    rgbImage->pixels[2000] = Vector3(1.f, 0.f, 0.f);
    canvas = new SDLRenderCanvas(xres, yres + 80, false, true);
    canvas->Initialize();
#endif

    /*Main loop*/
    while (!closeChroma) {
        int x, y;
        int pos = 0;
        Ray ray;
        float result;
        float pathWeight;
        int saveLimit = 1000;

        for (y = 0; y < yres; y++) {
            for (x = 0; x < xres; x++) {
                pos = camera->getRay(x, y, ray, pathWeight, tEnv);
                if (pos > -1) {
                    result = 0.0f;
                    renderer->rasterize(ray, result, tEnv, 2);
                    pathWeight *= 0.01f;
                    Vector3 xyzColor;
                    spectrum_p_to_xyz(ray.lambda, result, xyzColor.data);
                    sensor->accumulateGlobal((xyzColor * pathWeight), pos);    //not sure...
                }//ray gen
            } //x
        }//y

        if (fpsMeter() > 1.0f) {
            sensor->convertXYZtoRGB(*rgbImage);
        }

        /********screenshot?**********/
        if (sensor->acc >= saveLimit) {
            hdrScreenShot(*rgbImage);
            saveLimit += 1000;
        }

#ifndef NO_SDL
        SDLBlitAndHandle(*rgbImage);
#endif
    }

    return 0;
}
