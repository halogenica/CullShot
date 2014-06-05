#pragma once

#include "System/Common.h"

namespace gen
{
    
class App : public ci::app::AppNative
{
        
public:
    void ToggleFullscreen();

private:
    // Functions implicitly called by Cinder
	void prepareSettings(Settings *settings);
    void setup();
    void shutdown();
    void update();
    void draw();
    void resize();

    // Helper functions for intializaiton/destruction
    void InitGamepad();
    void InitBullet();
    void DestroyBullet();
};

}