//
// Created by Evrard on 10/04/2026.
//

#ifndef SMOKE_SIMULATOR_HIT_H
#define SMOKE_SIMULATOR_HIT_H

namespace isim
{
    class Object;
}

struct Hit {
    float t1 = -1.f;
    isim::Object* obj = nullptr;
    float t2 = -1.f;
};

#endif //SMOKE_SIMULATOR_HIT_H