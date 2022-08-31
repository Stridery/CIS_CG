#ifndef TRACE_H
#define TRACE_H

#include "slVector.H"
#include <vector>

class Ray {  //光线向量
public:
    SlVector3 e;
    SlVector3 d;
    int depth;
    Ray(const SlVector3 &_e, const SlVector3 &_d, int _depth = 0) : e(_e), d(_d), depth(_depth) {};
};

class Fill {  //填充
public: 
    SlVector3 color;
    double kd, ks, shine, t, ior;
};

class HitRecord {  //光照到过的东西
public:
    double t, alpha, beta, gamma;
    SlVector3 p, n, e, v;
    Fill f;
    int raydepth;
};

class Light {  
public:
    SlVector3 p, c;
};

class Surface {
public:
    virtual bool intersect(const Ray &r, double t0, double t1, HitRecord &hr) const = 0;
    virtual ~Surface() {};
};

class Triangle : public Surface {  
    SlVector3 a,b,c;
public:
    Triangle(const SlVector3 &_a, const SlVector3 &_b, const SlVector3 &_c) : a(_a), b(_b), c(_c) {};
    virtual bool intersect(const Ray &r, double t0, double t1, HitRecord &hr) const;
};

class TrianglePatch : public Triangle {  //
    SlVector3 n1, n2, n3;
public: 
    TrianglePatch(const SlVector3 &_a, const SlVector3 &_b, const SlVector3 &_c,
    const SlVector3 &_n1, const SlVector3 &_n2, const SlVector3 &_n3) 
        : Triangle(_a,_b,_c), n1(_n1), n2(_n2), n3(_n3) {};
    virtual bool intersect(const Ray &r, double t0, double t1, HitRecord &hr) const;
};

class Sphere : public Surface {
    SlVector3 c;
    double rad;
public:
    Sphere(const SlVector3 &_c, double _r) : c(_c), rad(_r) {};
    bool intersect(const Ray &r, double t0, double t1, HitRecord &hr) const;
};

class Tracer {
    SlVector3 bcolor/*背景颜色*/, eye/*眼睛位置*/, at/*看的方向*/, up/*抬头高度*/;
    double angle, hither;
    unsigned int res[2];
    std::vector<std::pair<Surface *, Fill> > surfaces;
    std::vector<Light> lights;
    double shadowbias;
  
    SlVector3 *im;
public:
    Tracer(const std::string &fname);
    ~Tracer();
    void traceImage();  
    SlVector3 trace(const Ray &ray, double t0, double t1, double c) const;
    SlVector3 shade(const HitRecord &hr, double c) const;
    void writeImage(const std::string &fname);

    bool color;
    int samples;
    double aperture;
    int maxraydepth;
};
	
#endif
