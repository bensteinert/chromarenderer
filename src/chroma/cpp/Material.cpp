#include <Vector3.h>
#include <vector>
#include <fstream>

#include "Material.h"
#include "ChromaCIE.h"
#include "StringHelper.h"


Material stdDiffuse = Material();
Material stdMirr = Material(V3ZERO, Vector3(0.99, 0.99, 0.99), V3ZERO, 1.0f, 0.0f, 1000000.0f, 0, "stdMirror");
Material stdNull = Material(Vector3(1, 0, 0), Vector3(1, 0, 0), V3ZERO, 1.0f, 0.0f, 1.0f, 0, "stdNull");
Material stdAir = Material(V3ZERO, V3ZERO, Vector3(0.999f, 0.999f, 0.999f), 1.0f, 0.0f, 1.0f, 1, "stdAir");
Material lightSensing = Material(V3ZERO, V3ZERO, V3ZERO, 1.0f, 0.0f, 1.0f, 0, "sensing");


Material::Material() :
        diffRefl(Vector3(0.3, 0.3, 0.3)), specRefl(Vector3(0, 0, 0)), transmission(Vector3(0, 0, 0)),
        diffCase(1.0f), specCase(0.0f), ior(1.0f), powerEmitting(0.0f), n_u(0.0f), n_v(0.0f),
        shaderID(0), texShaderID(0), contribution(false), dispersive(false),
        B1(0.0f), B2(0.0f), B3(0.0f), C1(0.0f), C2(0.0f), C3(0.0f) {
    //stdDiffuse in case of calling the empty constructor
    verify();
}


Material::Material(
        const Vector3 &diffRefl_in, const Vector3 &specRefl_in, const Vector3 &transm_in,
        const float ior_in, const float emitting_in, const float hardness, const unsigned char shader,
        const std::string &name_in) :
        diffRefl(diffRefl_in), specRefl(specRefl_in), transmission(transm_in),
        diffCase(0.0f), specCase(0.0f), ior(ior_in), powerEmitting(emitting_in),
        n_u(hardness), n_v(hardness), shaderID(shader), texShaderID(0),
        contribution(false), dispersive(false),
        B1(0.0f), B2(0.0f), B3(0.0f), C1(0.0f), C2(0.0f), C3(0.0f) {
    //name_in.copy(name,16);
    verify();
}


Material::Material(float values[12], const std::string &name_in) :
        diffRefl(Vector3(values)), specRefl(values + 3), transmission(values + 6),
        diffCase(1.0f), specCase(0.0f), ior(values[11]), powerEmitting(values[10]),
        n_u(values[9]), n_v(values[9]), shaderID(0), texShaderID(0),
        contribution(false), dispersive(false),
        B1(0.0f), B2(0.0f), B3(0.0f), C1(0.0f), C2(0.0f), C3(0.0f) {
    if (name_in.compare(0, 6, "marble") == 0) texShaderID = 1;
    else if (name_in.compare(0, 4, "wood") == 0) texShaderID = 2;
    else if (name_in.compare(0, 6, "stucco") == 0) texShaderID = 3;
    else if (name_in.compare(0, 6, "mirror") == 0) {
        n_u = 100000;
        n_v = 100000;
        shaderID = 0;
    }    //TODO:!mirror Material shader!

    else if (name_in.compare(0, 5, "glass") == 0) {
        shaderID = 1;
        dispersive = true;
        if (name_in.length() > 5) {
            std::string code = name_in.substr(6, name_in.length() - 6);
            if (!initSellmeier(code)) {
                initSellmeierDefault();
            }
        } else {
            initSellmeierDefault();
        }
    }

    name_in.copy(name, 16);
    verify();
}


// constructor for direct glass Material construction
Material::Material(const std::string &code) :
        diffRefl(Vector3(0, 0, 0)), specRefl(Vector3(0, 0, 0)),
        transmission(Vector3(0.4, 0.4, 0.4)),    // based on 99% transmittance for 25mm thick BK7 std glass plate
        diffCase(0.0f), specCase(0.0f), powerEmitting(0.0f), n_u(100000),
        n_v(100000), shaderID(1), texShaderID(0), contribution(false), dispersive(true) {
    //code.copy(name,16);
    if (!initSellmeier(code))
        exit(1);

    verify();
}


bool Material::parse(const std::string &input) {

    std::vector<std::string> parts;     // vector for ;-seperated variables
    strsplit(input, ';', parts);        // split line
    diffCase = 0.0f;
    specCase = 0.0f;

    if (parts.size() != 10) {
        cout << "Material Parser error: " << input << endl;
        return false;
    }
    else {
        diffRefl = strtoVect(parts[0]);
        specRefl = strtoVect(parts[1]);
        transmission = strtoVect(parts[2]);
        ior = lexical_cast<float>(parts[3]);
        powerEmitting = lexical_cast<float>(parts[4]);
        n_u = lexical_cast<float>(parts[5]);
        n_v = lexical_cast<float>(parts[6]);
        shaderID = lexical_cast<int>(parts[7]);
        texShaderID = lexical_cast<int>(parts[8]);
        contribution = false;
        dispersive = false;

        if (shaderID == 1) {
            dispersive = true;
            if (!initSellmeier(parts[9])) {
                initSellmeierDefault();
            }
        }
        //parts[9].copy(name,16);
        verify();
    }

    return true;
}


