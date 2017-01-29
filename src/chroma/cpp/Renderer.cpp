#include "Defines.h"
#include "ChromaCIE.h"
#include "Renderer.h"
#include "BSPUtils.h"
#include "Hitpoint.h"
#include "Primitive.h"
#include "Environment.h"


Renderer::Renderer(const Scene *scene_in, Camera *cam_in) :
        rayDepthLimit(0), aStruct(scene_in->aStruct), scene(scene_in), cam(cam_in)
        //bp(BlinnPhong(scene_in)), gs(Dielectric(scene_in))
{
    initShaders();
}


void Renderer::reinit(const Scene *scene_in) {
    scene = scene_in;
    aStruct = scene_in->aStruct;
    //bp = BlinnPhong(scene);
    //gs = Dielectric(scene);
}


void Renderer::initShaders() {
    //shaders = new Shader *[2];
    //shaders[0] = &(Renderer::bp);
    //shaders[1] = &(Renderer::gs);
}


Renderer::~Renderer() {
    //delete[] shaders;
}


void Renderer::rasterize(Ray &ray, float &result, ThreadEnv &env, int mode, bool verbose) const {
    Hitpoint hit;
    int depth = 0;
    MEASUREBRACKET(aStruct->intersect(ray, hit), env.TraversalTime)
    env.rayCount++;
    float pathWeight = 1.0f;
    ray.lambda = 555;

    if (hit.hit) {

        const Primitive *p = hit.index;
        const Material *mat = p->insideMat;

        if (verbose)
            cout << "Surface hit at " << hit.p << "\nwith normal " << hit.n << " adress: " << hit.index << endl;

        switch (mode) {
            case 0: {
                float fr = mat->SPEgetDiffRefl(hit, ray.lambda);
                Vector3 normal = p->getNormal(hit.p, -ray.direction);
                result = fr * max(normal * (-ray.direction), 0.0f);
                break;
            }
            case 1: {
                Vector3 normal = p->getNormal(hit.p, -ray.direction);
                result = spectrum_rgb_to_p(ray.lambda, vabs(normal).data);
                break;
            }
            case 2:
                //follow refracted rays
                while (mat->shaderID == 1 && hit.hit) {
                    depth++;
                    Vector3 refr_dir;
                    hit.getCoordSystem();
                    float fr = 0.0f;
                    //MEASUREBRACKET(fr = shaders[mat->shaderID]->brdf(hit, ray, *mat, refr_dir, env), env.ShadingTime);
                    pathWeight = pathWeight * fr;
                    ray = Ray(hit.p, refr_dir, EPS, FLT_MAX, ray.lambda);
                    hit.clear(); // clear does not remove lastHit index for Mailboxing!
                    MEASUREBRACKET(aStruct->intersect(ray, hit), env.TraversalTime)
                    env.rayCount++;
                    p = hit.index;
                    mat = p->insideMat;
                    if (depth >= 5) {
                        return;
                    }
                }
                if (hit.hit) {
                    // hitpoint after glass?
                    float fr = mat->SPEgetDiffRefl(hit, ray.lambda);
                    Vector3 normal = p->getNormal(hit.p, -ray.direction);
                    //result = (pathWeight*fr) * max(normal*(-ray.direction),0.0f);
                    result = pathWeight * fr;
                }
                break;
            case 3:
                //follow mirror rays
                while (mat->shaderID == 2 && hit.hit) {
                    Vector3 mirrorDir = mirror(ray.direction, hit.n);
                    ray = Ray(hit.p, mirrorDir, EPS, FLT_MAX, ray.lambda);
                    hit.clear(); // clear does not remove lastHit index for Mailboxing!
                    MEASUREBRACKET(aStruct->intersect(ray, hit), env.TraversalTime)
                    env.rayCount++;
                    p = hit.index;
                    mat = p->insideMat;
                }
                if (hit.hit) {
                    // hitpoint after mirror?
                    float fr = mat->SPEgetDiffRefl(hit, ray.lambda);
                    Vector3 normal = hit.n;
                    result = (pathWeight * fr) * max(normal * (-ray.direction), 0.0f);
                }
                break;
        }
    }
    else {
        //TODO:environment hit...
        if (ray.direction[1] < 0) {
            //...floor
            //float t = planeRayIntersection(ray, Vector3(0, 1, 0), Vector3(ray.origin[0], 0, ray.origin[2]));
            //Hitpoint hit = Hitpoint(ray.at(t), Vector3(0, 1, 0), 0);
            //TODO some procedural texture with t and hit maybe?
            result += pathWeight * spectrum_rgb_to_p(ray.lambda, Vector3(0.2f, 0.2f, 0.2f).data);
        }
        else {
            // environment hit...
            float envRad = 0.0f;
            MEASUREBRACKET(scene->env->getEnvironmentLight(ray, envRad), env.ShadingTime)
            result += pathWeight * envRad;
        }
    }
    return;
}
