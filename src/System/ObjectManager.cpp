#include "System/ObjectManager.h"
#include "App/App.h"
#include "ObjectData/ObjectData.h"
#include "ObjectData/RenderData.h"
#include "ObjectData/PhysicsData.h"
#include "ObjectData/SpriteSheet.h"
#include "Game/World.h"
#include "Game/Player.h"
#include "Game/PlayerCam.h"
#include "Particles/Particle.h"
#include "Particles/SquareParticle.h"

using namespace gen;
using namespace ci;

ObjectManager::ObjectManager()
{
    // Setup Camera
    CameraPersp     camera;
    camera.setPerspective(60, getWindowAspectRatio(), 1, 1000);
    camera.lookAt(Vec3f(0, 0, 50), Vec3f::zero());
    camera.setCenterOfInterestPoint(Vec3f::zero());
    m_pCamera = new PlayerCam(camera);

    // Initialize controls
    m_gameState     = TITLE;
    m_gravity       = false;
    m_displayStats  = false;
    m_displayGui    = false;
    m_fullscreen    = false;
    m_wireframe     = false;
    m_playMusic     = true;

    m_playtime      = 0;
    m_creditsTimer  = 0;    // counts up
    m_gameoverTimer = 0;    // counts down

    g_pApp->setFullScreen(m_fullscreen);

    if (m_gravity)
    {
        g_pBtDynamicsWorld->setGravity(btVector3(0, GRAVITY_CONST, 0));
    }
    else
    {
        g_pBtDynamicsWorld->setGravity(btVector3(0, 0, 0));
    }

    // Add World
    m_pWorld = new World();

    // FBO
    m_fboFormat.setSamples(4);
    m_fbo = gl::Fbo(g_pApp->getWindowWidth()*getWindow()->getContentScale(),
                    g_pApp->getWindowHeight()*getWindow()->getContentScale(),
                    m_fboFormat);
    
    // Load screens
    m_titleTex = gl::Texture(ci::loadImage(ci::app::loadResource(RES_TITLE_TEX)));
    m_rulesTex = gl::Texture(ci::loadImage(ci::app::loadResource(RES_RULES_TEX)));
    m_creditsTex = gl::Texture(ci::loadImage(ci::app::loadResource(RES_CREDITS_TEX)));
    m_gameoverTex = gl::Texture(ci::loadImage(ci::app::loadResource(RES_GAME_OVER_TEX)));
    m_continueTex = gl::Texture(ci::loadImage(ci::app::loadResource(RES_CONTINUE_TEX)));

    // Load shaders
    m_passthroughShader = gl::GlslProg(ci::app::loadResource(RES_VERT_PASSTHROUGH_SHADER), ci::app::loadResource(RES_FRAG_PASSTHROUGH_SHADER));
    m_flatshadeShader = gl::GlslProg(ci::app::loadResource(RES_VERT_FLATSHADE_SHADER), ci::app::loadResource(RES_FRAG_FLATSHADE_SHADER));

    // Load background music
    float bpm = 160;
    float bars = 4;
    m_bgTrackLength = 60.f / bpm * 8 * bars;
    m_loopPlaytime = m_bgTrackLength;   // trick audio to think it's overdue for play
#if CI_AUDIO
    m_bgm1 = ci::audio::load(ci::app::loadResource(RES_BGM1_SOUND));
    m_bgm2 = ci::audio::load(ci::app::loadResource(RES_BGM2_SOUND));
    m_bgm3 = ci::audio::load(ci::app::loadResource(RES_BGM3_SOUND));
    m_bgm4 = ci::audio::load(ci::app::loadResource(RES_BGM4_SOUND));
#elif AL_AUDIO
    m_pBgm1 = new OpenAL::Sound(ci::app::loadResource(RES_BGM1_SOUND));
    m_pBgm2 = new OpenAL::Sound(ci::app::loadResource(RES_BGM2_SOUND));
    m_pBgm3 = new OpenAL::Sound(ci::app::loadResource(RES_BGM3_SOUND));
    m_pBgm4 = new OpenAL::Sound(ci::app::loadResource(RES_BGM4_SOUND));
#endif

    // Load font for score
    m_scoreText.setFont(Font(app::loadResource(RES_SCORE_FONT), 80));
    m_scoreText.setColor(ci::Colorf(0.f, 0.f, 0.f));
    m_scoreText.setAlignment(TextBox::CENTER);
/*
    // Setup GUI
    g_pGui->lightColor = ColorA(1, 0, 0, 1);
    g_pGui->addLabel("OPTIONS:");
    g_pGui->addSeparator();
    g_pGui->addParam("Play music?", &m_playMusic, m_playMusic);
*/

/*
    // random cube
    ObjectData* pCube = new ObjectData(new RenderData(ci::app::loadResource(RES_CUBE_OBJ), ci::ColorAf(1.f, 0.f, 0.f, 1.f)),
                                                      NULL,
                                                      Vec3f(0.f,0.f,0.f), Quatf(0.f,0.f,0.f), Vec3f(10.f,10.f,10.f));

    // random circle
    ObjectData* pCircle = new ObjectData(new RenderData(ci::app::loadResource(RES_CIRCLE_OBJ), ci::ColorAf(0.f, 0.f, 0.f, 0.f)),
                                                      NULL,
                                                      Vec3f(0.f,0.f,0.1f), Quatf(0.f,0.f,0.f), Vec3f(30.f,30.f,1.f));
    m_objects.push_back(pCircle);
    m_objects.push_back(pCube);
*/
}

