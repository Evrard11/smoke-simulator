//
// Created by Evrard on 11/04/2026.
//

#include "texture.hh"

#include <iostream>

#include "../objects/box.hh"
#include "../main/engine.hh"

namespace isim {
    MaterialParams ImageTexture::get_params(const Ray& ray, const Hit& hit, const Scene& scene) const {
        int numSteps = 50;
        float stepSize = (hit.t1 - hit.t2) / numSteps;
        float transmittance = 1.0f; // light that goes through
        Color accumulatedLight = {0,0,0};

        for (int i = 0; i < numSteps; i++) {
            float t = hit.t1 + (i + 0.5f) * stepSize;
            Point3 pos = ray.at(t);

            Box* box = dynamic_cast<Box*>(hit.obj);
            Vector3 density_coords = (pos - box->min) / (box->max - box->min);
            float px = density_coords.x * (smoke_->densityMap.size() - 1);
            float py = density_coords.y * (smoke_->densityMap[0].size() - 1);
            float pz = density_coords.z * (smoke_->densityMap[0][0].size() - 1);
            // std::cout << px << " " << py << " " << pz << std::endl;
            float density = smoke_->SampleTrilinear(smoke_->densityMap,
                smoke_->cellCountX, smoke_->cellCountY, smoke_->cellCountZ, px, py, pz);

            float extinction = (absorptionCoef_ + scatteringCoef_) * density;
            float stepTransmittance = exp(-extinction * stepSize);
            transmittance *= stepTransmittance;

            for (Light* light : scene.lights) {
                Color Il = light->intensity_at(pos);

                float shadowDensity = marchToLight(pos, light, box, *smoke_, scene);
                float lightTransmittance = exp(-absorptionCoef_ * shadowDensity);

                accumulatedLight += Il * transmittance * density * scatteringCoef_ * lightTransmittance * stepSize;
            }
        }
        // return transmittance;
        // if (transmittance > 0.85f) transmittance = 1.f;
        return { accumulatedLight,Color(0,0,0), base_params_.ns, base_params_.kr, transmittance};
    }
}
