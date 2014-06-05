#pragma once

#include "System/Common.h"
#include "App/Input.h"
#include "Utils/Stats.h"

namespace gen
{
// protos
class ObjectData;
class SpriteSheet;
class PlayerCam;
class Particle;
class World;

class ObjectManager
{
public:
    ObjectManager();
    ~ObjectManager();
    void Update(float dt);
    void Draw();
    void UpdateSoundTimer(float dt);    // this is its own function because the timing is very sensitive
    void Start();                       // transition from Ready to Playing state.
    void Restart();
    void GameOver();
    void DrawScore();

    GameState       m_gameState;
    bool            m_gravity;
    bool            m_displayStats;
    bool            m_displayGui;
    bool            m_fullscreen;
    bool            m_wireframe;
    bool            m_playMusic;

    PlayerCam*      m_pCamera;

    std::deque<ObjectData*> m_objects;  // collection of assets
    World*          m_pWorld;

    ci::gl::Fbo     m_fbo;
    ci::gl::Fbo::Format m_fboFormat;

    ci::gl::Texture m_titleTex;
    ci::gl::Texture m_rulesTex;
    ci::gl::Texture m_creditsTex;
    ci::gl::Texture m_gameoverTex;
    ci::gl::Texture m_continueTex;

    double          m_playtime;
    double          m_creditsTimer;
    double          m_gameoverTimer;
    double          m_loopPlaytime;
    double          m_bgTrackLength;

    ci::gl::GlslProg    m_passthroughShader;
    ci::gl::GlslProg    m_flatshadeShader;

#if CI_AUDIO
    ci::audio::SourceRef m_bgm1;
    ci::audio::SourceRef m_bgm2;
    ci::audio::SourceRef m_bgm3;
    ci::audio::SourceRef m_bgm4;
#elif AL_AUDIO
    OpenAL::Sound*  m_pBgm1;
    OpenAL::Sound*  m_pBgm2;
    OpenAL::Sound*  m_pBgm3;
    OpenAL::Sound*  m_pBgm4;
#endif

    ci::TextBox     m_scoreText;
};
}
