#include "System/ObjectManager.h"
#include "App/App.h"
#include "Game/Player.h"
#include "Game/PlayerCam.h"

using namespace gen;
using namespace ci;
using namespace ci::app;

Player::Player() : m_spinPos(0), m_spinRate(0), m_spinTime(0), m_charging(false), m_timeRemaining(c_maxTime), m_timeTotal(0), m_cooldownTime(0)
{
    m_pShip = new ObjectData(new RenderData(ci::app::loadResource(RES_SHIP_OBJ), ci::ColorAf(1.f, 1.f, 1.f, 1.f)),
                             new PhysicsData(new btSphereShape(0.25f), 1, COL_PLAYER, COL_BULLET),
                             Vec3f(0.f,0.f,0.f), Quatf(0.f,0.f,0.f), Vec3f(1.f,1.f,1.f));
    m_pShip->m_pRenderData->m_cellshadePercent = 0.2;
    m_pShip->m_pPhysicsData->m_pBtRigidBody->setLinearFactor(btVector3(1,1,0));       // cannot move in z-axis
    m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularFactor(btVector3(0,0,1));      // can only rotate along z-axis

    m_pShipSpin = new ObjectData(new RenderData(ci::app::loadResource(RES_SHIP_SPIN_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 1.f)),
                                 NULL,
                                 Vec3f(0.f,0.f,0.f), Quatf(0.f,0.f,0.f), Vec3f(1.f,1.f,1.f));

    m_pRing = new ObjectData(new RenderData(ci::app::loadResource(RES_RING_DASH2_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 1.f)),
                             NULL,
                             Vec3f(0.f,0.f,0.f), Quatf(0.f,0.f,0.f), Vec3f(c_ringScale, c_ringScale, 1.f));

    m_pShipCullCircle = new ObjectData(new RenderData(ci::app::loadResource(RES_CIRCLE_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 0.f)),
                                       NULL,
                                       Vec3f(0.f,0.f,0.01f), Quatf(0.f,0.f,0.f), Vec3f(c_ringScale+1, c_ringScale+1, 1.f));

    m_pTimerCullCircle = new ObjectData(new RenderData(ci::app::loadResource(RES_CIRCLE_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 0.f)),
                                        NULL,
                                        Vec3f(0.f,0.f,-2.9f), Quatf(0.f,0.f,0.f), Vec3f(c_ringScale-2, c_ringScale-2, 1.f));

    m_pCooldownCircle = new ObjectData(new RenderData(ci::app::loadResource(RES_CIRCLE_DASH_OBJ), ci::ColorAf(.7f, 0.f, 0.f, 0.5f)),
                                       NULL,
                                       Vec3f(0.f,0.f,0.f), Quatf(0.f,0.f,0.f), Vec3f(1.f, 1.f, 1.f));

    m_pCooldownCircleOutline = new ObjectData(new RenderData(ci::app::loadResource(RES_CIRCLE_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 0.5f)),
                                              NULL,
                                              Vec3f(0.f,0.f,-0.01f), Quatf(0.f,0.f,0.f), Vec3f(1.f, 1.f, 1.f));
    for (int i = 0; i < c_maxShots; i++)
    {
        Shot* shot = new Shot(4);
        shot->m_pPhysicsData->m_pBtRigidBody->setDamping(0.8, 0); // "air resistance"
        shot->m_timeRemaining = 0;
        m_shots.push_back(shot);
    }

#if CI_AUDIO
    m_sfx_charge        = ci::audio::load(ci::app::loadResource(RES_SFX_CHARGE_SOUND));
    m_sfx_shoot         = ci::audio::load(ci::app::loadResource(RES_SFX_SHOOT_SOUND));
#elif CI_AUDIO2
    m_sfx_charge        = audio2::Voice::create(audio2::load(loadResource(RES_SFX_CHARGE_SOUND)));
    m_sfx_shoot         = audio2::Voice::create(audio2::load(loadResource(RES_SFX_SHOOT_SOUND)));
#elif AL_AUDIO
    m_sfx_charge_buffer     = OpenAL::CreateBuffer(ci::app::loadResource(RES_SFX_CHARGE_SOUND));
    m_sfx_shoot_buffer      = OpenAL::CreateBuffer(ci::app::loadResource(RES_SFX_SHOOT_SOUND));

    m_sfx_charge_sources.push_back(OpenAL::CreateSource(m_sfx_charge_buffer));
    m_sfx_shoot_sources.push_back(OpenAL::CreateSource(m_sfx_shoot_buffer));
#endif
}