ObjectManager::~ObjectManager()
{
    for (auto i = m_objects.begin(); i != m_objects.end(); i++)
    {
        delete(*i);
    }
    delete m_pWorld;
    delete m_pCamera;
#if CI_AUDIO
#elif AL_AUDIO
    delete m_pBgm1;
    delete m_pBgm2;
    delete m_pBgm3;
    delete m_pBgm4;
#endif
}

void ObjectManager::Start()
{

}

void ObjectManager::Restart()
{
    m_playtime = 0;
    
    m_pCamera->Reset();

    m_pWorld->Reset();

    srand(time(NULL));

    g_pObjectManager->m_gameState = PLAYING;
}

void ObjectManager::UpdateSoundTimer(float dt)
{
    m_playtime += dt;

    m_loopPlaytime += dt;

    if (m_bgTrackLength <= m_loopPlaytime)
    {
        m_loopPlaytime = 0;
        if (m_playMusic)
        {
            if (m_gameState == TITLE)
            {
#if CI_AUDIO
                ci::audio::Output::play(m_bgm1);
#elif AL_AUDIO
                m_pBgm1->Play();
#endif
            }
            else if (m_gameState == RULES)
            {
#if CI_AUDIO
                ci::audio::Output::play(m_bgm2);
#elif AL_AUDIO
                m_pBgm2->Play();
#endif
            }
            else if (m_gameState == PLAYING || m_gameState == GAME_OVER)
            {
#if CI_AUDIO
                ci::audio::Output::play(m_bgm4);
#elif AL_AUDIO
                m_pBgm4->Play();
#endif
            }
        }
    }
}

void ObjectManager::GameOver()
{
    m_gameoverTimer = 2.f;    // counts down
    m_gameState = GAME_OVER;
}

void ObjectManager::DrawScore()
{
    float time = m_pWorld->m_pPlayer->m_timeTotal;
    int time_hours = time/3600;
    time = fmodf(time, 3600.f);
    int time_minutes = time/60;
    time = fmodf(time, 60.f);
    int time_seconds = time/1;
    time = fmodf(time, 1.f);
    int time_centiseconds = time*100;

    std::stringstream displayString;
//    displayString << "00:00:00:00";
    displayString << std::setfill('0') << std::setw(2) << time_hours << ":";
    displayString << std::setfill('0') << std::setw(2) << time_minutes << ":";
    displayString << std::setfill('0') << std::setw(2) << time_seconds << ":";
    displayString << std::setfill('0') << std::setw(2) << time_centiseconds;

    m_scoreText.setSize(Vec2f(g_pApp->getWindowWidth(), TextBox::GROW));
    m_scoreText.setText(displayString.str());
    gl::Texture texture = gl::Texture(m_scoreText.render());

    gl::pushMatrices();
    gl::setMatricesWindow(getWindowSize());
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::enableAlphaBlending();

    gl::draw(texture, Vec2f(0,0));

    gl::disableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::color(ColorA(1,1,1,1));
    gl::popMatrices();
    
}

void ObjectManager::Update(float dt)
{
    m_pCamera->Update(dt);

    // Update assets
    for (auto i = m_objects.begin(); i != m_objects.end(); i++)
    {
        (*i)->Update(dt);
    }

    if (m_pWorld)
    {
        m_pWorld->Update(dt);
    }

    if (m_gameState == GAME_OVER)
    {
        m_gameoverTimer -= dt;
    }

    if (m_gameState == CREDITS)
    {
        m_creditsTimer += dt;
        if (m_creditsTimer >= 2)
        {
            g_pApp->quit();
        }
    }
}

