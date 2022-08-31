#define _USE_MATH_DEFINES
#include "trace.H"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
//#include <getopt.h>
#ifdef __APPLE__
#define MAX std::numeric_limits<double>::max()
#else
#define MAX DBL_MAX
#endif

// return the determinant of the matrix with columns a, b, c.
double det(const SlVector3& a, const SlVector3& b, const SlVector3& c) {
    return a[0] * (b[1] * c[2] - c[1] * b[2]) +
        b[0] * (c[1] * a[2] - a[1] * c[2]) +
        c[0] * (a[1] * b[2] - b[1] * a[2]);
}

inline double sqr(double x) { return x * x; }

bool Triangle::intersect(const Ray& r, double t0, double t1, HitRecord& hr) const {

    // Step 1 Ray-triangle test***************************************************************************
    SlVector3 nor = cross(this->a - this->b, this->a - this->c);
    normalize(nor);
    double x = dot(r.d, nor);
    if (x != 0) {
        double t = (dot(nor, this->a) - dot(nor, r.e)) / (x);
        SlVector3 p = r.e + t * r.d;
        double cross1 = dot(cross(this->b - this->a, p - this->a), cross(this->b - this->a, this->c - this->a));
        double cross2 = dot(cross(this->c - this->a, p - this->a), cross(this->c - this->a, this->b - this->a));
        double cross3 = dot(cross(this->c - this->b, p - this->b), cross(this->c - this->b, this->a - this->b));
        if (dot(cross(this->b - this->a, p - this->a), cross(this->b - this->a, this->c - this->a)) > 0 && dot(cross(this->c - this->a, p - this->a), cross(this->c - this->a, this->b - this->a)) > 0 && dot(cross(this->c - this->b, p - this->b), cross(this->c - this->b, this->a - this->b)) > 0) {
            if (t > t0 && t < t1) {
                //ÉèÖÃHitRecord
                hr.t = t;
                //hr.alpha = 1 - beta - gemma;
                //hr.beta = beta;
                //hr.gamma = gemma;
                hr.p = p;
                hr.n = nor;
                hr.e = 0.0 - r.d;
                return true;
            }
        }
    }
    return false;
}

bool TrianglePatch::intersect(const Ray& r, double t0, double t1, HitRecord& hr) const {
    bool temp = Triangle::intersect(r, t0, t1, hr);
    if (temp) {
        hr.n = hr.alpha * n1 + hr.beta * n2 + hr.gamma * n3;
        normalize(hr.n);
        return true;
    }
    return false;
}


bool Sphere::intersect(const Ray& r, double t0, double t1, HitRecord& hr) const {

    // Step 1 Sphere-triangle test***************************************************************************
    double t;
    double flag = (dot(r.d, r.e) - dot(r.d, this->c)) * (dot(r.d, r.e) - dot(r.d, this->c)) - dot(r.d, r.d) * (dot((r.e - this->c), (r.e - this->c)) - this->rad * this->rad);
    if (flag > 0) {
        t = -(sqrt(flag) + 2 * dot(r.d, r.e - this->c) / (2 * dot(r.d, r.d)));
        if (t > t0 && t < t1) {
            //ÉèÖÃHitRecord
            hr.t = t;
            hr.p = r.e + t * r.d;
            hr.n = hr.p - this->c;
            hr.e = 0.0 - r.d;
            normalize(hr.n);
            return true;
        }
    }
    return false;
}