Player::~Player()
{
    for (auto i : m_shots)
    {
        delete(i);
    }

    delete m_pShip;
    delete m_pShipSpin;
    delete m_pRing;
    delete m_pShipCullCircle;
    delete m_pTimerCullCircle;
    delete m_pCooldownCircle;
    delete m_pCooldownCircleOutline;
#if CI_AUDIO
#elif CI_AUDIO2
    m_sfx_charge->stop();
    m_sfx_shoot->stop();
#elif AL_AUDIO
    alDeleteSources(m_sfx_charge_sources.size(), &m_sfx_charge_sources[0]);
    alDeleteSources(m_sfx_shoot_sources.size(), &m_sfx_shoot_sources[0]);
#endif
}

void Player::Draw()
{
    // Timer cull circle comes first
    m_pTimerCullCircle->Draw();
    DrawTimer();

    // Draw the player
    m_pShip->Draw();
    m_pShipSpin->Draw();
    m_pRing->Draw();

    // Draw cooldown circle
    if (m_cooldownTime > 0)
    {
        float cooldownScale = m_cooldownTime / c_maxCooldown * (c_ringScale-10);
        m_pCooldownCircle->SetScale(Vec3f(cooldownScale, cooldownScale, 1.f));
        m_pCooldownCircle->Draw();

        float cooldownOutlineScale = m_cooldownTime / c_maxCooldown * (c_ringScale-9);
        m_pCooldownCircleOutline->SetScale(Vec3f(cooldownOutlineScale, cooldownOutlineScale, 1.f));
        m_pCooldownCircleOutline->Draw();
    }

    // Other cull circles come last
    for (auto i : m_shots)
    {
        if (i->m_timeRemaining > 0)
        {
            i->Draw();
        }
    }
    m_pShipCullCircle->Draw();
}

