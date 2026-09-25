#ifndef PLANE_HH
#define PLANE_HH

#include "object.hh"
#include "../utils/point.hh"
#include "../utils/vector.hh"

namespace isim {

    class Plane : public Object {
    public:
        Point3  point; 
        Vector3 normal;

        Plane(const Point3& point, const Vector3& normal,
              std::shared_ptr<TextureMaterial> texture)
            : Object(std::move(texture))
            , point(point)
            , normal(normal.normalized())
        {}

        std::optional<float> intersect(const Ray& ray) const override;
        Vector3 normal_at(const Point3& p) const override;
        ObjectType type() const override { return ObjectType::Plane; }
    };

}

#endif // PLANE_HH