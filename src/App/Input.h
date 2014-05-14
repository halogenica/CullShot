#pragma once

#include "System/Common.h"
#include <map>

namespace gen
{
// prototypes
class ObjectManager;
class Player;

enum INPUT_KEY
{
#ifdef DEBUG
    KEY_TOGGLE_STATS        = ci::app::KeyEvent::KEY_t,
    KEY_TOGGLE_GUI          = ci::app::KeyEvent::KEY_g,
    KEY_TOGGLE_WIREFRAME    = ci::app::KeyEvent::KEY_h,
    KEY_ADVANCE_GAME_STATE  = ci::app::KeyEvent::KEY_n, 
    KEY_NEXT_LEVEL          = ci::app::KeyEvent::KEY_TAB,
#endif
    KEY_TOGGLE_MUSIC        = ci::app::KeyEvent::KEY_m,
    KEY_TOGGLE_FULLSCREEN   = ci::app::KeyEvent::KEY_f,
    KEY_MOVE_LEFT_P1        = ci::app::KeyEvent::KEY_LEFT,
    KEY_MOVE_RIGHT_P1       = ci::app::KeyEvent::KEY_RIGHT,
    KEY_SHOOT               = ci::app::KeyEvent::KEY_SPACE,
    KEY_SCORE               = ci::app::KeyEvent::KEY_s,
};

enum GAMEPAD
{
#ifdef __APPLE__
// PS3 - vendorID == 1356
    PAD_A           = 14,   // cross
    PAD_B           = 13,   // circle
    PAD_X           = 15,   // square
    PAD_Y           = 12,   // triangle
    PAD_L1          = 10,
    PAD_R1          = 11,
    PAD_SELECT      = 0,
    PAD_START       = 3,
    PAD_L3          = 1,
    PAD_R3          = 2,
//    PAD_L2R2        = 2, // 0 to 1
    // L2_AXIS = 14 // -1 to 1
    // L2_BUTTON = 8
    // R2_AXIS = 15 // -1 to 1
    // R2_BUTTON = 9
    PAD_LSTICK_X    = 0, // -1 left to 1 right
    PAD_LSTICK_Y    = 1, // 1 down to -1 up
    PAD_RSTICK_X    = 2, // -1 left to 1 right
    PAD_RSTICK_Y    = 3, // 1 down to -1 up
    PAD_DPAD_X      = 5, // -1 left or 1 right
    PAD_DPAD_Y      = 6, // 1 down or -1 up
    // DPAD_RIGHT = 4
    // DPAD_UP = 5
    // DPAD_DOWN = 6
    // DPAD_LEFT = 7
#else
    // 360
    PAD_A           = 0,
    PAD_B           = 1,
    PAD_X           = 2,
    PAD_Y           = 3,
    PAD_L1          = 4,
    PAD_R1          = 5,
    PAD_SELECT      = 6,
    PAD_START       = 7,
    PAD_L3          = 8,
    PAD_R3          = 9,
//    PAD_L2R2        = 2, // 0 to 1
    PAD_LSTICK_X    = 0, // -1 left to 1 right
    PAD_LSTICK_Y    = 1, // 1 down to -1 up
    PAD_RSTICK_X    = 4, // -1 left to 1 right
    PAD_RSTICK_Y    = 3, // 1 down to -1 up
    PAD_DPAD_X      = 5, // -1 left or 1 right
    PAD_DPAD_Y      = 6, // 1 down or -1 up
#endif
};
    
struct InputKeyState        // For continuous input
{
    // TODO: These expand to bytes. big deal?
    bool MOVE_UP_STATE;
    bool MOVE_DOWN_STATE;
    bool MOVE_LEFT_STATE;
    bool MOVE_RIGHT_STATE;
};

class Input
{
public:
    Input(Player* pPlayer);
    virtual ~Input(){}
    void Update(float dt);  // For continuous input

    bool KeyDown(ci::app::KeyEvent event);
    bool KeyUp(ci::app::KeyEvent event);
    bool MouseDown(ci::app::MouseEvent event);
    bool MouseDrag(ci::app::MouseEvent event);
    bool MouseUp(ci::app::MouseEvent event);
    bool MouseWheel(ci::app::MouseEvent event);
    bool TouchesBegan(ci::app::TouchEvent event);
    bool TouchesMoved(ci::app::TouchEvent event);
    bool TouchesEnded(ci::app::TouchEvent event);

    // joystick events
    static void onButtonDown(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context);
    static void onButtonUp(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context);
    static void onAxisMoved(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, double timestamp, void * context);
    static void onDeviceAttached(struct Gamepad_device * device, void * context);
    static void onDeviceRemoved(struct Gamepad_device * device, void * context);

    static std::map<unsigned int, Input*> gamepadDeviceMap;

    InputKeyState m_keyState;

    // TODO: should be private
    Player*     m_pPlayer;
    void Shoot();
    
private:
    bool        m_selectState;
    float       m_selectStart;
    ci::Vec2i   m_selectPos;
    float       m_timeTap;
};

}
