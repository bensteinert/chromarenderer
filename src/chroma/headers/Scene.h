#ifndef _SCENE_H_
#define _SCENE_H_

#include <iostream>

#include "Primitive.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Material.h"

class Environment;
class NextEventEstimatorLight;
class AccStruct;

class Scene {

public:

    // central Primitives administration:
    AccStruct *aStruct;
    Triangle *tris;
    Sphere *spheres;
    //Disk* disks;
    Material *materials;
    Environment *env;
    NextEventEstimatorLight *neeLS;

    unsigned int numtris;
    unsigned int numspheres;
    unsigned int numdisks;
    unsigned int nummats;
    float totalLightArea;
    float totalLightPower;

    Scene(const std::string &sceneName);

    // TODO MIGRATION: Envmaps not yet migrated
    //Scene(const std::string &sceneName, const std::string &envName);

    ~Scene();

    int loadScene(const std::string &filename);

    void setupAccStruct(const int heuristic, Primitive *lensSampler = 0);

    void lightsImportance(bool blub);

//	inline void getEnvRadiance(const Ray& ray, float& result){
//		env->getEnvironmentLight(ray,result);
//	}

private:
    void reset();

    int LoadRA3Triangles(const std::string &filename);
    int LoadPrimitives(const std::string &filename);
    int LoadMaterials(const std::string &filename);

    void LoadCamera(const std::string &filename);

    void parseLineForSphere(std::string line, Sphere &sphere, const int &material_offset);
    void parseLineForMaterial(std::string line, Material &mat);

    //void parseLineForCone(string line, Cone& cone,const int& material_offset);
    //void parseLineForDisk(std::string line, Disk &disk, const int &material_offset);
    //void parseLineForLens(string line, SphericalLens& lens,const int& material_offset);

    void createEventEstimator();
};

#endif
