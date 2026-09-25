#include "engine.hh"
#include "../objects/box.hh"
#include "../utils/Hit.h"
#include <random>

#define BACKGROUND_COLOR Color(162.f/255, 210.f/255, 1) // (0.2f, 0.2f, 0.5f);

namespace isim {

    static const int MAX_DEPTH = 5;
    static const int SAMPLES = 16;

    Color shade(const Point3& hit_point,
                const Vector3& normal,
                const Vector3& view_dir,
                const MaterialParams& mat,
                const Scene& scene)
    {
        Color result(0.f, 0.f, 0.f);

        for (Light* light : scene.lights) {
            Vector3 L = light->direction_from(hit_point).normalized();
            Color Il = light->intensity_at(hit_point);
            float dist = light->distance_from(hit_point);

            // shadow ray
            Ray shadow_ray(hit_point, L);
            auto shadow_hit = scene.closest_intersection(shadow_ray, 1e-2f, true);

            // object between point and light
            if (shadow_hit && shadow_hit->t1 < dist)
                continue;

            // Diffuse
            float diff = std::max(0.f, normal.dot(L));
            result += mat.kd * Il * diff;

            // Specular
            Vector3 H = (L + view_dir).normalized();
            float spec = std::pow(std::max(0.f, normal.dot(H)), mat.ns);
            result += mat.ks * Il * spec;
        }

        // Ambient
        result += mat.kd * Color(0.1f, 0.1f, 0.1f);
        return result;
    }

    float marchToLight(const Point3& pos, const Light* light, const Box* box, const smoke::FluidGrid3D& smoke,
        const Scene& scene, int numSteps) {
        Vector3 L = light->direction_from(pos).normalized();
        float distToLight = light->distance_from(pos);

        Ray lightRay(pos, L);

        auto shadow_hit = scene.closest_intersection(lightRay, 1e-2f);
        if (!shadow_hit || shadow_hit->obj->type() != ObjectType::Box)
            return 0.f;

        float tMax = std::min(shadow_hit.value().t2, distToLight);
        float stepSize = tMax / numSteps;
        float totalDensity = 0.f;

        for (int i = 0; i < numSteps; i++) {
            float t = (i + 0.5f) * stepSize;
            Point3 samplePos = pos + L * t;

            Vector3 uvw = (samplePos - box->min) / (box->max - box->min);
            if (uvw.x < 0 || uvw.x > 1 || uvw.y < 0 || uvw.y > 1 || uvw.z < 0 || uvw.z > 1)
                break;

            float px = uvw.x * (smoke.cellCountX - 1);
            float py = uvw.y * (smoke.cellCountY - 1);
            float pz = uvw.z * (smoke.cellCountZ - 1);

            totalDensity += smoke.SampleTrilinear(smoke.densityMap,
                smoke.cellCountX, smoke.cellCountY, smoke.cellCountZ, px, py, pz) * stepSize;
        }
        return totalDensity;
    }

    Color get_color_result(const Ray& ray, const Hit& hit, const Scene& scene, int depth) {
        Point3 p = ray.at(hit.t1);
        Vector3 normal = hit.obj->normal_at(p);
        if (normal.dot(ray.direction) > 0.f) // backface culling
            normal = -normal;
        Point3 p_offset = p + normal * 1e-3f;
        Vector3 view_dir = (-ray.direction).normalized();

        MaterialParams mat = hit.obj->material_at(ray, hit);

        // Local shading
        Color local = shade(p_offset, normal, view_dir, mat, scene);

        // Reflection
        float cos_i = ray.direction.dot(normal);
        Vector3 reflect_dir = (ray.direction - normal * 2.f * cos_i).normalized();
        Ray reflect_ray(p_offset, reflect_dir);

        Color reflected = trace(reflect_ray, scene, depth - 1);

        // kr from texture
        return local * (1.f - mat.kr) + reflected * mat.kr;
    }

    Color trace(const Ray& ray, const Scene& scene, int depth) {
        if (depth <= 0)
            return Color(0.f, 0.f, 0.f);

        auto hit = scene.closest_intersection(ray);

        if (!hit)
            return BACKGROUND_COLOR;

        if (hit->obj->type() == ObjectType::Box) {
            MaterialParams mat = hit->obj->material_at(ray, hit.value(), &scene);
            auto hit_bg = scene.closest_intersection(ray, 1e-4f, true);

            if (!hit_bg)
                return mat.kd + BACKGROUND_COLOR * mat.transmittance;

            Color color_bg = get_color_result(ray, hit_bg.value(), scene, depth);
            return mat.kd + color_bg * mat.transmittance;
        }

        return get_color_result(ray, hit.value(), scene, depth);
    }

    std::vector<Color> raytrace(const Scene& scene, const Image& img) {
        int W = img.get_width();
        int H = img.get_height();
        std::vector<Color> pixels(W * H);

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> jitter(-0.5f, 0.5f);

        for (int i = 0; i < W * H; ++i) {
            int x = i % W;
            int y = i / W;

            Color accum(0.f, 0.f, 0.f);
            for (int s = 0; s < SAMPLES; ++s) {
                Ray ray = scene.cam.cast_ray(x, y, W, H, jitter(rng), jitter(rng));
                accum += trace(ray, scene, MAX_DEPTH);
            }
            pixels[i] = accum * (1.f / SAMPLES);
        }
        return pixels;
    }

}