void Player::Update(float dt)
{
    if (m_timeRemaining > c_maxTime)
    {
        m_timeRemaining = c_maxTime;
    }
    if (g_pObjectManager->m_gameState == PLAYING)
    {
        m_timeRemaining -= dt;
        m_timeTotal += dt;
        m_cooldownTime -= dt;
    }

    m_pShip->Update(dt);

    if (m_charging)
    {
        m_spinTime += dt;

        if (m_spinRate < c_maxSpinRate)
        {
            m_spinRate += 0.00003;
        }

        m_spinPos += m_spinRate;
        if (m_spinPos >= PI)
        {
            // Play sound
#if CI_AUDIO
            ci::audio::Output::play(m_sfx_charge);
#elif CI_AUDIO2
            m_sfx_charge->stop();
            m_sfx_charge->play();
#elif AL_AUDIO
            ALenum state;
            ALuint source = m_sfx_charge_sources.back();
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING)
            {
                source = OpenAL::CreateSource(m_sfx_charge_buffer);
                m_sfx_charge_sources.push_back(source);
            }
            alSourcePlay(source);
#endif
        }
        m_spinPos = fmodf(m_spinPos, PI);
    }
    else
    {
        if (m_spinRate > 0.01)
        {
            m_spinRate -= 0.00003;
            m_spinPos += m_spinRate;
        }
        else
        {
            if (m_spinPos >= PI)
            {
                // Play sound
#if CI_AUDIO
                ci::audio::Output::play(m_sfx_charge);
#elif CI_AUDIO2
                m_sfx_charge->stop();
                m_sfx_charge->play();
#elif AL_AUDIO
                ALenum state;
                ALuint source = m_sfx_charge_sources.back();
                alGetSourcei(source, AL_SOURCE_STATE, &state);
                if (state == AL_PLAYING)
                {
                    source = OpenAL::CreateSource(m_sfx_charge_buffer);
                    m_sfx_charge_sources.push_back(source);
                }
                alSourcePlay(source);
#endif
            }
            m_spinPos = fmodf(m_spinPos, PI);

            if (m_spinPos + m_spinRate < PI)
            {
                m_spinPos += m_spinRate;
            }
            else
            {
                m_spinRate = 0;
                m_spinPos = 0;
            }
        }
    }

    m_pShipSpin->SetRot(Quatf(0.f, m_spinPos, 0.f) * m_pShip->GetRot());
    m_pShipSpin->Update(dt);

    m_pRing->SetRot(Quatf(0.f, 0.f, dt + m_pRing->GetRot().getRoll()));
    m_pRing->Update(dt);

    m_pCooldownCircle->SetRot(Quatf(0.f, 0.f, dt*5 + m_pCooldownCircle->GetRot().getRoll()));
    m_pCooldownCircle->Update(dt);
    m_pCooldownCircleOutline->SetRot(Quatf(0.f, 0.f, dt*5 + m_pCooldownCircleOutline->GetRot().getRoll()));
    m_pCooldownCircleOutline->Update(dt);

    m_pTimerCullCircle->Update(dt);
    m_pShipCullCircle->Update(dt);

    for (auto i : m_shots)
    {
        if (i->m_timeRemaining <= 0)
        {
            i->m_scale = 0;
            i->SetPos(Vec3f(0,0,0.01f));
        }
        else
        {
            i->m_scale = std::min((i->m_timeTotal - i->m_timeRemaining) / i->m_timeTotal * 5, 1.f) * c_maxShotRadius * 2 + std::sin(i->m_timeRemaining*20)/5;
            i->Update(dt);
        }
    }

    if (g_pObjectManager->m_gameState == PLAYING && m_timeRemaining <= 0)
    {
        g_pObjectManager->GameOver();
    }

    // screen shake
    if (g_pObjectManager->m_gameState == PLAYING)
    {
        float x = GetRandomMinMax(0.f, m_spinRate*20);
        float y = GetRandomMinMax(0.f, m_spinRate*20);
        float z = 50.f;
        CameraPersp cam = g_pObjectManager->m_pCamera->GetCamera();
        cam.setEyePoint(Vec3f(x,y,z));
        g_pObjectManager->m_pCamera->SetCurrentCam(cam);
    }
}

void Player::Reset()
{
    m_timeRemaining = c_maxTime;
    m_timeTotal = 0;
    m_cooldownTime = 0;
    for (auto i : m_shots)
    {
        i->m_timeRemaining = 0;
        i->m_scale = 0;
        i->SetPos(Vec3f(0,0,0.01f));
    }
}

void Player::Shoot()
{
    Shot* shot = NULL;
    for (auto i : m_shots)
    {
        if (i->m_timeRemaining <= 0)
        {
            shot = i;
            break;
        }
    }

    if (shot && (m_spinRate > 0))
    {
        // Reset the shot
        shot->m_timeRemaining = shot->m_timeTotal;

        // Get orientation of player
        Vec3f orient = Vec3f(0,1,0);
        Quatf rot = m_pShip->GetRot();
        if (rot.getAngle() != 0)
        {
            orient.rotate(rot.getAxis(), rot.getAngle());
        }
        // Apply impulse to cull shot
        btVector3 impulse = btVector3(orient.x,orient.y,orient.z);
        impulse = impulse * 55 * (m_spinRate / c_maxSpinRate);
        shot->m_pPhysicsData->m_pBtRigidBody->applyImpulse(impulse,btVector3(0,0,0));

        // Play sound
#if CI_AUDIO
        ci::audio::Output::play(m_sfx_shoot);
#elif CI_AUDIO2
        m_sfx_shoot->stop();
        m_sfx_shoot->play();
#elif AL_AUDIO
        ALenum state;
        ALuint source = m_sfx_shoot_sources.back();
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING)
        {
            source = OpenAL::CreateSource(m_sfx_shoot_buffer);
            m_sfx_shoot_sources.push_back(source);
        }
        alSourcePlay(source);
#endif
    }
}
