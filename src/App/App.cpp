#include "System/Common.h"
#include "System/ObjectManager.h"
#include "App/App.h"
#include "ObjectData/PhysicsData.h"
#include "Game/World.h"
#include "Game/PlayerCam.h"

#if AL_AUDIO
#include "OpenAL.h"
#endif

using namespace gen;

// externs
extern ContactProcessedCallback gContactProcessedCallback;

// global declarations
gen::Stats*                 g_pStats;               // the graphics and physics statistics
gen::App*                   g_pApp;                 // the application
gen::Input*                 g_pInputP1 = NULL;      // the input control system
gen::Input*                 g_pInputP2 = NULL;      // the input control system
gen::ObjectManager*         g_pObjectManager;       // the object manager
btDiscreteDynamicsWorld*    g_pBtDynamicsWorld;     // the physics world
//mowa::sgui::SimpleGUI*      g_pGui;                 // the GUI

// timestep variables
unsigned long       timeNow;        // the timestamp of the current frame
unsigned long       timePrev;       // the timestamp of the previous frame
unsigned long       timeFrame;      // the time delta for the current frame
unsigned long       timeSum;        // accumulation of time
const unsigned long timeDelta = 1;  // the constant update time delta
const unsigned long timeMax = 16;   // the constant maximum time delta

// bullet variables
btBroadphaseInterface*                  pBtBroadphase;              // the broadphase
btDefaultCollisionConfiguration*        pBtCollisionConfiguration;  // the collision config
btCollisionDispatcher*                  pBtDispatcher;              // the dispatch config
btSequentialImpulseConstraintSolver*    pBtSolver;                  // the physics solver
btCollisionShape*                       pBtGroundShape;             // the ground
btDefaultMotionState*                   pBtGroundMotionState;
btRigidBody*                            pBtGroundRigidBody;

// functions
void gen::App::prepareSettings(Settings *settings)
{
    settings->setWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	settings->enableHighDensityDisplay();
    settings->setFrameRate(1000.0f);
    settings->setTitle("CULL SHOT");
}

void gen::App::setup()
{
    g_pApp = this;

    timePrev = ci::app::getElapsedSeconds();
    timeNow  = 0;
    timeSum = 0;

    InitBullet();   // Must init bullet before statics
#if AL_AUDIO
    OpenAL::InitOpenAL();
#endif
    g_pStats = new Stats();
//    g_pGui = new mowa::sgui::SimpleGUI(g_pApp);
    g_pObjectManager = new ObjectManager();
    // TODO: move input creating inside object manager
    g_pInputP1 = new gen::Input(g_pObjectManager->m_pWorld->m_pPlayer);
    InitGamepad();

    this->getWindow()->connectKeyDown(&gen::Input::KeyDown, g_pInputP1);
    this->getWindow()->connectKeyUp(&gen::Input::KeyUp, g_pInputP1);
    this->getWindow()->connectMouseDown(&gen::Input::MouseDown, g_pInputP1);
    this->getWindow()->connectMouseDrag(&gen::Input::MouseDrag, g_pInputP1);

    // With all subsystems initialized, setup the game
//    g_pObjectManager->ResetGame();
}

void gen::App::shutdown()
{
    // Destroy the game before the subsystems disappear
//    g_pObjectManager->DestroyGame();

    // Cinder destroys "this" App object
    if (g_pInputP1)
    {
        delete g_pInputP1;
    }
    if (g_pInputP2)
    {
        delete g_pInputP2;
    }

    delete g_pObjectManager;
//    delete g_pGui;
    delete g_pStats;
#if AL_AUDIO
    OpenAL::DestroyOpenAL();
#endif
    DestroyBullet();
//    Gamepad_shutdown();
//    _CrtDumpMemoryLeaks();  // TODO: remove code checking for memory leaks
}

void gen::App::update()
{
    timeNow = ci::app::getElapsedSeconds() * 1000.0f ;
    timeFrame = timeNow - timePrev;
    timePrev = timeNow;

    g_pStats->updateFPS(timeFrame * .001f);    // g_pStats uses time in seconds
    g_pObjectManager->UpdateSoundTimer(timeFrame * .001f);

    // practical limitation, don't let updates overcome framerate
    if (timeFrame > timeMax)
    {
        timeFrame = timeMax;
    }

    timeSum += timeFrame;

    // update at a rate of timeDiff, regardless of framerate
    while (timeSum >= timeDelta)
    {
        UpdateTimestep(timeDelta * .001f);
        g_pBtDynamicsWorld->stepSimulation(timeDelta*.001f, 1, timeMax*.001f);
        g_pStats->updateUPS(timeDelta * .001f);    // g_pStats uses time in seconds
        timeSum -= timeDelta;
    }
}

void gen::App::draw()
{
    // set state

    // begin
    
    // draw
    g_pObjectManager->Draw();

    // end
    
    // present
}

void gen::App::resize()
{
    PlayerCam* pPlayerCam = g_pObjectManager->m_pCamera;
    if (pPlayerCam)
    {
        CameraPersp cam = pPlayerCam->GetCamera();
        cam.setPerspective(60, getWindowAspectRatio(), 1, 1000);
        pPlayerCam->SetCurrentCam(cam);
    }
}

void gen::App::InitGamepad()
{
    Gamepad_deviceAttachFunc(gen::Input::onDeviceAttached, (void *) 0x1);
    Gamepad_deviceRemoveFunc(gen::Input::onDeviceRemoved, (void *) 0x2);
    Gamepad_buttonDownFunc(gen::Input::onButtonDown, (void *) 0x3);
    Gamepad_buttonUpFunc(gen::Input::onButtonUp, (void *) 0x4);
    Gamepad_axisMoveFunc(gen::Input::onAxisMoved, (void *) 0x5);
    Gamepad_init();
}

void gen::App::ToggleFullscreen()
{
    g_pObjectManager->m_fullscreen = !g_pObjectManager->m_fullscreen;
    this->setFullScreen(g_pObjectManager->m_fullscreen);
    g_pObjectManager->m_fbo = gl::Fbo(g_pApp->getWindowWidth()*ci::app::getWindow()->getContentScale(),
                                        g_pApp->getWindowHeight()*ci::app::getWindow()->getContentScale(),
                                        g_pObjectManager->m_fboFormat);
}

void gen::App::InitBullet()
{
    // Build the broadphase
    pBtBroadphase = new btDbvtBroadphase();
 
    // Set up the collision configuration and dispatcher
    pBtCollisionConfiguration = new btDefaultCollisionConfiguration();
    pBtDispatcher = new btCollisionDispatcher(pBtCollisionConfiguration);
 
    // The actual physics solver
    pBtSolver = new btSequentialImpulseConstraintSolver;
 
    // The physics world
    g_pBtDynamicsWorld = new btDiscreteDynamicsWorld(pBtDispatcher,pBtBroadphase,pBtSolver,pBtCollisionConfiguration);
    g_pBtDynamicsWorld->setGravity(btVector3(0, GRAVITY_CONST, 0));

    gContactProcessedCallback = (ContactProcessedCallback)PhysicsData::CollisionCallback;
}

void gen::App::UpdateTimestep(float dt)
{
    g_pObjectManager->Update(dt);

    if (g_pInputP1)
    {
        g_pInputP1->Update(dt);
    }
    if (g_pInputP2)
    {
        g_pInputP2->Update(dt);
    }
}

void gen::App::DestroyBullet()
{
    delete g_pBtDynamicsWorld;
    delete pBtSolver;
    delete pBtDispatcher;
    delete pBtCollisionConfiguration;
    delete pBtBroadphase;
    delete pBtGroundShape;
}
