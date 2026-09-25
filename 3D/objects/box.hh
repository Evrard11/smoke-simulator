#ifndef BOX_HH
#define BOX_HH

#include "object.hh"
#include <memory>

namespace isim {

    class Box : public Object {
    public:
        Box(const Point3& min,
            const Point3& max,
            std::shared_ptr<TextureMaterial> texture);

        std::optional<float> intersect(const Ray& ray) const override;
        Vector3 normal_at(const Point3& point) const override;

        std::optional<std::array<float, 2>> intersect_box(const Ray& ray) const;
        ObjectType type() const override { return ObjectType::Box; }

        Point3 min;
        Point3 max;
    };

}

#endif /* BOX_HH */