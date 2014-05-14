#pragma once

#include "System/Common.h"
#include <deque>

namespace gen
{
// protos
class ObjectData;

const float c_zOffset       = -3.f;

class Enemy
{
public:
    Enemy();
    ~Enemy();
    void Draw();
    void Update(float dt);
    void Reset();

    ObjectData* m_pModel;
    bool        m_inBounds;

private:
    void Reset2();
    ObjectData*         m_pRingPoint;
    ci::gl::GlslProg    m_diagonalsShader;
    float               m_timeTillSpawn;
};

}
