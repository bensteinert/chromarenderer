#ifndef RENDERER_H_
#define RENDERER_H_


#include "BVHTree.h"
#include "Ray.h"
#include "Hitpoint.h"
#include "Scene.h"
#include "Camera.h"

using namespace std;

class Renderer {

public:

    int rayDepthLimit; // 1 means two ray paths and 3 hitpoints...
    const AccStruct *aStruct;
    const Scene *scene;
    Camera *cam;

    //BlinnPhong bp;
    //Dielectric gs;
    //Shader **shaders;

    Renderer(const Scene *scene, Camera *cam_in);

    ~Renderer();

    void reinit(const Scene *scene_in);

    void rasterize(Ray &ray, float &result, ThreadEnv &sb, int mode = 0, bool verbose = false) const;


    inline void refreshAStruct(const AccStruct *newStruct) {
        aStruct = newStruct;
        //bp.aStruct = newStruct;
        //gs.aStruct = newStruct;
    }


private:
    void initShaders();
};

#endif /*RENDERER_H_*/
