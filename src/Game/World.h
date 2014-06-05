#pragma once

#include "System/Common.h"
#include "ObjectData/ObjectData.h"

namespace gen
{
// protos
class Enemy;

class World
{
public:
    World();
    ~World();
    void Draw();
    void Update(float dt);
    void Reset();
    void Score();

    ObjectData*         m_pDiagonalsRect;
    Player*             m_pPlayer;
    std::deque<Enemy*>  m_enemies;

    ci::gl::GlslProg    m_diagonalsShader;


private:
#if CI_AUDIO
    ci::audio::SourceRef m_sfx_score_hit;
    ci::audio::SourceRef m_sfx_score_miss;
#elif AL_AUDIO
    OpenAL::Sound*  m_pSfxScoreHit;
    OpenAL::Sound*  m_pSfxScoreMiss;
#endif
};
}
