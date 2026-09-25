#ifndef CAMERA_HH
#define CAMERA_HH

#include "../utils/point.hh"
#include "../utils/vector.hh"
#include "../light/ray.hh"

namespace isim {

    class Camera {
        public:
            Point3 P; // spotted point
            Point3 C; // center
            Vector3 up;

            float alpha;
            float gamma;
            float zmin;

            Camera(Point3 P, Point3 C, Vector3 up, float alpha, float gamma, float zmin) : P(P), C(C), up(up), alpha(alpha), gamma(gamma), zmin(zmin) {}
            Ray cast_ray(int x, int y, int W, int H, float dx = 0.f, float dy = 0.f) const;
            void setPolarCoords(float radius, float theta, float phi=0);
    };

}

#endif // CAMERA_HH