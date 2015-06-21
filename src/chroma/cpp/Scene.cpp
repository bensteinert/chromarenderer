#include <vector>
#include <fstream>

#include "Scene.h"
#include "Material.h"
#include "Chroma.h"
#include "AccStruct.h"
#include "BVHTree.h"
#include "SunSky.h"
#include "StringHelper.h"
#include "NextEventEstimatorLight.h"

#define NAMELENGTH 32


Scene::Scene(const std::string &sceneName) :
        aStruct(0), tris(0), spheres(0), materials(0), env(0), neeLS(0) {

    if (loadScene(sceneName) == 0) {
        std::cout << "Nothing to render... Abort" << std::endl;
        exit(1);
    }

    env = new SunSky(8.0f);
}


//Scene::Scene(const std::string &sceneName, const std::string &envName) :
//        aStruct(0), tris(0), spheres(0), materials(0), env(0), neeLS(0) {
//
//    if (!loadScene(sceneName)) {
//        std::cout << "Nothing to render. Abort" << std::endl;
//        exit(1);
//    }
//
//    env = new EnvironmentMap(envName);
//}


Scene::~Scene() {
    delete[] tris;
    delete[] spheres;
    delete[] materials;
    delete env;
    delete aStruct;
    delete neeLS;
}


int Scene::loadScene(const std::string &filename) {

    reset();
    std::string path = std::string(scene_path);
    path.append(filename);
    path.append("/");
    path.append(filename);
    std::string::size_type pathLength = path.length();

    /*Materials*/
    path.append(".mat");
    LoadMaterials(path);

    /*Primitives*/
    path.replace(pathLength, 4, ".prm");
    LoadPrimitives(path);

    /*Mesh*/
    path.replace(pathLength, 4, ".ra3");
    LoadRA3Triangles(path);

    int numPrim = numtris + numspheres + numdisks;
    return numPrim;
}


void Scene::setupAccStruct(const int heuristic, Primitive *lensSampler) {

    std::cout << "Creating Accelleration structure ..." << std::endl;
    std::clock_t stop, start = clock();

    delete aStruct;
    aStruct = (AccStruct *) new BVHTree(*this, MAX_DEPTH, MIN_TRIANGLES_PER_LEAF, heuristic, lensSampler);

    stop = clock();
    double time = ((double)(stop - start)) / CLOCKS_PER_SEC;
    std::cout <<" BVH construction time: " << time << " seconds" << std::endl;

    createEventEstimator();
}


void Scene::createEventEstimator() {
    std::vector<Primitive *> ls;
    for (int i = 0; i < aStruct->totalNumberOfPrimitives; i++) {
        if (aStruct->primitives[i]->insideMat->powerEmitting)
            ls.push_back(aStruct->primitives[i]);
    }
    neeLS = new NextEventEstimatorLight(ls);
}


void Scene::lightsImportance(bool contrib) {

    for (int i = 0; i < nummats; i++) {
        Material &matptr = materials[i];
        if (matptr.powerEmitting > 0.0f) {
            matptr.contribution = contrib;
        }
    }
}


void Scene::reset() {
    delete[] tris;
    //delete[] disks;
    delete[] spheres;
    delete[] materials;
    delete env;
    delete aStruct;

    tris = 0;
    //disks = 0;
    spheres = 0;
    materials = 0;
    numtris = nummats = numdisks = numspheres = 0;
    env = 0;
    aStruct = 0;
    totalLightArea = 0.0f;
}


