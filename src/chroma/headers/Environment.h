#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

class Ray;

class Environment {

public:

    Environment() { };


    virtual ~Environment() { };

    virtual void getEnvironmentLight(const Ray &ray, float &radiance) const = 0;
};

#endif /* ENVIRONMENT_H_ */
