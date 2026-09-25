#ifndef SPHERE_HH
#define SPHERE_HH

#include "object.hh"

namespace isim {

    class Sphere : public Object {
    public:
        Sphere(const Point3& center, float radius, std::shared_ptr<TextureMaterial> texture)
            : Object(std::move(texture)), center(center), radius(radius) {}

        std::optional<float> intersect(const Ray& ray) const override;
        Vector3 normal_at(const Point3& point) const override;
        ObjectType type() const override { return ObjectType::Sphere; }


    // private:
        Point3 center;
        float radius;
    };

}

#endif /* SPHERE_HH */