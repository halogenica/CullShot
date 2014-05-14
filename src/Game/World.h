#pragma once

#include "System/Common.h"
#include "ObjectData/ObjectData.h"
#include <deque>
#include <algorithm>

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
#elif CI_AUDIO2
    ci::audio2::VoiceRef m_sfx_score_hit;
    ci::audio2::VoiceRef m_sfx_score_miss;
#elif AL_AUDIO
    ALuint m_sfx_score_hit_buffer;
    ALuint m_sfx_score_miss_buffer;
    std::deque<ALuint> m_sfx_score_hit_sources;
    std::deque<ALuint> m_sfx_score_miss_sources;
#endif
};
}
