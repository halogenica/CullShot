#include "System/Common.h"
#include "System/ObjectManager.h"
#include "App/App.h"
#include "ObjectData/PhysicsData.h"
#include "Game/World.h"
#include "Game/PlayerCam.h"

using namespace gen;

// externs
extern ContactProcessedCallback gContactProcessedCallback;

// global declarations
gen::App*                   g_pApp;                 // the application
gen::Input*                 g_pInputP1 = NULL;      // the input control system
gen::Input*                 g_pInputP2 = NULL;      // the input control system
gen::Stats*                 g_pStats;               // the graphics and physics statistics
gen::ObjectManager*         g_pObjectManager;       // the object manager
btDiscreteDynamicsWorld*    g_pBtDynamicsWorld;     // the physics world
mowa::sgui::SimpleGUI*      g_pGui;                 // the GUI

// timestep variables
float       timeNow;                // the timestamp of the current frame in seconds
float       timePrev;               // the timestamp of the previous frame in seconds
float       timeFrame;              // the time delta for the current frame in seconds
float       timeSum;                // accumulation of time in seconds
const float timeDelta = 1 * .001f;  // the constant update time delta in seconds
const float timeMax = 16 * .001f;   // the constant maximum time delta in seconds

// bullet variables
btBroadphaseInterface*                  pBtBroadphase;              // the broadphase
btDefaultCollisionConfiguration*        pBtCollisionConfiguration;  // the collision config
btCollisionDispatcher*                  pBtDispatcher;              // the dispatch config
btSequentialImpulseConstraintSolver*    pBtSolver;                  // the physics solver
btDefaultMotionState*                   pBtGroundMotionState;
btRigidBody*                            pBtGroundRigidBody;

// crt debug variables
#ifdef _CRTDBG_MAP_ALLOC
_CrtMemState     memStateBegin;
_CrtMemState     memStateEnd;
_CrtMemState     memStateDiff;
#endif

/**************************************
Private functions
**************************************/
void gen::App::prepareSettings(Settings *settings)
{
    settings->setWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	settings->enableHighDensityDisplay();
    settings->setFrameRate(1000.0f);
    settings->setTitle("CULL SHOT");
}

void gen::App::setup()
{
#ifdef _CRTDBG_MAP_ALLOC
    _CrtMemCheckpoint(&memStateBegin);
#endif
    g_pApp = this;

    timePrev = ci::app::getElapsedSeconds();
    timeNow  = 0;
    timeSum = 0;

    InitBullet();   // Must init bullet before statics
#if AL_AUDIO
    OpenAL::InitOpenAL();
#endif
    g_pStats = new Stats();
    g_pGui = new mowa::sgui::SimpleGUI(g_pApp, Font(loadResource(RES_SGUI_FONT), 8));
    g_pObjectManager = new ObjectManager();
    g_pInputP1 = new gen::Input(g_pObjectManager->m_pWorld->m_pPlayer);
    InitGamepad();

    this->getWindow()->connectKeyDown(&gen::Input::KeyDown, g_pInputP1);
    this->getWindow()->connectKeyUp(&gen::Input::KeyUp, g_pInputP1);
    this->getWindow()->connectMouseDown(&gen::Input::MouseDown, g_pInputP1);
    this->getWindow()->connectMouseDrag(&gen::Input::MouseDrag, g_pInputP1);
}

void gen::App::shutdown()
{
    if (g_pInputP1)
    {
        delete g_pInputP1;
    }
    if (g_pInputP2)
    {
        delete g_pInputP2;
    }

    delete g_pObjectManager;
    delete g_pGui;
    delete g_pStats;
#if AL_AUDIO
    OpenAL::DestroyOpenAL();
#endif
    DestroyBullet();
    Gamepad_shutdown();

#ifdef _CRTDBG_MAP_ALLOC
    _CrtMemCheckpoint(&memStateEnd);
    _CrtMemDifference(&memStateDiff, &memStateBegin, &memStateEnd);
// TODO: _CrtDumpMemoryLeaks() finds leaks in many third party libraries, but we just want to know about this engine    
// TODO: _CrtMemDumpAllObjectsSince() also finds leaks in Cinder
    _CrtMemDumpAllObjectsSince(&memStateBegin);
#endif
}

void gen::App::update()
{
    timeNow = ci::app::getElapsedSeconds() ;
    timeFrame = timeNow - timePrev;
    timePrev = timeNow;

    g_pStats->UpdateFPS(timeFrame);

    // sound timer has higher resolution than game timer
    g_pObjectManager->UpdateSoundTimer(timeFrame);

    // practical limitation, don't let updates overcome framerate
    if (timeFrame > timeMax)
    {
        timeFrame = timeMax;
    }

    timeSum += timeFrame;

    // update at a rate of timeDiff, regardless of framerate
    while (timeSum >= timeDelta)
    {
        g_pObjectManager->Update(timeDelta);

        if (g_pInputP1)
        {
            g_pInputP1->Update(timeDelta);
        }
        if (g_pInputP2)
        {
            g_pInputP2->Update(timeDelta);
        }

        g_pBtDynamicsWorld->stepSimulation(timeDelta, 1, timeMax);
        g_pStats->UpdateUPS(timeDelta);

        timeSum -= timeDelta;
    }
}

void gen::App::draw()
{
    g_pObjectManager->Draw();
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
    Gamepad_deviceAttachFunc(gen::Input::DeviceAttached, (void *) 0x1);
    Gamepad_deviceRemoveFunc(gen::Input::DeviceRemoved, (void *) 0x2);
    Gamepad_buttonDownFunc(gen::Input::ButtonDown, (void *) 0x3);
    Gamepad_buttonUpFunc(gen::Input::ButtonUp, (void *) 0x4);
    Gamepad_axisMoveFunc(gen::Input::AxisMoved, (void *) 0x5);
    Gamepad_init();
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

    // Register callbacks
    gContactProcessedCallback = (ContactProcessedCallback)PhysicsData::CollisionCallback;
}

void gen::App::DestroyBullet()
{
    delete g_pBtDynamicsWorld;
    delete pBtSolver;
    delete pBtDispatcher;
    delete pBtCollisionConfiguration;
    delete pBtBroadphase;
}

/**************************************
Public functions
**************************************/
void gen::App::ToggleFullscreen()
{
    g_pObjectManager->m_fullscreen = !g_pObjectManager->m_fullscreen;
    this->setFullScreen(g_pObjectManager->m_fullscreen);
    g_pObjectManager->m_fbo = gl::Fbo(g_pApp->getWindowWidth()*ci::app::getWindow()->getContentScale(),
                                        g_pApp->getWindowHeight()*ci::app::getWindow()->getContentScale(),
                                        g_pObjectManager->m_fboFormat);
}