#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include "object.hh"

namespace isim {

    class Triangle : public Object {
    public:
        Triangle(const Point3& A, const Point3& B, const Point3& C, std::shared_ptr<TextureMaterial> texture)
            : Object(std::move(texture)), A(A), B(B), C(C) {}

        std::optional<float> intersect(const Ray& ray) const override;
        Vector3 normal_at(const Point3& point) const override;
        ObjectType type() const override { return ObjectType::Triangle; }


    private:
        Point3 A;
        Point3 B;
        Point3 C;
    };

}

#endif /* TRIANGLE_HH */