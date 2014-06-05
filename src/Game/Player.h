#pragma once

#include "System/Common.h"
#include "ObjectData/ObjectData.h"
#include "ObjectData/RenderData.h"
#include "ObjectData/PhysicsData.h"

namespace gen
{
// protos
class PlayerCam;
class ObjectData;

const float c_ringScale = 20.f;
const float c_maxSpinRate = 0.03;
const int   c_maxShots = 8;
const float c_maxTime = 20.f;
const float c_maxShotRadius = 5.f;
const float c_maxCooldown = 0.5f;

class Shot : public ObjectData
{
public:
    Shot(float timeToLive) : 
        m_timeTotal(timeToLive), 
        m_timeRemaining(timeToLive), 
        m_scale(0),
        ObjectData(new RenderData(ci::app::loadResource(RES_CIRCLE_OBJ), ci::ColorAf(1.f, 1.f, 1.f, 0.5f)),
                   new PhysicsData(new btSphereShape(1.0f), 1, COL_SHOT, COL_ENEMY),
                   Vec3f(0.f,0.f,0.01f), Quatf(0.f,0.f,0.f), Vec3f(c_maxShotRadius*2, c_maxShotRadius*2, 1.f))
    {
        m_pRing = new ObjectData(new RenderData(ci::app::loadResource(RES_RING_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 1.0f)),
                                 NULL,
                                 Vec3f(0.f,0.f,0.01f), Quatf(0.f,0.f,0.f), Vec3f(m_scale, m_scale, 1.f));
    }

    ~Shot()
    {
        delete m_pRing;
    }

    void Update(float dt)
    {
#if 0
        if (m_timeRemaining > 0 && m_timeRemaining - dt <= 0)
        {
            console() << "position: " << this->m_pos << std::endl;
        }
#endif
        this->SetScale(Vec3f(m_scale, m_scale, 1.f));
        m_timeRemaining -= dt;
        ObjectData::Update(dt);
        m_pRing->SetScale(this->GetScale());
        m_pRing->SetPos(this->GetPos());
        m_pRing->SetRot(this->GetRot());
        m_pRing->Update(dt);
    }

    void Draw()
    {
        m_pRing->Draw();
        ObjectData::Draw();
    }

    float m_timeTotal;
    float m_timeRemaining;
    float m_scale;
    ObjectData* m_pRing;
};

class Player
{
public:
    Player();
    ~Player();
    virtual void Draw();
    virtual void Update(float dt);
    virtual void Reset();

    virtual void Shoot();

    ObjectData* m_pShip;
    bool m_charging;
    std::deque<Shot*> m_shots;
    float m_timeRemaining;
    float m_timeTotal;
    float m_cooldownTime;

private:
    inline void DrawTimer()
    {
        float ratio = m_timeRemaining / c_maxTime;

        glPushMatrix();
        glColor4f((1-ratio), 0.f, 0.f, 1.f);
        ratio *= 360;
        glBegin(GL_TRIANGLE_FAN);

        const float DEG2RAD = PI/180;

        glVertex3f(0.f, 0.f, -3.f);
        float angleOffset = PI/2;
//        angleOffset += (360-ratio) * DEG2RAD;
        for (float i=0; i <= ratio; i+=4.5f)
        {
            float degInRad = i*DEG2RAD;
            glVertex3f(cos(degInRad + angleOffset) * (c_ringScale-1)/2,
                       sin(degInRad + angleOffset) * (c_ringScale-1)/2,
                       -3.f);
        }

        glEnd();
        glPopMatrix();
    }

    ObjectData* m_pShipSpin;
    ObjectData* m_pRing;
    ObjectData* m_pShipCullCircle;
    ObjectData* m_pTimerCullCircle;
    ObjectData* m_pCooldownCircle;
    ObjectData* m_pCooldownCircleOutline;
    float m_spinTime;
    float m_spinRate;
    float m_spinPos;

#if CI_AUDIO
    ci::audio::SourceRef m_sfx_charge;
    ci::audio::SourceRef m_sfx_shoot;
#elif AL_AUDIO
    OpenAL::Sound* m_pSfxCharge;
    OpenAL::Sound* m_pSfxShoot;
 #endif
};
}
