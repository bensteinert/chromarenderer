#include <iostream>

#include "Ray.h"
#include "SunSky.h"
#include "ChromaCIE.h"

const float shader_S0_380_780[] = {63.4,65.8,94.8,104.8,105.9,96.8,113.9,125.6,125.5,121.3,121.3,113.5,113.1,110.8,106.5,108.8,105.3,104.4,100,96,95.1,89.1,90.5,90.3,88.4,84,85.1,81.9,82.6,84.9,81.3,71.9,74.3,76.4,63.3,71.7,77,65.2,47.7,68.6,65,};
const float shader_S1_380_780[] = { 38.5, 35, 43.4, 46.3, 43.9, 37.1, 36.7, 35.9, 32.6, 27.9, 24.3, 20.1, 16.2, 13.2, 8.6, 6.1, 4.2, 1.9, 0, -1.6, -3.5, -3.5, -5.8, -7.2, -8.6, -9.5, -10.9, -10.7, -12, -14, -13.6, -12, -13.3, -12.9, -10.6, -11.6, -12.2, -10.2, -7.8, -11.2, -10.4,};
const float shader_S2_380_780[] = {3,1.2,-1.1,-0.5,-0.7,-1.2,-2.6,-2.9,-2.8,-2.6,-2.6,-1.8,-1.5,-1.3,-1.2,-1,-0.5,-0.3,0,0.2,0.5,2.1,3.2,4.1,4.7,5.1,6.7,7.3,8.6,9.8,10.2,8.3,9.6,8.5,7,7.6,8,6.7,5.2,7.4,6.8,};
//float shader_sun_power[38];

// "A Practical Analytic Model for Daylight" Table 2. Sun spectral radiance W/(cm^2 um sr) in 10nm Steps
const double shader_daylight_sunSpecRad_380_750[] = {1655.9,1623.37,2112.75,2588.82,2582.91,2423.23,2676.05,2965.83,3054.54,3005.75,3066.37,2883.04,2871.21,2782.5,2710.06,2723.36,2636.13,2550.38,2506.02,2531.16,2535.59,2513.42,2463.15,2417.32,2368.53,2321.21,2282.77,2233.98,2197.02,2152.67,2109.79,2072.83,2024.04,1987.08,1942.72,1907.24,1862.89,1825.92,};
const double shader_daylight_k_o_450_770[] = {0.003,0.006,0.009,0.014,0.021,0.03,0.04,0.048,0.063,0.075,0.085,0.103,0.12,0.12,0.115,0.125,0.12,0.105,0.09,0.079,0.067,0.057,0.048,0.036,0.028,0.023,0.018,0.014,0.011,0.01,0.009,0.007,0.004,};
const double shader_daylight_k_g_760_770[] = {3.0, 0.21,};
const double shader_daylight_k_wa_690_780[] = {0.016,0.024,0.0125,1,0.87,0.061,0.001,1e-05,1e-05,0.0006,};

//int sky_daylight_sundir(float *dir, void *data)
//{
//  shader_daylight_t* s = (shader_daylight_t *)data;
//  for(int k=0;k<3;k++) dir[k] = s->sun_dir[k];
//  return 1;
//}

float DistributionPerez(const float* coeff, float cosThetaS2, float thetaSun, float thetaV, float gamma)
{
  float cosGamma2 = cosf(gamma); cosGamma2*=cosGamma2;
  float p0 = (1 + coeff[0]*expf(coeff[1]/cosf(thetaV)))*(1+coeff[2]*expf(coeff[3]*gamma)+coeff[4]*cosGamma2);
  float p1 = (1 + coeff[0]*expf(coeff[1]))*(1+coeff[2]*expf(coeff[3]*thetaSun)+coeff[4]*cosThetaS2);
  return p0 / p1;
}

void getSunDirForTime(const float timeOfDay, Vector3& dir){

	float timeVal = (timeOfDay-5.0f)/15.0f;
	float x = (timeVal-0.5f)*4;
//	float y = -cos((timeVal-0.5f)*PI_f);
//	float z = -cos((timeVal-0.5f)*PI_f)*3.5f;
	float z = -cos((timeVal-0.5f)*PI_f);
	float y = -cos((timeVal-0.5f)*PI_f)*3.5f;
	dir = Vector3(x,y,z).normalizedCopy();
}

