#include "scene.hh"
#include "../objects/box.hh"

namespace isim {

    std::optional<Hit> Scene::closest_intersection(const Ray& ray, float t_min, bool noBox) const {
        float closest_t = std::numeric_limits<float>::max();
        float tmax = -1.f; // used for volumetric
        Object* closest_obj = nullptr;

        for (Object* obj : objects) {
            if (noBox && dynamic_cast<Box*>(obj)) continue;

            auto t = obj->intersect(ray);
            if (t && *t > t_min && *t < closest_t) {
                closest_t   = *t;
                closest_obj = obj;
            }
        }

        if (closest_obj)
            return Hit{ closest_t, closest_obj, dynamic_cast<Box*>(closest_obj) ? tmax : -1.f };
        return std::nullopt;
    }

}