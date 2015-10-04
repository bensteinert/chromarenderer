#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "Renderer.h"

using namespace std;


class PathTracer : public Renderer{

public:

	PathTracer(const Scene* scene, Camera* cam_in);
	~PathTracer();

	void pathTrace(Ray ray, float &result, const int &pathDepth, ThreadEnv &sb) const;
	void pathTraceDL(Ray ray, float &result, const int &pathDepth, ThreadEnv &sb) const;

private:
	//inline Vector3 DirectLightning(const Hitpoint& hit, const Ray& ray, const Material& mat, const Vector3& pathWeight, statsBlock& sb) const;
	//inline Vector3 DiffuseDirectLightning(const Hitpoint& hit, const Material& mat, statsBlock& sb) const;

};
#endif /*RAYTRACER_H_*/