SunSky::SunSky(const float timeOfDay){

	getSunDirForTime(timeOfDay,sunDir);	// dir from sun to scene!
	//float tmp = -sqrtf(3.0f);
	//sunDir = Vector3(tmp,tmp,-tmp);
	//sunDir.normalize();
	std::cout << "direction from sun: " << sunDir << std::endl;
	turbidity = 2.0f;
	thetaSunInRad = acosf(-sunDir[1]);	//acosf(-sunDir[2]);
	sun_power_scale = 400.f/(turbidity*turbidity); // 400

	Init();

	xyz_to_rgb(sun_XYZ.data, sun_rgb.data);
	fprintf(stderr, "sun color XYZ: %f %f %f\n", sun_XYZ[0], sun_XYZ[1], sun_XYZ[2]);
	fprintf(stderr, "sun color RGB: %f %f %f\n", sun_rgb[0], sun_rgb[1], sun_rgb[2]);
}

void SunSky::Init(){
	// precomputed the values for the sky-color model

	float theta = thetaSunInRad;
	float theta2 = theta * theta;
	float theta3 = theta * theta2;
	float t = turbidity;
	float t2 = t * t;

	zenith_xyY[0] = ( 0.00166*theta3 - 0.00375*theta2 + 0.00209*theta + 0) * t2 +
	(-0.02903*theta3 + 0.06377*theta2 - 0.03203*theta + 0.00394) * t +
	( 0.11693*theta3 - 0.21196*theta2 + 0.06052*theta + 0.25886);
	zenith_xyY[1] = ( 0.00275*theta3 - 0.00610*theta2 + 0.00317*theta + 0) * t2 +
	(-0.04214*theta3 + 0.08970*theta2 - 0.04153*theta + 0.00516) * t +
	( 0.15346*theta3 - 0.26756*theta2 + 0.06670*theta + 0.26688);
	zenith_xyY[2] = (4.0453 * t - 4.9710) * tan((4.0/9.0 - t/120.0) * (PI_f - 2*theta)) - 0.2155 * t + 2.4192;

	// the constants are from "A Practical Analytical model for Daylight" Appendix A (p.22)
	perez_Y[0] =  0.1787f * t - 1.4630f;
	perez_Y[1] = -0.3554f * t + 0.4275f;
	perez_Y[2] = -0.0227f * t + 5.3251f;
	perez_Y[3] =  0.1206f * t - 2.5771f;
	perez_Y[4] = -0.0679f * t + 0.3703f;
	perez_x[0] = -0.0193f * t - 0.2592f;
	perez_x[1] = -0.0665f * t + 0.0008f;
	perez_x[2] = -0.0004f * t + 0.2125f;
	perez_x[3] = -0.0641f * t - 0.8989f;
	perez_x[4] = -0.0033f * t + 0.0452f;
	perez_y[0] = -0.0167f * t - 0.2608f;
	perez_y[1] = -0.0950f * t + 0.0092f;
	perez_y[2] = -0.0079f * t + 0.2102f;
	perez_y[3] = -0.0441f * t - 1.6537f;
	perez_y[4] = -0.0109f * t + 0.0529f;

	// float sun[sizeof(sunSpecRad_380_750)/sizeof(double)]; // in float because of conversion into XYZ
	// relative optical mass
	double m = 1.0 / ( cos(thetaSunInRad) + 0.15*pow((93.885 - (180.0/PI_f)*thetaSunInRad),-1.253));
	double beta = 0.04608*(turbidity) + 0.04586;
	double alpha = 1.3;
	double l = 0.35;  // cm; ozone
	double w = 2.0; // cm;
	fprintf(stderr, "\n m=%e", m);
	int lambda_start = 38;
	sun_XYZ = Vector3(0,0,0);

	for (int lambda_nm10 = lambda_start; lambda_nm10 <= 75; lambda_nm10 += 1)
	{
		double tau_r, tau_a, tau_o, tau_g, tau_wa;
		double lambda_um = lambda_nm10/100.0;  // lambda in um
		int i = lambda_nm10 - lambda_start;
		tau_r = exp(-m * 0.008735 * pow(lambda_um, -4.08)); // copied from RiSunConstants.C
		tau_a = exp(-m * beta * pow(lambda_um, -alpha));
		// if (lambda_nm10 >= 45 && lambda_nm10 <= 77)
		tau_o = exp(-shader_daylight_k_o_450_770[lambda_nm10-lambda_start]*l*m);
		// else tau_o = 1.0;
		if (lambda_nm10 >= 76 && lambda_nm10 <= 77) // !!TODO: check: this seems to be always 1!!
			tau_g = exp((-1.41*shader_daylight_k_g_760_770[lambda_nm10-76]*m)/pow(1.0+118.93*shader_daylight_k_g_760_770[lambda_nm10-76]*m, 0.45));
		else tau_g = 1.0;
		// if (lambda_nm10 >= 69 && lambda_nm10 <= 78)
		tau_wa = exp((-0.2385*shader_daylight_k_wa_690_780[lambda_nm10-69]*w*m)/pow(1.0+20.07*shader_daylight_k_wa_690_780[lambda_nm10-69]*w*m, 0.45));
		// else tau_wa = 1.0;

		float sunAmpl = (float)(sun_power_scale*tau_r*tau_a*tau_o*tau_g*tau_wa*shader_daylight_sunSpecRad_380_750[lambda_nm10-lambda_start]);
		//shader_sun_power[i] = sunAmpl*38.0*20;
		sun_XYZ[0] += sunAmpl * spectrum_x[i+2]; // /38.0;
		sun_XYZ[1] += sunAmpl * spectrum_y[i+2]; // /38.0;
		sun_XYZ[2] += sunAmpl * spectrum_z[i+2]; // /38.0;
	}
	//const float x = sun_XYZ[0]/(sun_XYZ[0] + sun_XYZ[1] + sun_XYZ[2]);
	//const float y = sun_XYZ[1]/(sun_XYZ[0] + sun_XYZ[1] + sun_XYZ[2]);
	//sun_M1 = (-1.3515f - 1.7703f*x + 5.9114f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
	//sun_M2 = (0.03f - 31.4424f*x + 30.0717f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);

}


