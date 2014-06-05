#include "System/ObjectManager.h"
#include "App/App.h"
#include "ObjectData/RenderData.h"
#include "ObjectData/PhysicsData.h"
#include "Game/Player.h"
#include "Game/Enemy.h"
#include "Game/World.h"
#include "Game/PlayerCam.h"

using namespace gen;
using namespace ci;
using namespace ci::app;

World::World()
{
#if CI_AUDIO
    m_sfx_score_hit     = ci::audio::load(ci::app::loadResource(RES_SFX_SCORE_HIT_SOUND));
    m_sfx_score_miss    = ci::audio::load(ci::app::loadResource(RES_SFX_SCORE_MISS_SOUND));
#elif AL_AUDIO
    m_pSfxScoreHit      = new OpenAL::Sound(ci::app::loadResource(RES_SFX_SCORE_HIT_SOUND));
    m_pSfxScoreMiss     = new OpenAL::Sound(ci::app::loadResource(RES_SFX_SCORE_MISS_SOUND));
#endif

    // Initialize Player
    m_pPlayer = new Player();

    // Initialize Enemies
    m_enemies.push_back(new Enemy());
    m_enemies.push_back(new Enemy());
    m_enemies.push_back(new Enemy());
    m_enemies.push_back(new Enemy());
    m_enemies.push_back(new Enemy());

    m_diagonalsShader = gl::GlslProg(ci::app::loadResource(RES_VERT_PASSTHROUGH_SHADER), ci::app::loadResource(RES_FRAG_DIAGONALS_SHADER));

    // show diagonals
    m_pDiagonalsRect = new ObjectData(new RenderData(ci::app::loadResource(RES_SQUARE_OBJ), ci::ColorAf(0.7f, 0.7f, 0.7f, 1.f)), 
                                      NULL,
                                      Vec3f(0,0,0), Quatf(0,0,0), Vec3f(1000.f,500.f,1.f));
}

World::~World()
{
    for (auto i : m_enemies)
    {
        delete i;
    }
    delete m_pPlayer;
    delete m_pDiagonalsRect;

#if CI_AUDIO
#elif AL_AUDIO
    delete m_pSfxScoreHit;
    delete m_pSfxScoreMiss;
#endif
}

void World::Draw()
{
    for (auto i : m_enemies)
    {
        i->Draw();
    }

    m_pPlayer->Draw();

    if (m_pDiagonalsRect && g_pObjectManager->m_gameState == PLAYING)
    {
        m_diagonalsShader.bind();
        m_diagonalsShader.uniform("resolution", Vec2f(g_pApp->getWindowWidth(), g_pApp->getWindowHeight()));
        m_diagonalsShader.uniform("timer", static_cast<float>(g_pObjectManager->m_playtime));
        m_diagonalsShader.uniform("resolutionScale", getWindow()->getContentScale());
        m_pDiagonalsRect->Draw();
        m_diagonalsShader.unbind();
    }
}

void World::Update(float dt)
{
    if (g_pObjectManager->m_gameState == TITLE ||
        g_pObjectManager->m_gameState == RULES)
    {

    }
    else
    {

    }
    
    for (auto i : m_enemies)
    {
        i->Update(dt);
    }

    m_pPlayer->Update(dt);
    m_pDiagonalsRect->Update(dt);
}

void World::Score()
{
    if (m_pPlayer->m_cooldownTime <= 0.f)
    {
        // Apply cooldown
        m_pPlayer->m_cooldownTime = c_maxCooldown;
        CameraPersp cam = g_pObjectManager->m_pCamera->GetCamera();

        bool score = false;
        for (auto enemy : m_enemies)
        {
            Vec3f enemyPos = enemy->m_pModel->GetPos();
            Vec2f enemyUV = cam.worldToScreen(enemyPos, g_pApp->getWindowWidth(), g_pApp->getWindowHeight());
            for (auto shot : m_pPlayer->m_shots)
            {
                Vec3f shotPos = shot->GetPos();

                // adjust shot position to same plane as enemy
                Vec2f shotUV = cam.worldToScreen(shotPos, g_pApp->getWindowWidth(), g_pApp->getWindowHeight());
                Vec2f dir = enemyUV - shotUV;
                Vec3f shotRadiusPos = shotPos + Vec3f(dir.normalized() * c_maxShotRadius, shotPos.z);
                Vec2f shotRadiusUV = cam.worldToScreen(shotRadiusPos, g_pApp->getWindowWidth(), g_pApp->getWindowHeight());

                if (enemyPos.z == c_zOffset && shotUV.distance(enemyUV) <= shotUV.distance(shotRadiusUV))
                {
                    m_pPlayer->m_timeRemaining += 3.f;
                    enemy->m_pModel->m_pPhysicsData->m_pBtRigidBody->setLinearVelocity(btVector3(0.f, 0.f, 20.f));
                    shot->m_pPhysicsData->m_pBtRigidBody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));

    #if CI_AUDIO
                    ci::audio::Output::play(m_sfx_score_hit);
    #elif AL_AUDIO
                    m_pSfxScoreHit->Play();
    #endif

                    score = true;
                    break;
                }
            }
        }

        if (!score)
        {
    #if CI_AUDIO
            ci::audio::Output::play(m_sfx_score_miss);
    #elif AL_AUDIO
            m_pSfxScoreMiss->Play();
    #endif
        }
    }
}

void World::Reset()
{
    m_pPlayer->Reset();
    for (auto enemy : m_enemies)
    {
        enemy->Reset();
    }
}