int Scene::LoadRA3Triangles(const std::string &path) {
    FILE *file = fopen(path.c_str(), "rb");

    if (!file) {
        tris = 0;
        numtris = 0;
        std::cout << "file " << path << " not found. RA3 loader skipped." << std::endl;
        return -1;
    }
    std::cout << "Loading RA3 from " << path << " ";

    fseek(file, 0, SEEK_END);
    const long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    numtris = (unsigned int) (filesize / (4 * 9 + 1)); // 18 float und 2 Byte

    std::cout << "with " << numtris << " Triangles... ";

    tris = new Triangle[numtris];
    float face_point_coords[9];
    unsigned char matID = 0;

    long i = 0;
    while (i < numtris) {
        if (fread(face_point_coords, sizeof(float), 9, file) != 9) {
            std::cerr << "ERROR: Corrupted RA3 file " << path << "! Aborting" << std::endl;
            delete[] tris;
            tris = 0;
            numtris = 0;
            return 0;
        }

        fread(&matID, sizeof(char), 1, file);
        tris[i] = Triangle(face_point_coords, &materials[matID]);
        i++;
    }

    std::cout << "finished." << std::endl;
    fclose(file);
    return 1;
}


int readString(FILE *file, char *result, char limit) {
    int i = 0;
    while (i < NAMELENGTH) {
        fread(result + i, 1, 1, file);
        if (result[i] == limit) {
            result[i] = '\0';
            return i;
        }
        i++;
    }
    return 0;
}


int Scene::LoadMaterials(const std::string &path) {

    FILE *myfile;
    myfile = fopen(path.c_str(), "rb");
    float *buffer = 0;
    char *name = new char[NAMELENGTH];

    if (!myfile) {
        std::cout << "file " << path << " not found. MAT loader skipped." << std::endl;
        materials = 0;
        nummats = 0;
        return -1;
    }


    std::cout << "Loading Materials from " << path << " ";

    fseek(myfile, 0, SEEK_SET);
    fread(&nummats, 4, 1, myfile);

    std::cout << "with " << nummats << " data sets... ";

    if (nummats > 256) {
        goto readError;
    }

    materials = (Material *) malloc(sizeof(Material) * nummats); //malloc needed for a possible realloc later...

    /*
    format per Material:
    String terminated with % (max. NAMELENGTH bytes)
    3 floats diffuse color
    3 folats specualer color
    3 floats with values "phong hardness" and emitting val and IOR
     == 12 * 4 Byte to read
    */
    buffer = new float[12];
    for (int i = 0; i < nummats; i++) {
        if (readString(myfile, name, '%') == 0) {
            goto readError;
        }
        if (fread(buffer, sizeof(float), 12, myfile) != 12) {
            goto readError;
        }
        materials[i] = Material(buffer, std::string(name));
    }

    std::cout << "finished." << std::endl;
    fclose(myfile);


    delete[] buffer;
    return 1;

    readError:
    cout << "ERROR: corrupted material file! Abort" << endl;
    delete[] materials;
    delete[] buffer;
    materials = 0;
    nummats = 0;
    exit(1);
}


void Scene::parseLineForSphere(std::string line, Sphere &sphere, const int &material_offset) {

    std::vector<std::string> parts;            // vector for ;-seperated variables

    strsplit(line, ';', parts);        // split line
    if (parts.size() != 3) {
        std::cout << "parseLineForSphere error in line: " << line << std::endl;
        return;
    }
    else {
        Vector3 center = strtoVect(parts[0]);
        float radius = lexical_cast<float>(parts[1]);
        int matID = lexical_cast<int>(parts[2]) + material_offset;

        if (matID >= nummats) {
            cout << "specified material does not exist! Replaced by default" << endl;
            sphere = Sphere(center, radius, &stdDiffuse);
        }
        else {
            sphere = Sphere(center, radius, &materials[matID]);
        }
    }
}


//void Scene::parseLineForDisk(string line, Disk &disk, const int &material_offset) {
//
//    vector<string> parts;            // vector for ;-seperated variables
//
//    strsplit(line, ';', parts);        // split line
//    if (parts.size() != 4) {
//        cout << "parseLineForDisk error: " << line << endl;
//        return;
//    }
//    else {
//        Vector3 center = strtoVect(parts[0]);
//        Vector3 normal = strtoVect(parts[1]);
//        normal.normalize(); // to be sure...
//        float radius = lexical_cast<float>(parts[2]);
//        int matID = lexical_cast<int>(parts[3]) + material_offset;
//
//        if (matID >= nummats) {
//            cout << "specified material does not exist! Replaced by default" << endl;
//            disk = Disk(center, normal, radius);
//        }
//        else {
//            disk = Disk(center, normal, radius, &materials[matID]);
//        }
//    }
//}

