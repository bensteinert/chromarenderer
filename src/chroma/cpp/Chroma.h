#ifndef CHROMA_H
#define CHROMA_H

#include "Defines.h"
#include "Structs.h"

class Material;
class Sampler;

extern Material stdDiffuse;
extern Material stdMirr;
extern Material stdNull;
extern Material stdAir;
extern Material lightSensing;

extern Sampler* globalSampler;

static const char scene_path[] = "./scenes/";
static const char lens_path[] = "./lenses/";
static const char pupil_path[] = "./lenses/pupils/";
static const char envmap_path[] = "./envmaps/";

#endif /*CHROMA_H*/
