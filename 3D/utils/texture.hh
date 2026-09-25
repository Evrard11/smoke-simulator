#ifndef TEXTURE_HH
#define TEXTURE_HH

#include "color.hh"
#include "../smoke/fluidGrid.h"
#include "../utils/Hit.h"

#include "../light/ray.hh"
// #include "../main/scene.hh"

using namespace std;

namespace isim {
    class Scene;

    // Parameters
    struct MaterialParams {
        Color kd; 
        Color ks;  
        float ns;  
        float kr;
        float transmittance=-1.f;
    };

    // Abstract class for texture materials
    class TextureMaterial {
    public:
        virtual ~TextureMaterial() = default;

        // Get material parameters at a given position
        virtual MaterialParams get_params(const Ray& ray, const Hit& hit, const Scene& scene) const = 0;
    };

    // Same parameters everywhere
    class UniformTexture : public TextureMaterial {
    public:
        UniformTexture(const MaterialParams& params) : params_(params) {}

        MaterialParams get_params(const Ray& ray, const Hit& hit, const Scene& scene) const override {
            return params_;
        }

    private:
        MaterialParams params_;
    };

    class ImageTexture : public TextureMaterial {
    public:
        ImageTexture(smoke::FluidGrid3D* smoke, const MaterialParams& base_params)
            : smoke_(smoke), base_params_(base_params) {}

        MaterialParams get_params(const Ray& ray, const Hit& hit, const Scene& scene) const override;

    private:
        smoke::FluidGrid3D* smoke_;
        MaterialParams base_params_;
        float absorptionCoef_=0.1f;
        float scatteringCoef_=2.0f;
    };
    
}

#endif /* TEXTURE_HH */