//void Scene::parseLineForLens(string line, SphericalLens& lens,const int& material_offset){
//
//	vector<string> parts;			// vector for ;-seperated variables
//	strsplit(line,';',parts);		// split line
//
//	if(parts.size()!=6){
//		cout << "parseLineForLens error: " << line << endl;
//		return;
//	}
//	else{
//		Vector3 leftCenter = strtoVect(parts[0]);
//		float leftRadius = lexical_cast<float>(parts[1]);
//		Vector3 rightCenter = strtoVect(parts[2]);
//		float rightRadius = lexical_cast<float>(parts[3]);
//		float dia = lexical_cast<float>(parts[4]);
//		int matID = lexical_cast<int>(parts[5]) + material_offset;
//
//		cerr << "Spherical Lens import as primitive deprecated!" <<endl;
//		if(matID>=nummats){
//			cout << "specified material does not exist! Replaced by default" << endl;
//			//TODO: SphercalLensimport for this method is deprecated!
//			//lens = SphericalLens(leftCenter,leftRadius,rightCenter,rightRadius,dia);
//		}
//		else{
//			//lens = SphericalLens(leftCenter,leftRadius,rightCenter,rightRadius,dia, &materials[matID]);
//		}
//	}
//}


//void Scene::parseLineForCone(string line, Cone& cone,const int& material_offset){
//
//	vector<string> parts;			// vector for ;-seperated variables
//
//	strsplit(line,';',parts);		// split line
//	if(parts.size()!=6){
//		cout << "parseLineForCone error: " << line << endl;
//		return;
//	}
//	else{
//		Vector3 minDisk_center = strtoVect(parts[0]);
//		float minRadius = lexical_cast<float>(parts[1]);
//		Vector3 dir = strtoVect(parts[2]);
//		float axisLength = lexical_cast<float>(parts[3]);
//		float deg_angle = lexical_cast<float>(parts[4]);
//		int matID = lexical_cast<int>(parts[5]) + material_offset;
//
//		if(matID>=nummats){
//			cout << "specified material does not exist! Replaced by default" << endl;
//			cone = Cone(minDisk_center, dir, minRadius,axisLength,deg_angle);
//		}
//		else{
//			cone = Cone(minDisk_center, dir, minRadius,axisLength,deg_angle,&materials[matID]);
//		}
//	}
//}


