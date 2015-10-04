#include "Hitpoint.h"
#include "PathTracer.h"
#include "Environment.h"


PathTracer::PathTracer(const Scene *scene_in, Camera *cam_in) :
        Renderer(scene_in, cam_in) { }


PathTracer::~PathTracer() { }


void PathTracer::pathTrace(Ray ray, float &result, const int &pathDepth, ThreadEnv &env) const {

    int depth = -1; // 0 means direct light from sources!
    float pathWeight = 1.0f;
    Vector3 inc_dir;
    Hitpoint hit;

    while (pathWeight > EPS && depth < pathDepth) {
        MEASUREBRACKET(aStruct->intersect(ray, hit), env.TraversalTime)
        env.rayCount++;
        depth++;

        if (hit.hit) {
            hit.getCoordSystem();
            const Material *mat = hit.getToMaterial();

            if (mat->contribution) {
                result += pathWeight * mat->SPEgetLe(hit, ray.lambda);
                cam->toCamCoordSys(ray);
                //env.svg.drawLine(ray.origin,ray.origin+ray.direction*25,(int)ray.lambda/50);
            }

            float fr;
            MEASUREBRACKET(fr = shaders[mat->shaderID]->brdf(hit, ray, *mat, inc_dir, env), env.ShadingTime)
            float absorbCase = RR_LIMIT;

            pathWeight = (pathWeight / absorbCase) * fr * env.sampler.terminate(absorbCase);
            ray = Ray(hit.p, inc_dir, EPS, FLT_MAX, ray.lambda);
            hit.clear(); // clear does not remove lastHit index for Mailboxing!
        }
        else {
            // environment hit...
//			if(ray.direction[1]<0){
//				//...floor
//				float t=planeRayIntersection(ray,Vector3(0,1,0),Vector3(ray.origin[0],0,ray.origin[2]));
//				Hitpoint hit = Hitpoint(ray.at(t),Vector3(0,1,0),0);
//				result += pathWeight*spectrum_rgb_to_p(ray.lambda,(Stucco(hit, Vector3(0.2f,0.2f,0.1f),8)).data);
//				return;
//			}
            // ... sky
            float envRad = 0.0f;
            MEASUREBRACKET(scene->env->getEnvironmentLight(ray, envRad), env.ShadingTime)
            result += pathWeight * (envRad);
            return;
        }
    }
}


void PathTracer::pathTraceDL(Ray ray, float &result, const int &pathDepth, ThreadEnv &env) const {
    int depth = -1; // 0 means direct light from source
    float pathWeight = 1.0f;
    Vector3 inc_dir;

    Ray dummy; //TODO::kill'em!
    Hitpoint lightHit;
    Hitpoint pathHit;

    float fr, geomTermMultFr;
    float absorbCase;

    MEASUREBRACKET(aStruct->intersect(ray, pathHit), env.TraversalTime)
    env.rayCount++;
    depth++;

    if (pathHit.hit) {
        pathHit.getCoordSystem();
        const Material *mat = pathHit.getToMaterial();

        if (mat->contribution) {
            // lambertian Light Sources have eaqual radiance in all directions
            result += mat->SPEgetLe(pathHit, ray.lambda);
        }

        //direct Light
        MEASUREBRACKET(
                geomTermMultFr = shaders[mat->shaderID]->radiance(scene->neeLS, pathHit, ray, lightHit, dummy, env),
                env.ShadingTime)
        if (geomTermMultFr > EPS)
            result += pathWeight * geomTermMultFr * lightHit.index->insideMat->SPEgetLe(lightHit, ray.lambda);
        lightHit.clearAll();

        //pathTrace... without absorbtion
        MEASUREBRACKET(fr = shaders[mat->shaderID]->brdf(pathHit, ray, *mat, inc_dir, env), env.ShadingTime)
        pathWeight *= fr;
        ray = Ray(pathHit.p, inc_dir, EPS, FLT_MAX, ray.lambda);
        pathHit.clear(); // clear does not remove lastHit index for Mailboxing!
    }
    else
        return;

    //start from first hitpoint
    while (pathWeight > EPS && depth < pathDepth) {
        MEASUREBRACKET(aStruct->intersect(ray, pathHit), env.TraversalTime)
        env.rayCount++;
        depth++;

        if (pathHit.hit) {

            pathHit.getCoordSystem();
            const Material *mat = pathHit.getToMaterial();

            //direct Light
            MEASUREBRACKET(
                    geomTermMultFr = shaders[mat->shaderID]->radiance(scene->neeLS, pathHit, ray, lightHit, dummy, env),
                    env.ShadingTime)
            if (geomTermMultFr > EPS)
                result += pathWeight * geomTermMultFr * lightHit.index->insideMat->SPEgetLe(lightHit, ray.lambda);
            lightHit.clearAll();

            //pathTrace
            MEASUREBRACKET(fr = shaders[mat->shaderID]->brdf(pathHit, ray, *mat, inc_dir, env), env.ShadingTime)
            absorbCase = 0.667f;
            pathWeight = (pathWeight / absorbCase) * fr * env.sampler.terminate(absorbCase);
            ray = Ray(pathHit.p, inc_dir, EPS, FLT_MAX, ray.lambda);
            pathHit.clear(); // clear does not remove lastHit index for Mailboxing!
        }
        else break; //TODO:environment hit...
    }
}