SunSky::~SunSky(){
}

void SunSky::getEnvironmentLight(const Ray& ray, float& radiance)const{

//	if(overcast){
//		getOvercastSkylight(ray.direction,radiance);
//		return;
//	}
	Vector3 direction = ray.direction;//-Vector3(ray.direction[0],ray.direction[2],ray.direction[1]);

	float thetaV;        // the angle about surface normal
	float gamma;         // the angle between direction and sun
	float x,y,Y;

	gamma = acosf(fminf(-(direction*sunDir),1.0f));
//	if (gamma < (PI_f*6.312f/(2.0f*180.0f))) {
//		radiance=
//		return;
//	} // this allows to hit the sun with a path tracer


	if (direction[1] < 0.01f){
		Vector3 temp = direction;
		temp[1] = 0.01f;/*-temp[2];*/
		temp.normalize();
		thetaV = acosf(temp[1]);
	}
	else
		thetaV = acosf(direction[1]);

	float cosThetaS2 = cosf(thetaSunInRad); cosThetaS2*=cosThetaS2;

	x = zenith_xyY[0] * DistributionPerez(perez_x, cosThetaS2, thetaSunInRad, thetaV, gamma);
	y = zenith_xyY[1] * DistributionPerez(perez_y, cosThetaS2, thetaSunInRad, thetaV, gamma);
	Y = zenith_xyY[2] * DistributionPerez(perez_Y, cosThetaS2, thetaSunInRad, thetaV, gamma);

	const float M1 = (-1.3515f - 1.7703f*x + 5.9114f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
	const float M2 = (0.03f - 31.4424f*x + 30.0717f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
	int i = ray.lambda/10.0f - 38.0f;
	// FIXME: RiSunSky.C has Y* spect/spect.Y here!
	radiance = Y*(shader_S0_380_780[i] + M1*shader_S1_380_780[i] + M2*shader_S2_380_780[i]);
	radiance = fmax(radiance,0.0f);
	//assert(radiance>0.0f);
	//cout << "dir: " << ray.direction << "lambda: " << ray.lambda << "power: " << radiance << endl;
}

//void SunSky::getOvercastSkylight(const Vector3& dir, Vector3& radiance) const{
//
//	if(dir[1]>0.0f)
//		radiance= (Vector3(0.8,1,1)*((1.0f + 2.0f *dir[1])/3.0f)*100.0f);
//	else
//		radiance = Vector3(0,0,0);
//
//}

//typedef struct shader_daylight_t
//{
//  float theta_sun;
//  float turbidity;
//  float sun_XYZ[3];
//  float sun_rgb[3];
//  float sun_dir[3];
//  float u[3], v[3];
//  float sun_power_scale;
//  float zenith_xyY[3];
//  float perez_x[5];
//  float perez_y[5];
//  float perez_Y[5];
//  float sun_M1;
//  float sun_M2;
//} shader_daylight_t;



//void shader_daylight_compute_sun_XYZ(shader_daylight_t *s)
//{
//  //float sun[sizeof(sunSpecRad_380_750)/sizeof(double)]; // in float because of conversion into XYZ
//  // relative optical mass
//  double m = 1.0 / (cos(s->theta_sun) + 0.15*pow((93.885 - (180.0/M_PI)*(s->theta_sun)),-1.253));
//  double beta = 0.04608*(s->turbidity) + 0.04586;
//  double alpha = 1.3;
//  double l = 0.35;  // cm; ozone
//  double w = 2.0; // cm;
//  //fprintf(stderr, "\n m=%e", m);
//
//  int lambda_start = 38;
//  s->sun_XYZ[0] = s->sun_XYZ[1] = s->sun_XYZ[2] = 0.0f;
//  for (int lambda_nm10 = lambda_start; lambda_nm10 <= 75; lambda_nm10 += 1)
//  {
//    double tau_r, tau_a, tau_o, tau_g, tau_wa;
//    double lambda_um = lambda_nm10/100.0;  // lambda in um
//    int i = lambda_nm10 - lambda_start;
//    tau_r = exp(-m * 0.008735 * pow(lambda_um, -4.08)); // copied from RiSunConstants.C
//    tau_a = exp(-m * beta * pow(lambda_um, -alpha));
//    // if (lambda_nm10 >= 45 && lambda_nm10 <= 77)
//      tau_o = exp(-shader_daylight_k_o_450_770[lambda_nm10-lambda_start]*l*m);
//    // else tau_o = 1.0;
//    if (lambda_nm10 >= 76 && lambda_nm10 <= 77) // !!TODO: check: this seems to be always 1!!
//      tau_g = exp((-1.41*shader_daylight_k_g_760_770[lambda_nm10-76]*m)/pow(1.0+118.93*shader_daylight_k_g_760_770[lambda_nm10-76]*m, 0.45));
//    else tau_g = 1.0;
//    // if (lambda_nm10 >= 69 && lambda_nm10 <= 78)
//      tau_wa = exp((-0.2385*shader_daylight_k_wa_690_780[lambda_nm10-69]*w*m)/pow(1.0+20.07*shader_daylight_k_wa_690_780[lambda_nm10-69]*w*m, 0.45));
//    // else tau_wa = 1.0;
//
//    float sunAmpl = (float)(s->sun_power_scale*tau_r*tau_a*tau_o*tau_g*tau_wa*shader_daylight_sunSpecRad_380_750[lambda_nm10-lambda_start]);
//    shader_sun_power[i] = sunAmpl*38.0*20;
//    s->sun_XYZ[0] += sunAmpl * spectrum_x[i+2]; // /38.0;
//    s->sun_XYZ[1] += sunAmpl * spectrum_y[i+2]; // /38.0;
//    s->sun_XYZ[2] += sunAmpl * spectrum_z[i+2]; // /38.0;
//  }
//  const float x = s->sun_XYZ[0]/(s->sun_XYZ[0] + s->sun_XYZ[1] + s->sun_XYZ[2]);
//  const float y = s->sun_XYZ[1]/(s->sun_XYZ[0] + s->sun_XYZ[1] + s->sun_XYZ[2]);
//  s->sun_M1 = (-1.3515f - 1.7703f*x + 5.9114f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
//  s->sun_M2 = (0.03f - 31.4424f*x + 30.0717f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
//
//  spectrum_xyz_to_rgb(s->sun_XYZ, s->sun_rgb);
//#if 0
//  fprintf(stderr, "sun color XYZ: %f %f %f\n", s->sun_XYZ[0], s->sun_XYZ[1], s->sun_XYZ[2]);
//  fprintf(stderr, "sun color RGB: %f %f %f\n", s->sun_rgb[0], s->sun_rgb[1], s->sun_rgb[2]);
//#endif
//}

//int sky_daylight_init(FILE *f, void **data)
//{
//  shader_daylight_t *s = (shader_daylight_t *)malloc(sizeof(shader_daylight_t));
//  *data = s;
//
//  // const float deg2rad = M_PI/180.0f;
//  //const float sm = ?;
//
//  // float lat, lon, time, doy;
//  // if(fscanf(f, "%f %f %f %f %f", &lat, &lon, &time, &doy, &s->turbidity) < 5)
//  // {
//  //   fprintf(stderr, "[daylight] could not read all parameters! expecting:\n  lat lon time-of-day (decimal hours) day-of-year (1-365) turbidity (2-30)\n");
//  //   lat = 48.0f;
//  //   lon = 11.0f;
//  //   time = 9.0f;
//  //   doy = 255;
//  //   s->turbidity = 2.0f;
//  // }
//  // fscanf(f, "%*[^\n]\n");
//  // lat *= deg2rad;
//  // lon *= deg2rad;
//
//  // const float solar_time = time + 0.170f * sinf(4*M_PI*(doy-80)/373.0f) - 0.129f*sinf(2*M_PI*(doy-8)/355.0f);// + 12.0f*(sm - lon)/M_PI;
//  // const float decl = 0.4093*sinf(2*M_PI*(doy-81)/368);
//  // const float theta = M_PI/2.0f - asinf(sinf(lat)*sinf(decl) - cos(lat)*cos(decl)*cosf(M_PI*solar_time/12.0f));
//  // const float phi = atanf(-cosf(decl)*sinf(M_PI*solar_time/12.0f)/(cosf(lat)*sinf(decl) - sinf(lat)*cosf(decl)*cosf(M_PI*solar_time/12.0f)));
//
//  // s->sun_dir[2] = - cosf(theta);
//  // s->sun_dir[0] = - sinf(theta)*sinf(phi);
//  // s->sun_dir[1] = - sinf(theta)*cosf(phi);
//  // s->theta_sun = theta;
//  if(fscanf(f, "%f %f %f %f", s->sun_dir, s->sun_dir+1, s->sun_dir+2, &s->turbidity) < 4)
//  {
//    fprintf(stderr, "[daylight] could not read all parameters! expecting:\n  <sundir x y z> turbidity\n");
//    s->sun_dir[0] = s->sun_dir[1] = s->sun_dir[2] = - sqrtf(3.0f);
//    s->turbidity = 2.0f;
//  }
//  fscanf(f, "%*[^\n]\n");
//  s->turbidity = fmaxf(2.0, fminf(s->turbidity, 10.0));
//  normalise(s->sun_dir);
//  s->theta_sun = acosf(fmaxf(0.0f, fmaxf(1.0f, s->sun_dir[2])));
//  get_perpendicular(s->sun_dir, s->u);
//  normalise(s->u);
//  crossproduct(s->sun_dir, s->u, s->v);
//  normalise(s->v);
//
//  s->sun_power_scale = 400.f/(s->turbidity*s->turbidity); // 400
//
//  // precomputed the values for the sky-color model
//  float theta = s->theta_sun;
//  float theta2 = theta * theta;
//  float theta3 = theta * theta2;
//  float t = s->turbidity;
//  float t2 = t * t;
//
//  s->zenith_xyY[0] = ( 0.00166*theta3 - 0.00375*theta2 + 0.00209*theta + 0) * t2 +
//    (-0.02903*theta3 + 0.06377*theta2 - 0.03203*theta + 0.00394) * t +
//    ( 0.11693*theta3 - 0.21196*theta2 + 0.06052*theta + 0.25886);
//  s->zenith_xyY[1] = ( 0.00275*theta3 - 0.00610*theta2 + 0.00317*theta + 0) * t2 +
//    (-0.04214*theta3 + 0.08970*theta2 - 0.04153*theta + 0.00516) * t +
//    ( 0.15346*theta3 - 0.26756*theta2 + 0.06670*theta + 0.26688);
//  s->zenith_xyY[2] = (4.0453 * t - 4.9710) * tan((4.0/9.0 - t/120.0) * (M_PI - 2*theta)) - 0.2155 * t + 2.4192;
//
//  // the constants are from "A Practical Analytical model for Daylight" Appendix A (p.22)
//  s->perez_Y[0] =  0.1787f * t - 1.4630f; s->perez_Y[1] = -0.3554f * t + 0.4275f;
//  s->perez_Y[2] = -0.0227f * t + 5.3251f; s->perez_Y[3] =  0.1206f * t - 2.5771f;
//  s->perez_Y[4] = -0.0679f * t + 0.3703f;
//  s->perez_x[0] = -0.0193f * t - 0.2592f; s->perez_x[1] = -0.0665f * t + 0.0008f;
//  s->perez_x[2] = -0.0004f * t + 0.2125f; s->perez_x[3] = -0.0641f * t - 0.8989f;
//  s->perez_x[4] = -0.0033f * t + 0.0452f;
//  s->perez_y[0] = -0.0167f * t - 0.2608f; s->perez_y[1] = -0.0950f * t + 0.0092f;
//  s->perez_y[2] = -0.0079f * t + 0.2102f; s->perez_y[3] = -0.0441f * t - 1.6537f;
//  s->perez_y[4] = -0.0109f * t + 0.0529f;
//
//  shader_daylight_compute_sun_XYZ(s);
//  return 0;
//}



//float sky_daylight(const rayhit_t* hit, const float* dir, void* data)
//{
//  shader_daylight_t* s = (shader_daylight_t *)data;
//  float thetaV;        // the angle about surface normal
//  float gamma;         // the angle between direction and sun
//  float x,y,Y;
//
//  gamma = acosf(fminf(-dotproduct(dir, s->sun_dir), 1.0f));
//
//  if (dir[2] < 0.01f)
//  {
//    float temp[3] = {dir[0], dir[1], dir[2]};
//    temp[2] = 0.01f;/*-temp[2];*/ normalise(temp);
//    thetaV = acosf(temp[2]);
//  }
//  else thetaV = acosf(dir[2]);
//
//  float cosThetaS2 = cosf(s->theta_sun); cosThetaS2*=cosThetaS2;
//  x = s->zenith_xyY[0] * shader_daylight_DistributionPerez(s->perez_x, cosThetaS2, s->theta_sun, thetaV, gamma);
//  y = s->zenith_xyY[1] * shader_daylight_DistributionPerez(s->perez_y, cosThetaS2, s->theta_sun, thetaV, gamma);
//  Y = s->zenith_xyY[2] * shader_daylight_DistributionPerez(s->perez_Y, cosThetaS2, s->theta_sun, thetaV, gamma);
//
//  const float M1 = (-1.3515f - 1.7703f*x + 5.9114f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
//  const float M2 = (0.03f - 31.4424f*x + 30.0717f*y)/(0.0241f + 0.2562f*x - 0.7341f*y);
//  int i = hit->lambda/10.0f - 38.0f;
//  // FIXME: RiSunSky.C has Y* spect/spect.Y here!
//  const float power = Y*(shader_S0_380_780[i] + M1*shader_S1_380_780[i] + M2*shader_S2_380_780[i]);
//  //assert(isfinite(power));
//  return power;
//}

//float sky_daylight_add_sun(const rayhit_t *hit, const ray_t *ray, void *data)
//{
//  shader_daylight_t* s = (shader_daylight_t *)data;
//  const float gamma = acosf(fminf(-dotproduct(ray->dir, s->sun_dir), 1.0f));
//  const float sun_rad = 0.0088f;//0.0044f;//sqrtf(0.00002f);// 0.014f;
//  //const float sun_rad = (M_PI*6.312f/(2.0f*180.0f));
//  if (gamma < sun_rad)// (M_PI*6.312f/(2.0f*180.0f)))
//  //if (gamma < (M_PI*6.312f/(2.0f*180.0f)))
//  {
//    int i = hit->lambda/10.0f - 38.0f;
//    // return s->sun_XYZ[1]*(shader_S0_380_780[i] + s->sun_M1*shader_S1_380_780[i] + s->sun_M2*shader_S2_380_780[i]);
//    return shader_sun_power[i];
//    // return spectrum_rgb_to_p(hit->lambda, s->sun_rgb);
//  }
//  return 0.0f;
//}

//float sky_daylight_sample_sun(rayhit_t *hit, ray_t *ray, const float x1, const float x2, float *pdf, void *data)
//{
//  shader_daylight_t* s = (shader_daylight_t *)data;
//  // sample solid angle of sun (approximate as disc)
//  const float sun_rad = 0.0088f;//0.0044f;//sqrtf(0.00002f);// 0.014f;
//  const float sin_max_gamma = sun_rad;//sinf(M_PI*6.312f/(2.0f*180.0f));
//  const float r = sqrtf(x1)*sin_max_gamma;
//  const float inv_p = M_PI*sin_max_gamma*sin_max_gamma;
//  const float cos_max_gamma = sqrtf(1.0f - sin_max_gamma*sin_max_gamma);
//  for(int k=0;k<3;k++) ray->dir[k] = -cos_max_gamma*s->sun_dir[k] + sinf(2*M_PI*x2)*r*s->u[k] + cosf(2*M_PI*x2)*r*s->v[k];
//  //for(int k=0;k<3;k++) ray->dir[k] = - s->sun_dir[k];
//  int i = hit->lambda/10.0f - 38.0f;
//  const float power = shader_sun_power[i];
//  // const float power = s->sun_XYZ[1]*(shader_S0_380_780[i] + s->sun_M1*shader_S1_380_780[i] + s->sun_M2*shader_S2_380_780[i]);
//  // const float power = spectrum_rgb_to_p(hit->lambda, s->sun_rgb);
//  if(pdf) *pdf = 1.0f/inv_p;
//  return inv_p*power;
//}

//float sky_daylight_sun_pdf(const rayhit_t *hit, const ray_t *ray, void *data)
//{
//  shader_daylight_t* s = (shader_daylight_t *)data;
//  // sample solid angle of sun (approximate as disc)
//  const float sun_rad = 0.0088f;//0.0044f;//sqrtf(0.00002f);// 0.014f;
//  const float sin_max_gamma = sun_rad;//sinf(M_PI*6.312f/(2.0f*180.0f));
//  const float gamma = acosf(fminf(-dotproduct(ray->dir, s->sun_dir), 1.0f));
//  if (gamma < sun_rad) return 1.0f/(M_PI*sin_max_gamma*sin_max_gamma);
//  return 0.0f;
//}
//
//float shader_add_sun_d(const rayhit_t *hit, const ray_t *ray, void *data)
//{
//  return 0.0f;
//}
//
//float shader_sample_sun_d(rayhit_t *hit, ray_t *ray, const float x1, const float x2, float *pdf, void *data)
//{
//  if(pdf) *pdf = 1.0f;
//  return 0.0f;
//}
//
//float shader_sun_pdf_d(const rayhit_t *hit, const ray_t *ray, void *data)
//{
//  return 1.0f;
//}
//
//float shader_add_sun(const rayhit_t *hit, const ray_t *ray)
//{
//  return rt.shader->skyshader.add_sun(hit, ray, rt.shader->skyshader.data);
//}
//
//float shader_sample_sun(rayhit_t *hit, ray_t *ray, const float x1, const float x2, float *pdf)
//{
//  const float power = rt.shader->skyshader.sample_sun(hit, ray, x1, x2, pdf, rt.shader->skyshader.data);
//  //assert(isfinite(power));
//  if(hit->prim >= 0 && prim_offset_ray(hit, ray)) return 0.0f;
//  for(int k=0;k<3;k++) ray->invdir[k] = 1.0f/ray->dir[k];
//  return power;
//}
//
//float shader_sun_pdf(const rayhit_t *hit, const ray_t *ray)
//{
//  return rt.shader->skyshader.sun_pdf(hit, ray, rt.shader->skyshader.data);
//}
