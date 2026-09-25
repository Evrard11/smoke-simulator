#ifndef SCENE_HH
#define SCENE_HH

#include <vector>
#include <optional>
#include "../camera/camera.hh"
#include "../objects/object.hh"
#include "../light/light.hh"
#include "../light/ray.hh"
#include "../utils/Hit.h"

namespace isim {

    class Scene {
    public:
        Camera               cam;
        std::vector<Object*> objects;
        std::vector<Light*>  lights; 

        Scene(Camera cam)
            : cam(cam), objects({}), lights({}) {}

        Scene(Camera cam, std::vector<Object*> objects, std::vector<Light*> lights)
            : cam(cam), objects(std::move(objects)), lights(std::move(lights)) {}

        std::optional<Hit> closest_intersection(const Ray& ray, float t_min = 1e-4f, bool noBox=false) const;
    };

}

#endif /* SCENE_HH */