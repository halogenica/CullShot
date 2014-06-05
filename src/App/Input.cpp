#include "System/ObjectManager.h"
#include "App/Input.h"
#include "App/App.h"
#include "ObjectData/ObjectData.h"
#include "ObjectData/PhysicsData.h"
#include "ObjectData/RenderData.h"
#include "Game/Player.h"
#include "Game/World.h"

using namespace ci;

#define POLL_ITERATION_INTERVAL 10

std::map<unsigned int, gen::Input*> gen::Input::s_gamepadDeviceMap;

gen::Input::Input(Player* pPlayer) : m_pPlayer(pPlayer), m_keyState()
{
}

void gen::Input::Update(float dt)
{
	static unsigned int iterationsToNextPoll = POLL_ITERATION_INTERVAL;
	
	iterationsToNextPoll--;
	if (iterationsToNextPoll == 0) {
		Gamepad_detectDevices();
		iterationsToNextPoll = POLL_ITERATION_INTERVAL;
	}
    
    Gamepad_processEvents();

    // handle constant "button down" events, like flying

//    if (g_pObjectManager->m_gameState == PLAYING && m_pPlayer->m_alive)
    {
        if (m_keyState.MOVE_LEFT_STATE)
        {
        }
        else if (m_keyState.MOVE_RIGHT_STATE)
        {
        }
    }
    
//    if (g_pObjectManager->m_gameState == PLAYING  && m_pPlayer->m_alive)
    {
        if (m_keyState.MOVE_UP_STATE)
        {
        }
        else if (m_keyState.MOVE_DOWN_STATE)
        {
        }
    }
}

bool gen::Input::KeyDown(ci::app::KeyEvent event)
{
    auto key = event.getCode();
    bool retval1 = true;
    bool retval2 = true;

    // Handle special input regardless of state
    switch (key)
    {
#ifdef DEBUG
        case KEY_TOGGLE_STATS:
            g_pObjectManager->m_displayStats = !g_pObjectManager->m_displayStats;
            break;

        case KEY_TOGGLE_WIREFRAME:
            g_pObjectManager->m_wireframe = !g_pObjectManager->m_wireframe;
            break;

        case KEY_ADVANCE_GAME_STATE:
            switch (g_pObjectManager->m_gameState)
            {
                case TITLE:
                    g_pObjectManager->m_gameState = RULES;
                    break;
                case RULES:
                    g_pObjectManager->m_gameState = PLAYING;
                    break;
                case PLAYING:
                    g_pObjectManager->m_gameState = GAME_OVER;
                    break;
                case GAME_OVER:
                    g_pObjectManager->m_gameState = CREDITS;
                    break;
                case CREDITS:
                    g_pObjectManager->m_gameState = TITLE;
                    break;
                default:
                    break;
            }
            break;

        case KEY_TOGGLE_GUI:
            g_pObjectManager->m_displayGui = !g_pObjectManager->m_displayGui;
            break;
#endif
        case KEY_TOGGLE_FULLSCREEN:
            g_pApp->ToggleFullscreen();
            break;

        case KEY_TOGGLE_MUSIC:
            g_pObjectManager->m_loopPlaytime = g_pObjectManager->m_bgTrackLength;   // trick audio to think it's overdue for play
            g_pObjectManager->m_playMusic = !g_pObjectManager->m_playMusic;
#if CI_AUDIO
            // TODO: how to stop cinder audio playback?
#elif AL_AUDIO
            g_pObjectManager->m_pBgm1->Stop();
            g_pObjectManager->m_pBgm2->Stop();
            g_pObjectManager->m_pBgm3->Stop();
            g_pObjectManager->m_pBgm4->Stop();
#endif
            break;

        case ci::app::KeyEvent::KEY_ESCAPE:
            g_pObjectManager->m_gameState = CREDITS;
            break;

        default:
            retval1 = false;   // the input was not handled
            break;
    }

    // Handle input depending on state
    if (g_pObjectManager->m_gameState == TITLE)
    {
        if (key == KEY_SHOOT)
        {
            g_pObjectManager->m_gameState = RULES;
        }
    }
    else if (g_pObjectManager->m_gameState == RULES)
    {
        if (key == KEY_SHOOT)
        {
            g_pObjectManager->m_gameState = PLAYING;
            g_pObjectManager->Restart();
        }
    }
    else if (g_pObjectManager->m_gameState == PLAYING)
    {
        switch (key)
        {
            case KEY_MOVE_LEFT_P1:
                g_pInputP1->m_keyState.MOVE_LEFT_STATE = true;
                if (g_pObjectManager->m_gameState == PLAYING)
                {
                    g_pInputP1->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0, 0, 5));
                }
                break;

            case KEY_MOVE_RIGHT_P1:
                g_pInputP1->m_keyState.MOVE_RIGHT_STATE = true;
                if (g_pObjectManager->m_gameState == PLAYING)
                {
                    g_pInputP1->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0, 0, -5));
                }
                break;

            case KEY_SHOOT:
                g_pInputP1->m_pPlayer->m_charging = true;
                break;

            case KEY_SCORE:
                g_pObjectManager->m_pWorld->Score();
                break;

            default:
                retval2 = false;   // the input was not handled
                break;
        }
    }
    else if (g_pObjectManager->m_gameState == GAME_OVER)
    {
        if (key == KEY_SHOOT)
        {
            if (g_pObjectManager->m_gameoverTimer <= 0)
            {
                g_pObjectManager->m_gameState = PLAYING;
                g_pObjectManager->Restart();
            }
        }
    }

    return (retval1 || retval2);    // was the input handled
}