void ObjectManager::Draw()
{
    // Render state
#if ! defined(CINDER_GLES)
    m_wireframe ? gl::enableWireframe() : gl::disableWireframe();
#endif

//    m_fbo.bindFramebuffer();

    gl::enableVerticalSync();   // TODO: why can't this be set once at setup?
    gl::enableAlphaBlending();
    gl::enableDepthRead();
    gl::enable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    gl::clear(ColorA(178/255.f,178/255.f,178/255.f,1.0f));

    gl::pushMatrices();
    gl::setViewport(Area(0, 0, g_pApp->getWindowWidth()*getWindow()->getContentScale(), g_pApp->getWindowHeight()*getWindow()->getContentScale()));
    gl::setMatrices(m_pCamera->GetCamera());

    // flat shaded objects
/*
    m_flatshadeShader.bind();
    m_flatshadeShader.uniform("LightPosition", Vec4f(10.f, 10.f, 0.f, 1.f));
    m_flatshadeShader.uniform("Ld", Vec3f(1.f,1.f,1.f));
    m_flatshadeShader.unbind();
*/

    for (auto i = m_objects.begin(); i != m_objects.end(); i++)
    {
        (*i)->Draw();
    }

    m_pWorld->Draw();

    gl::popMatrices();
/*
    // Render framebuffer
    m_fbo.unbindFramebuffer();
    gl::enableVerticalSync();   // TODO: why can't this be set once at setup?
    gl::enableAlphaBlending();
    gl::enableDepthRead(false);
    gl::enableDepthWrite(false);
    gl::disable(GL_CULL_FACE);

    // Post process shader on FBO
    gl::color(1.f, 1.f, 1.f, 1.f);
    gl::setMatricesWindow(getWindowSize(), false);
    gl::Texture fboTex = m_fbo.getTexture();

    fboTex.enableAndBind();
    m_passthroughShader.bind();
    m_passthroughShader.uniform("tex0", 0);
    m_passthroughShader.uniform("resolution", Vec2f(fboTex.getWidth(), fboTex.getHeight()));
    m_passthroughShader.uniform("timer", static_cast<float>(m_playtime));
    m_passthroughShader.uniform("resolutionScale", getWindow()->getContentScale());

    gl::drawSolidRect(getWindowBounds());

    m_passthroughShader.unbind();
    fboTex.unbind();
    fboTex.disable();
*/

    // Draw game screens
//    gl::pushMatrices();
     gl::color(1,1,1,1);
    Vec2i windowSize = getWindowSize();

    int dim = windowSize.y < windowSize.x ? windowSize.y : windowSize.x;
    float scale = 1.4f;
    dim *= scale;

    float texRatio = 1.f;   // height / width
    int xoffset = 0;
    int yoffset = 0;
    Area bounds;

    switch (m_gameState)
    {
        case TITLE:
            texRatio = 0.5;
            xoffset = (windowSize.x - dim) / 2;
            yoffset = (windowSize.y - (dim*texRatio)) / 2;
            bounds.set(xoffset, yoffset, windowSize.x - xoffset, windowSize.y - yoffset);

            gl::setViewport(Area(0, 0, g_pApp->getWindowWidth()*getWindow()->getContentScale(), g_pApp->getWindowHeight()*getWindow()->getContentScale()));
            gl::setMatricesWindow(windowSize);
            gl::draw(m_titleTex, bounds);
            break;
        case RULES:
            texRatio = 0.5;
            xoffset = (windowSize.x - dim) / 2;
            yoffset = (windowSize.y - (dim*texRatio)) / 2;
            bounds.set(xoffset, yoffset, windowSize.x - xoffset, windowSize.y - yoffset);

            gl::setViewport(Area(0, 0, g_pApp->getWindowWidth()*getWindow()->getContentScale(), g_pApp->getWindowHeight()*getWindow()->getContentScale()));
            gl::setMatricesWindow(windowSize);
            gl::draw(m_rulesTex, bounds);
            break;
        case PLAYING:
            break;
        case GAME_OVER:
            texRatio = 0.5;
            xoffset = (windowSize.x - dim) / 2;
            yoffset = (windowSize.y - (dim*texRatio)) / 2;
            bounds.set(xoffset, yoffset, windowSize.x - xoffset, windowSize.y - yoffset);

            gl::setViewport(Area(0, 0, g_pApp->getWindowWidth()*getWindow()->getContentScale(), g_pApp->getWindowHeight()*getWindow()->getContentScale()));
            gl::setMatricesWindow(windowSize);
            if (m_gameoverTimer <= 0)
            {
                gl::draw(m_continueTex, bounds);
            }
            else
            {
                gl::draw(m_gameoverTex, bounds);
            }
            DrawScore();
            break;
        case CREDITS:
            texRatio = 0.5;
            xoffset = (windowSize.x - dim) / 2;
            yoffset = (windowSize.y - (dim*texRatio)) / 2;
            bounds.set(xoffset, yoffset, windowSize.x - xoffset, windowSize.y - yoffset);

            gl::setViewport(Area(0, 0, g_pApp->getWindowWidth()*getWindow()->getContentScale(), g_pApp->getWindowHeight()*getWindow()->getContentScale()));
            gl::setMatricesWindow(windowSize);
            gl::draw(m_creditsTex, bounds);
            break;
        default:
            break;
    }
//    gl::pushMatrices();


#if ! defined(CINDER_GLES)
    gl::disableWireframe();
#endif

    // Draw statistics
    if (m_displayStats)
    {
        g_pStats->Display();
    }

    // Draw gui
    if (m_displayGui)
    {
//        g_pGui->draw();
    }
}
