#ifndef ENGINE_HH
#define ENGINE_HH

#include <vector>
#include "scene.hh"
#include "../utils/image.hh"
#include "../utils/color.hh"
#include "../objects/box.hh"

namespace isim {

    std::vector<Color> raytrace(const Scene& scene, const Image& img);
    Color trace(const Ray& ray, const Scene& scene, int depth);
    float marchToLight(const Point3& pos, const Light* light, const Box* box, const smoke::FluidGrid3D& smoke,
        const Scene& scene, int numSteps = 16);
}

#endif // ENGINE_HH