bool gen::Input::KeyUp(ci::app::KeyEvent event)
{
    bool retval = true;

    switch (event.getCode())
    {
        case KEY_MOVE_LEFT_P1:
            g_pInputP1->m_keyState.MOVE_LEFT_STATE = false;
            break;

        case KEY_MOVE_RIGHT_P1:
            g_pInputP1->m_keyState.MOVE_RIGHT_STATE = false;
            break;

        case KEY_SHOOT:
            g_pInputP1->m_pPlayer->m_charging = false;
            g_pInputP1->m_pPlayer->Shoot();
            break;

        default:
            retval = false;   // the input was not handled
            break;
    }

    if (g_pInputP1->m_keyState.MOVE_LEFT_STATE == true)
    {
        g_pInputP1->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0, 0, 5));
    }
    else if (g_pInputP1->m_keyState.MOVE_RIGHT_STATE == true)
    {
        g_pInputP1->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0, 0, -5));
    }
    else
    {
        g_pInputP1->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0,0,0));
    }

    return retval;
}

bool gen::Input::MouseDown(ci::app::MouseEvent event)
{
    return true;
}

bool gen::Input::MouseDrag(ci::app::MouseEvent event)
{
    return false;
}

bool gen::Input::MouseUp(ci::app::MouseEvent event)
{
    return false;
}

bool gen::Input::MouseWheel(ci::app::MouseEvent event)
{
    return false;
}

bool gen::Input::TouchesBegan(ci::app::TouchEvent event)
{
    return false;
}

bool gen::Input::TouchesMoved(ci::app::TouchEvent event)
{
    return false;
}

bool gen::Input::TouchesEnded(ci::app::TouchEvent event)
{
    return false;
}

/**************************************
Gamepad code
**************************************/
#ifdef DEBUG
bool verbose = true;
#else
bool verbose = false;
#endif

void gen::Input::ButtonDown(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context) 
{
	if (verbose) 
    {
        console() << "Button " << buttonID << " down on device " << device->deviceID << " at " << timestamp << " with context " << context << std::endl;
	}

    gen::Input* pPlayerInput = NULL;
    pPlayerInput = s_gamepadDeviceMap[device->deviceID];

    if (pPlayerInput)
    {
        if (g_pObjectManager->m_gameState == TITLE)
        {
            if (buttonID == PAD_A)
            {
                g_pObjectManager->m_gameState = RULES;
            }
        }
        else if (g_pObjectManager->m_gameState == RULES)
        {
            if (buttonID == PAD_A)
            {
                g_pObjectManager->m_gameState = PLAYING;
                g_pObjectManager->Restart();
            }
        }
        else if (g_pObjectManager->m_gameState == PLAYING)
        {
            if (buttonID == PAD_A)
            {
                pPlayerInput->m_pPlayer->m_charging = true;
            }
            if (buttonID == PAD_X)
            {
                g_pObjectManager->m_pWorld->Score();
            }
        }
        else if (g_pObjectManager->m_gameState == GAME_OVER)
        {
            if (buttonID == PAD_A)
            {
                if (g_pObjectManager->m_gameoverTimer <= 0)
                {
                    g_pObjectManager->m_gameState = PLAYING;
                    g_pObjectManager->Restart();
                }
            }
        }
    }
}

void gen::Input::ButtonUp(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context) 
{
	if (verbose) 
    {
        console() << "Button " << buttonID << " up on device " << device->deviceID << " at " << timestamp << " with context " << context << std::endl;
	}

    gen::Input* pPlayerInput = NULL;
    pPlayerInput = s_gamepadDeviceMap[device->deviceID];

    if (pPlayerInput)
    {
        if (g_pObjectManager->m_gameState == PLAYING)
        {
            if (buttonID == PAD_A)
            {
                pPlayerInput->m_pPlayer->m_charging = false;
                pPlayerInput->m_pPlayer->Shoot();
            }
        }
    }
}

void gen::Input::AxisMoved(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, double timestamp, void * context) 
{
	if (verbose && axisID < 38) // PS3 controllers register motion sensors as axes starting around 38, so mute them
    {
        if (value <= -0.3f || value >= 0.3f)
        {
        console() << "Axis " << axisID << " moved from " << lastValue <<" to " << value << " on device " << device->deviceID << " at " << timestamp << " with context " << context << std::endl;
        }
	}

    gen::Input* pPlayerInput = NULL;
    pPlayerInput = s_gamepadDeviceMap[device->deviceID];

    if (pPlayerInput)
    {
        if (g_pObjectManager->m_gameState == PLAYING)
        {
            if (axisID == PAD_LSTICK_X)
            {
                if (value <= -0.2f)
                {
                    pPlayerInput->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0, 0, value * -5));
                }
                else if (value >= 0.2)
                {
                    pPlayerInput->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0, 0, value * -5));
                }
                else
                {
                    pPlayerInput->m_pPlayer->m_pShip->m_pPhysicsData->m_pBtRigidBody->setAngularVelocity(btVector3(0,0,0));
                }
            }
        }
    }
}

void gen::Input::DeviceAttached(struct Gamepad_device * device, void * context) 
{
	if (verbose) 
    {
        console() << "Device ID " << device->deviceID << " attached (vendor = " << device->vendorID <<"; product = " << device->productID << ") with context" << context << std::endl;
    }

    if (s_gamepadDeviceMap.size() == 0)
    {
        s_gamepadDeviceMap[device->deviceID] = g_pInputP1;
    }
    else if (s_gamepadDeviceMap.size() == 1)
    {
        s_gamepadDeviceMap[device->deviceID] = g_pInputP2;
    }
    else
    {
        ci::app::console() << "Only 2 players are supported!" << std::endl;
    }
}

void gen::Input::DeviceRemoved(struct Gamepad_device * device, void * context) 
{
	if (verbose) 
    {
        console() << "Device ID " << device->deviceID << " removed with context " << context << std::endl;
	}
}