Tracer::Tracer(const std::string& fname) {
    std::ifstream in(fname.c_str(), std::ios_base::in);
    std::string line;
    char ch;
    Fill fill;
    bool coloredlights = false;
    while (in) {
        getline(in, line);
        switch (line[0]) {
        case 'b': {
            std::stringstream ss(line);
            ss >> ch >> bcolor[0] >> bcolor[1] >> bcolor[2];
            break;
        }

        case 'v': {
            getline(in, line);
            std::string junk;
            std::stringstream fromss(line);
            fromss >> junk >> eye[0] >> eye[1] >> eye[2];

            getline(in, line);
            std::stringstream atss(line);
            atss >> junk >> at[0] >> at[1] >> at[2];

            getline(in, line);
            std::stringstream upss(line);
            upss >> junk >> up[0] >> up[1] >> up[2];

            getline(in, line);
            std::stringstream angless(line);
            angless >> junk >> angle;

            getline(in, line);
            std::stringstream hitherss(line);
            hitherss >> junk >> hither;

            getline(in, line);
            std::stringstream resolutionss(line);
            resolutionss >> junk >> res[0] >> res[1];
            break;
        }

        case 'p': {
            bool patch = false;
            std::stringstream ssn(line);
            unsigned int nverts;
            if (line[1] == 'p') {
                patch = true;
                ssn >> ch;
            }
            ssn >> ch >> nverts;
            std::vector<SlVector3> vertices;
            std::vector<SlVector3> normals;
            for (unsigned int i = 0; i < nverts; i++) {
                getline(in, line);
                std::stringstream ss(line);
                SlVector3 v, n;
                if (patch) ss >> v[0] >> v[1] >> v[2] >> n[0] >> n[1] >> n[2];
                else ss >> v[0] >> v[1] >> v[2];
                vertices.push_back(v);
                normals.push_back(n);
            }
            bool makeTriangles = false;
            if (vertices.size() == 3) {
                if (patch) {
                    surfaces.push_back(std::pair<Surface*, Fill>(new TrianglePatch(vertices[0], vertices[1], vertices[2],
                        normals[0], normals[1], normals[2]), fill));
                }
                else {
                    surfaces.push_back(std::pair<Surface*, Fill>(new Triangle(vertices[0], vertices[1], vertices[2]), fill));
                }
            }
            else if (vertices.size() == 4) {
                SlVector3 n0 = cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
                SlVector3 n1 = cross(vertices[2] - vertices[1], vertices[3] - vertices[1]);
                SlVector3 n2 = cross(vertices[3] - vertices[2], vertices[0] - vertices[2]);
                SlVector3 n3 = cross(vertices[0] - vertices[3], vertices[1] - vertices[3]);
                if (dot(n0, n1) > 0 && dot(n0, n2) > 0 && dot(n0, n3) > 0) {
                    makeTriangles = true;
                    if (patch) {
                        surfaces.push_back(std::pair<Surface*, Fill>(new TrianglePatch(vertices[0], vertices[1], vertices[2],
                            normals[0], normals[1], normals[2]), fill));
                        surfaces.push_back(std::pair<Surface*, Fill>(new TrianglePatch(vertices[0], vertices[2], vertices[3],
                            normals[0], normals[2], normals[3]), fill));
                    }
                    else {
                        surfaces.push_back(std::pair<Surface*, Fill>(new Triangle(vertices[0], vertices[1], vertices[2]), fill));
                        surfaces.push_back(std::pair<Surface*, Fill>(new Triangle(vertices[0], vertices[2], vertices[3]), fill));
                    }
                }
                if (!makeTriangles) {
                    std::cerr << "I didn't make triangles.  Poly not flat or more than quad.\n";
                }
            }
            break;
        }

        case 's': {
            std::stringstream ss(line);
            SlVector3 c;
            double r;
            ss >> ch >> c[0] >> c[1] >> c[2] >> r;
            surfaces.push_back(std::pair<Surface*, Fill>(new Sphere(c, r), fill));
            break;
        }

        case 'f': {
            std::stringstream ss(line);
            ss >> ch >> fill.color[0] >> fill.color[1] >> fill.color[2] >> fill.kd >> fill.ks >> fill.shine >> fill.t >> fill.ior;
            break;
        }

        case 'l': {
            std::stringstream ss(line);
            Light l;
            ss >> ch >> l.p[0] >> l.p[1] >> l.p[2];
            if (!ss.eof()) {
                ss >> l.c[0] >> l.c[1] >> l.c[2];
                coloredlights = true;
            }
            lights.push_back(l);
            break;
        }

        default:
            break;
        }
    }
    if (!coloredlights) for (unsigned int i = 0; i < lights.size(); i++) lights[i].c = 1.0 / sqrt(lights.size());
    im = new SlVector3[res[0] * res[1]];
    shadowbias = 1e-6;
    samples = 1;
    aperture = 0.0;
}

Tracer::~Tracer() {
    if (im) delete[] im;
    for (unsigned int i = 0; i < surfaces.size(); i++) delete surfaces[i].first;
}


SlVector3 Tracer::shade(const HitRecord& hr, double c) const {
    if (color) return hr.f.color;

    SlVector3 color(0.0);
    SlVector3 newColor(0.0);
    HitRecord dummy;

    for (unsigned int i = 0; i < lights.size(); i++) {
        const Light& light = lights[i];
        bool shadow = false;

        // Step 3 Check for shadows here
        for (int j = 0; j < this->lights.size(); j++) {
            SlVector3 dir = this->lights[j].p - hr.p;
            normalize(dir);
            Ray r = Ray(hr.p, dir);
            for (int a = 0; a < this->surfaces.size(); a++) {
                shadow = this->surfaces[a].first->intersect(r, shadowbias, mag(lights[i].p - hr.p), dummy);
                if (shadow) {
                    break;
                }
            }

            if (!shadow) {

                // Step 2 do shading here
                double spetacular = 1.0;
                double diffuse = 0.0;
                SlVector3 r = getSym(hr.n, dir);

                diffuse = (dot(dir, hr.n) > 0.0) ? dot(dir, hr.n) : 0.0;
                spetacular = (dot(r, hr.e) > 0.0) ? pow(dot(r, hr.e), hr.f.shine) : 0.0;
                //Original Phong BRDF
                color += hr.f.kd * hr.f.color * light.c * diffuse + hr.f.ks * hr.f.color * light.c * spetacular;
            }
        }

    }


    // Step 4 Add code for computing reflection color here
    // Step 5 Add code for computing refraction color here
    SlVector3 nextd = getSym(hr.n, hr.e);
    normalize(nextd);
    Ray nextr = Ray(hr.p, nextd);
    c++;
    SlVector3 reflection = trace(nextr, shadowbias, MAX, c);
    color += reflection * hr.f.ks * hr.f.color * dot(nextr.d, hr.n) + reflection * hr.f.ks * hr.f.color;
    return newColor;
}