int Scene::LoadPrimitives(const std::string &filename) {

    string sLine;

    int tri_mat_offset = nummats;

    std::ifstream filestream(filename.c_str(), std::ios::in);
    if (filestream) {

        std::cout << "Loading Primitives from " << filename << " ";

        while (getline(filestream, sLine)) {
            if (sLine.empty()) continue;     // skip empty lines outside data blocks
            if (sLine[0] == '#') continue;   // skip comments...

            else {
                if (sLine.compare("MATERIALS") == 0) {

                    std::fstream::pos_type offset = filestream.tellg();
                    unsigned int primMats = 0;

                    // count nr of entries to allocate
                    while (getline(filestream, sLine)) {
                        if (sLine.empty()) break;        // an empty line terminates a Primitive block
                        if (sLine[0] == '#') continue;    // skip comments...
                        primMats++;
                    }

                    // allocate memory for (additional) materials
                    int ptr = 0;
                    if (nummats > 0) {
                        ptr = nummats;
                        nummats = nummats + primMats;
                        materials = (Material *) realloc(materials, sizeof(Material) * (nummats));
                    }
                    else {
                        nummats = primMats;
                        materials = new Material[nummats];
                        // duplications due to two import sections cant be avoided...
                    }

                    // restart and parse
                    filestream.seekg(offset);

                    while (getline(filestream, sLine)) {
                        if (sLine.empty()) break;        // an empty line terminates a Primitive block
                        if (sLine[0] == '#') continue;    // skip comments...
                        else {
                            materials[ptr].parse(sLine);
                            ptr++;
                        }
                    }
                }

                    /******************************SPHERES*********************************/
                else if (sLine.compare("SPHERES") == 0) {

                    std::fstream::pos_type offset = filestream.tellg();

                    // count nr of entries to allocate
                    while (getline(filestream, sLine)) {
                        if (sLine.empty()) break;        // an empty line terminates a Primitive block
                        if (sLine[0] == '#') continue;    // skip comments...
                        numspheres++;
                    }

                    if (numspheres > 0) {
                        // allocate mem for Spheres
                        spheres = new Sphere[numspheres];
                        // restart and parse
                        filestream.seekg(offset);
                        int count = 0;
                        while (getline(filestream, sLine)) {
                            if (sLine.empty()) break;        // an empty line terminates a Primitive block
                            if (sLine[0] == '#') continue;    // skip comments...
                            else {
                                parseLineForSphere(sLine, spheres[count], tri_mat_offset);
                                count++;
                            }
                        }
                    }
                }

                    /******************************CONES*********************************/

                else if (sLine.compare("CONES") == 0) {

                    std::fstream::pos_type offset = filestream.tellg();

                    // count nr of entries to allocate
                    while (getline(filestream, sLine)) {
                        if (sLine.empty()) break;        // an empty line terminates a Primitive block
                        if (sLine[0] == '#') continue;    // skip comments...
                        //numcones++;
                    }

//					if(numcones>0){
//						// allocate mem for Cones
//						cones = new Cone[numcones];
//						// restart and parse
//						filestream.seekg(offset);
//						int count = 0;
//						while(getline(filestream,sLine)){
//							if( sLine.empty() ) break; 		// an empty line terminates a Primitive block
//							if( sLine[0]=='#') continue;	// skip comments...
//							else {
//								parseLineForCone(sLine,cones[count],tri_mat_offset);
//								count++;
//							}
//						}
//					}
                }

                    /******************************DISKS*********************************/
                else if (sLine.compare("DISKS") == 0) {

                    std::fstream::pos_type offset = filestream.tellg();

                    // count nr of entries to allocate
                    while (getline(filestream, sLine)) {
                        if (sLine.empty()) break;        // an empty line terminates a Primitive block
                        if (sLine[0] == '#') continue;    // skip comments...
                        numdisks++;
                    }

//                    if (numdisks > 0) {
//                        // allocate mem for Disks
//                        disks = new Disk[numdisks];
//                        // restart and parse
//                        filestream.seekg(offset);
//                        int count = 0;
//                        while (getline(filestream, sLine)) {
//                            if (sLine.empty()) break;        // an empty line terminates a Primitive block
//                            if (sLine[0] == '#') continue;    // skip comments...
//                            else {
//                                parseLineForDisk(sLine, disks[count], tri_mat_offset);
//                                count++;
//                            }
//                        }
//                    }
                }

                    /******************************LENSES*********************************/
                else if (sLine.compare("LENSES") == 0) {

                    std::fstream::pos_type offset = filestream.tellg();

                    // count nr of entries to allocate
                    while (getline(filestream, sLine)) {
                        if (sLine.empty()) break;        // an empty line terminates a Primitive block
                        if (sLine[0] == '#') continue;    // skip comments...
                        //numlenses++;
                    }
//					if(numlenses>0){
//						// allocate mem for Disks
//						lenses = new SphericalLens[numlenses];
//						// restart and parse
//						filestream.seekg(offset);
//						int count = 0;
//						while(getline(filestream,sLine)){
//							if( sLine.empty() ) break; 		// an empty line terminates a Primitive block
//							if( sLine[0]=='#') continue;	// skip comments...
//							else {
//								parseLineForLens(sLine,lenses[count],tri_mat_offset);
//								count++;
//							}
//						}
//					}
                }
            }
        }
        std::cout << "finished" << std::endl;
        return 1;
    }

    std::cout << "file " << filename << " not found. PRM loader skipped." << std::endl;
    return 0;
}

