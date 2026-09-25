#ifndef OBJECT_HH
#define OBJECT_HH

#include <optional>
#include <memory>
#include "../utils/vector.hh"
#include "../utils/texture.hh"
#include "../light/ray.hh"

namespace isim {

    enum class ObjectType { Sphere, Plane, Triangle, Box };

    class Object {
    public:
        Object(std::shared_ptr<TextureMaterial> texture)
            : texture(std::move(texture)) {}
        
        virtual ~Object() = default;

        // Return if the ray intersects the object, and if so, at what distance
        virtual std::optional<float> intersect(const Ray& ray) const = 0;

        // Return the normal vector at a given point on the surface of the object
        virtual Vector3 normal_at(const Point3& point) const = 0;

        // Texture
        virtual MaterialParams material_at(const Ray& ray, const Hit& hit, const Scene* scene=nullptr) const {
            return texture->get_params(ray, hit, *scene);
        }

        // return the type of the given object
        virtual ObjectType type() const = 0;

        protected:
        std::shared_ptr<TextureMaterial> texture;
    };

}

#endif /* OBJECT_HH */