Vector3 Material::RGBgetDiffRefl(const Hitpoint &hit) const {
    switch (texShaderID) {
//		case 1:
//			return Marble(hit,diffRefl,PERLIN_PARAM);
//		case 2:
//			return Wood(hit,diffRefl);
//		case 3:
//			return Stucco(hit,diffRefl,64);
        default:
            return diffRefl;
    }
}


float Material::SPEgetDiffRefl(const Hitpoint &hit, const float lambda) const {
    //TODO: Be aware with gamma corrected textures one day....
    Vector3 rgb = RGBgetDiffRefl(hit);
    float result = spectrum_rgb_to_p(lambda, rgb.data);
    return result;
}


Vector3 Material::RGBgetSpecRefl(const Hitpoint &hit) const {
    return specRefl;
}


float Material::SPEgetSpecRefl(const Hitpoint &hit, const float lambda) const {
    return spectrum_rgb_to_p(lambda, specRefl.data);
}


float Material::SPEgetLe(const Hitpoint &hit, const float lambda) const {
    return powerEmitting * getCIE_D65_SPD(lambda);
}


Vector3 Material::RGBgetTransmission(const float dist) const {
    Vector3 result = Vector3(exp(-(1.0f - transmission[0]) * dist), exp(-(1.0f - transmission[1]) * dist),
                             exp(-(1.0f - transmission[2]) * dist));
    return result;
}


float Material::SPEgetTransmission(const float dist, const float lambda) const {
    // a bit changed, so it interprets the color naturally as seen color, not absorbed color
    float power = spectrum_rgb_to_p(lambda, (transmission).data);
    float result = exp(-(1.0f - power) * dist);
    return result;
}


float Material::SPEgetIOR(const float lambda) const {
    // TODO: precompute in some way, because only 10 wavelengths for box basis are neccessary....
    if (dispersive) {
        float lambdaSq = lambda * lambda / 1000000.0f;
        float L_ior = ((B1 * lambdaSq) / (lambdaSq - C1)) + ((B2 * lambdaSq) / (lambdaSq - C2)) +
                      ((B3 * lambdaSq) / (lambdaSq - C3)) + 1.0f;    //PPBS Optik p.817:
        float result = sqrtf(L_ior);
        return result;
    }
    else return ior;
}


void Material::initSellmeierDefault() {
    // some nice dispersive Sellmaiers.... as default
    B1 = 1.46141885f;
    B2 = 0.24771301f;
    B3 = 0.94999583f;
    C1 = 0.01118261f;
    C2 = 0.05085947f;
    C3 = 112.041888f;
}


bool Material::initSellmeier(const std::string &code) {

    string path = string("./schott.csv");
    std::ifstream filestream(path.c_str(), ios::in);
    vector<string> words;
    string line;
    bool found = false;

    if (!filestream) cerr << "\tfile " << path << " not found. No Glass catalog!." << endl;
    else {
        cout << "\tLooking for " << code << " in Schott catalog... ";
        while (getline(filestream, line)) {
            if (line.empty()) continue; // skip empty lines outside data blocks
            if (line[0] == '#') continue;    // skip comments...
            else {
                words.clear();
                strsplit(line, '\t', words);
                assert(words.size() == 12);
                if (words[0].compare(code) == 0) {
                    cout << "found." << endl;
                    found = true;
                    break;
                }
            }
        }

        if (found) {
            //#Glass	nd	ne	vd	ve	code	B1		B2		B3		C1		C2		C3
//			sscanf(words[1].c_str(),"%f",&ior);
//			sscanf(words[6].c_str(),"%f",&B1);
//			sscanf(words[7].c_str(),"%f",&B2);
//			sscanf(words[8].c_str(),"%f",&B3);
//			sscanf(words[9].c_str(),"%f",&C1);
//			sscanf(words[10].c_str(),"%f",&C2);
//			sscanf(words[11].c_str(),"%f",&C3);
            ior = lexical_cast<float>(words[1]);
            B1 = lexical_cast<float>(words[6]);
            B2 = lexical_cast<float>(words[7]);
            B3 = lexical_cast<float>(words[8]);
            C1 = lexical_cast<float>(words[9]);
            C2 = lexical_cast<float>(words[10]);
            C3 = lexical_cast<float>(words[11]);
        }
        else {
            cerr << "\tERROR: glass not found!" << endl;
        }
    }
    return found;
}


void Material::verify() {

    Vector3 sum = diffRefl + specRefl;

    if (sum > 1.0f) {
        float maxVal = sum.getMax();
        diffRefl = diffRefl * (1.0f / maxVal);
        specRefl = specRefl * (1.0f / maxVal);
    }

    double spec, diff, contrib;
    spec = specRefl.getMax();
    diff = diffRefl.getMax();
    contrib = spec + diff;

    if (contrib > 0.0) {
        diffCase = diff / contrib;
        specCase = spec / contrib;
    }
    else {
        diffCase = 0.0f;
        specCase = 0.0f;
    }

    assert(diffCase + specCase == 1.0f || diffCase + specCase == 0.0f);
    assert(transmission<<1.0f);
    if (shaderID == 1) {
        assert(transmission > 0.0f);
        assert(diffCase == 0.0);
        assert(specCase == 0.0);
        assert(dispersive ? B1 > 0.0f : B1 == 0.0f);
    }
    else if (shaderID == 0) {
        assert(!(transmission>0.0f));
    }
}