//void PathTracer::pathtraceDLv01(Ray ray, Vector3& result, const int& pathDepth, statsBlock& env) const{
//
//	int depth = -1;
//	Vector3 pathWeight = Vector3(1,1,1);
//	Hitpoint hit;
//
//	while(pathWeight>EPS && depth < pathDepth){
//
//		aStruct->intersect(ray,hit); env.rayCount++;
//		depth++;
//
//		if(hit.hit){
//
//			hit.getCoordSystem();
//			Material& mat = *(hit.index->mat);
//
//			if(depth == 0 && mat.emitting)
//				result += mat.RGBgetEmission(hit);
//
//			result += pathWeight ^ DiffuseDirectLightning(hit,mat,env); // /PI_f doesnt vanish because no sampling in hemisphere!
//
//			float abort = genrand_real2();
//			if(abort > RR_LIMIT) break;
//
//			pathWeight = (pathWeight / RR_LIMIT) ^ mat.RGBgetDiffRefl(hit);
//
//			Vector3 inc_dir = get_cosdistr_HemSample(hit);
//			ray = Ray(hit.p,inc_dir,EPS,FLT_MAX);
//			hit.clear(); // clear does not remove lastHit index for Mailboxing!
//		}
//		else break;
//	}
//}
//
//void PathTracer::pathtracev01(Ray ray, Vector3& result, const int& pathDepth, statsBlock& env) const{
//
//	int depth = -1;
//	Vector3 pathWeight = Vector3(1,1,1);
//	Hitpoint hit;
//
//	while(pathWeight>EPS && depth < pathDepth) {
//
//		aStruct->intersect(ray,hit); env.rayCount++;
//		depth++;
//
//		if(hit.hit){
//
//			hit.getCoordSystem();
//			Material& mat = *(hit.index->mat);
//
//			if(mat.emitting)
//				// L+= LampIrradiance * Pathweight * PI
//				// pathWeight *= diffColor/PI
//				// --> L+= LampIrradiance * pathweight
//				// --> pathWeight *= diffColor
//				result += pathWeight^mat.RGBgetEmission(hit); // * PI_f / PI_f / cos_theta_i * cos_theta_i
//
//			float brdfCase = genrand_real2();
//
//			if(brdfCase > RR_LIMIT) break;
//
//			pathWeight = (pathWeight / RR_LIMIT) ^ mat.RGBgetDiffRefl(hit); //diffuse bounce
//			Vector3 inc_dir = get_cosdistr_HemSample(hit);
//			ray = Ray(hit.p,inc_dir,EPS,FLT_MAX);
//			hit.clear(); // clear does not remove lastHit index for Mailboxing!
//		}
//		else break;
//	}
//}