SlVector3 Tracer::trace(const Ray& r, double t0, double t1, double c) const {
    if (c == this->maxraydepth) {
        return { 0.0, 0.0, 0.0 };
    }
    HitRecord tempHR;
    HitRecord hr;
    hr.t = MAX;
    SlVector3 color(bcolor);
    SlVector3 selfColor = SlVector3(0.0);
    bool hit = false;

    // Step 1 See what a ray hits  
    for (int i = 0; i < this->surfaces.size(); i++) {
        if (this->surfaces[i].first->intersect(r, t0, t1, tempHR)) {
            hit = true;
            if (tempHR.t < hr.t) {
                hr = tempHR;
                hr.f = this->surfaces[i].second;
            }
        }
    }

    if (hit) {
        selfColor += shade(hr, c);
        return selfColor;
    }
    else if (c != 0) {
        return { 0.0, 0.0, 0.0 };
    }
    return color;
}

void Tracer::traceImage() {
    // set up coordinate system
    SlVector3 w = eye - at;
    w /= mag(w);
    SlVector3 u = cross(up, w);
    normalize(u);
    SlVector3 v = cross(w, u);
    normalize(v);

    double d = mag(eye - at);
    double h = tan((M_PI / 180.0) * (angle / 2.0)) * d;
    double l = -h;
    double r = h;
    double b = h;
    double t = -h;

    SlVector3* pixel = im;

    for (unsigned int j = 0; j < res[1]; j++) {
        std::cout << j << std::endl;
        for (unsigned int i = 0; i < res[0]; i++, pixel++) {
            SlVector3 result(0.0, 0.0, 0.0);

            for (int k = 0; k < samples; k++) {

                double rx = 1.1 * rand() / RAND_MAX;
                double ry = 1.1 * rand() / RAND_MAX;

                double x = l + (r - l) * (i + rx) / res[0];
                double y = b + (t - b) * (j + ry) / res[1];
                SlVector3 dir = -d * w + x * u + y * v;

                Ray r(eye, dir);
                normalize(r.d);

                result += trace(r, hither, MAX, 0.0);
            }
            (*pixel) = result / samples * (result + 1);
        }
    }
}

void Tracer::writeImage(const std::string& fname) {
#ifdef __APPLE__
    std::ofstream out(fname, std::ios::out | std::ios::binary);
#else
    std::ofstream out(fname.c_str(), std::ios_base::binary);
#endif
    out << "P6" << "\n" << res[0] << " " << res[1] << "\n" << 255 << "\n";
    SlVector3* pixel = im;
    char val;
    for (unsigned int i = 0; i < res[0] * res[1]; i++, pixel++) {
        val = (unsigned char)(std::min(1.0, std::max(0.0, (*pixel)[0])) * 255.0);
        out.write(&val, sizeof(unsigned char));
        val = (unsigned char)(std::min(1.0, std::max(0.0, (*pixel)[1])) * 255.0);
        out.write(&val, sizeof(unsigned char));
        val = (unsigned char)(std::min(1.0, std::max(0.0, (*pixel)[2])) * 255.0);
        out.write(&val, sizeof(unsigned char));
    }
    out.close();
}


int main(int argc, char* argv[]) {
    //int c;
    double aperture = 0.0;
    int samples = 1;
    int maxraydepth = 2;
    bool color = false;
    /*while ((c = getopt(argc, argv, "a:s:d:c")) != -1) {
        switch(c) {
            case 'a':
            aperture = atof(optarg);
            break;
            case 's':
            samples = atoi(optarg);
            break;
            case 'c':
            color = true;
            break;
            case 'd':
            maxraydepth = atoi(optarg);
            break;
            default:
            abort();
        }
    }

    if (argc-optind != 2) {
        std::cout<<"usage: trace [opts] input.nff output.ppm"<<std::endl;
        for (int i=0; i < argc; i++) std::cout<<argv[i]<<std::endl;
        exit(0);
    }*/

    Tracer tracer("C:\\assignment-04\\InputFiles\\rings.nnf");
    tracer.aperture = aperture;
    tracer.samples = samples;
    tracer.color = color;
    tracer.maxraydepth = maxraydepth;
    tracer.traceImage();
    tracer.writeImage("C:\\assignment-04\\InputFiles\\rings.ppm");
};
