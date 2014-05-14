#pragma once

#include "System/Common.h"

namespace gen
{
    
class App : public ci::app::AppNative
{
        
public:
	void prepareSettings(Settings *settings);
    void setup();
    void shutdown();
    void update();
    void draw();
    void resize();

    void ToggleFullscreen();

protected:
    void InitGamepad();
    void InitBullet();
    void UpdateTimestep(float dt);  // updates the simulation
    void DestroyBullet();
};

}