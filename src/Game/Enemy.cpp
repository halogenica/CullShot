#include "Game/Enemy.h"
#include "Game/Player.h"
#include "ObjectData/ObjectData.h"
#include "ObjectData/RenderData.h"
#include "ObjectData/PhysicsData.h"
#include "App/App.h"
#include "System/ObjectManager.h"

using namespace gen;
using namespace ci;
using namespace ci::app;

const float c_maxEnemyDist = 36;    // max reach of shot = 33.75
const float c_minEnemyDist = c_ringScale/2 + 4;

Enemy::Enemy()
{
    m_pModel = new ObjectData(new RenderData(ci::app::loadResource(RES_CUBE_OBJ), ci::ColorAf(1.f, 0.f, 0.f, 1.f)),
                              new PhysicsData(new btSphereShape(1.0f), 1),
                              Vec3f(100.f,0.f,c_zOffset), Quatf(0.f,0.f,0.f), Vec3f(3.f, 3.f, 3.f));
    m_pModel->m_pRenderData->m_cellshadePercent = 0.2f;

    m_pRingPoint = new ObjectData(new RenderData(ci::app::loadResource(RES_RING_POINT_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 1.f)),
                                  NULL,
                                  Vec3f(0.f,0.f,0.f), Quatf(0.f,0.f,0.f), Vec3f(1.f, 1.f, 1.f));

    m_diagonalsShader = gl::GlslProg(ci::app::loadResource(RES_VERT_PASSTHROUGH_SHADER), ci::app::loadResource(RES_FRAG_DIAGONALS2_SHADER));

    Reset();
}

Enemy::~Enemy()
{
    delete m_pModel;
    delete m_pRingPoint;
}

void Enemy::Reset()
{
    // Random spawn timer
    m_timeTillSpawn = GetRandomMinMax(0.f, 8.f);
}

void Enemy::Reset2()
{
    // Place enemy on outer ring with trajectory to pass by inner ring
    Vec3f innerRingPos = GetRandomVector();
    innerRingPos = Vec3f(innerRingPos.x, innerRingPos.y, 0.f);
    innerRingPos = innerRingPos.normalized() * c_minEnemyDist;
    Vec3f direction = innerRingPos;
    direction.rotateZ(PI/2);
    direction.normalize();
    Vec3f outerRingPos = direction * (sqrtf(c_maxEnemyDist*c_maxEnemyDist - c_minEnemyDist*c_minEnemyDist)) + innerRingPos;
    outerRingPos.z = c_zOffset;
    m_pModel->SetPos(outerRingPos);
    direction = direction * -5.f;
    m_pModel->m_pPhysicsData->m_pBtRigidBody->setLinearVelocity(btVector3(direction.x, direction.y, 0));

    // Random rotation
    Vec3f rot = GetRandomVector().normalized() * 5;
    m_pModel->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(rot.x, rot.y, rot.z));
}

void Enemy::Draw()
{
    if (m_inBounds)
    {
        m_diagonalsShader.bind();
        m_diagonalsShader.uniform("resolution", Vec2f(g_pApp->getWindowWidth(), g_pApp->getWindowHeight()));
        m_diagonalsShader.uniform("timer", static_cast<float>(g_pObjectManager->m_playtime));
        m_diagonalsShader.uniform("resolutionScale", getWindow()->getContentScale());

        m_pModel->Draw();

        m_diagonalsShader.unbind();

        m_pRingPoint->Draw();
    }
}

void Enemy::Update(float dt)
{
    Vec3f pos = m_pModel->GetPos();

    // Point ring aims at enemy
    Vec2f up = Vec2f(0,1);
    Vec3f posn = pos.normalized();
    float theta = acos(up.dot(Vec2f(posn.x, posn.y)));
    if (m_pModel->GetPos().x < 0)
    {
        theta = -theta;
    }
    m_pRingPoint->SetRot(Quatf(0.f, 0.f, -theta));

    // Point ring scaled (remap distance 18:36 to scale 5:10)
    float scale = pos.length();
    scale = scale / c_minEnemyDist * 5.f;
    m_pRingPoint->SetScale(Vec3f(scale, scale, 1.f));

    // Fade out the ring if the enemy is too far away (scale 10 = alpha 1, scale 12 = alpha 0)
    float alpha = 1.f;
    float maxScale = c_maxEnemyDist / c_minEnemyDist * 5.f;
    if (scale > (maxScale-2))
    {
        alpha = (maxScale - scale) / 2;
    }
    m_pRingPoint->m_pRenderData->m_color.a = alpha;
    m_pModel->m_pRenderData->m_color.a = alpha;

    m_inBounds = Vec3f(pos.x, pos.y, 0.f).length() <= c_maxEnemyDist && pos.z <= c_maxEnemyDist;

    m_pModel->Update(dt);
    m_pRingPoint->Update(dt);

    pos = m_pModel->GetPos();

    if (Vec3f(pos.x, pos.y, 0.f).length() > c_maxEnemyDist || pos.z > c_maxEnemyDist)
    {
        if (m_inBounds)
        {
            Reset();
        }
    }

    if (!m_inBounds)
    {
        if (m_timeTillSpawn <= 0.f)
        {
            Reset2();
        }
        else
        {
            m_timeTillSpawn -= dt;
        }
    }
}
