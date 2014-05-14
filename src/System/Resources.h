#pragma once
#include "cinder/CinderResources.h"

//#define RES_MY_RES			        CINDER_RESOURCE( ../resources/, image_name.png, 128, IMAGE )
#define RES_SGUI_FONT                   CINDER_RESOURCE( ../../../libraries/cinder_0.8.6_dev/blocks/SimpleGUI/resources/, pf_tempesta_seven.ttf, 128, CI_FONT )
#define RES_STATS_FONT                  CINDER_RESOURCE( ../resources/, ProggySquare.ttf,               129, CI_FONT )

#define RES_CUBE_OBJ                    CINDER_RESOURCE( ../resources/, cube.obj,                       130, CI_OBJ )
#define RES_SPHERE_OBJ                  CINDER_RESOURCE( ../resources/, sphere.obj,                     131, CI_OBJ )
#define RES_SQUARE_OBJ                  CINDER_RESOURCE( ../resources/, square.obj,                     132, CI_OBJ )
#define RES_TORUS_OBJ                   CINDER_RESOURCE( ../resources/, torus.obj,                      133, CI_OBJ )
#define RES_CIRCLE_OBJ                  CINDER_RESOURCE( ../resources/, circle.obj,                     134, CI_OBJ )
#define RES_SHIP_OBJ                    CINDER_RESOURCE( ../resources/, ship.obj,                       135, CI_OBJ )
#define RES_SHIP_SPIN_OBJ               CINDER_RESOURCE( ../resources/, ship_spin.obj,                  136, CI_OBJ )
#define RES_RING_OBJ                    CINDER_RESOURCE( ../resources/, ring.obj,                       137, CI_OBJ )
#define RES_RING_DASH_OBJ               CINDER_RESOURCE( ../resources/, ring_dash.obj,                  138, CI_OBJ )
#define RES_RING_DASH2_OBJ              CINDER_RESOURCE( ../resources/, ring_dash2.obj,                 139, CI_OBJ )
#define RES_RING_POINT_OBJ              CINDER_RESOURCE( ../resources/, ring_point.obj,                 140, CI_OBJ )
#define RES_CIRCLE_DASH_OBJ             CINDER_RESOURCE( ../resources/, circle_dash.obj,                141, CI_OBJ )
#define RES_CIRCLE_DASH2_OBJ            CINDER_RESOURCE( ../resources/, circle_dash2.obj,               142, CI_OBJ )

#define RES_TITLE_TEX                   CINDER_RESOURCE( ../resources/, title.png,                      150, CI_TEX )
#define RES_RULES_TEX                   CINDER_RESOURCE( ../resources/, rules.png,                      151, CI_TEX )
#define RES_CREDITS_TEX                 CINDER_RESOURCE( ../resources/, credits.png,                    152, CI_TEX )
#define RES_GAME_OVER_TEX               CINDER_RESOURCE( ../resources/, gameover.png,                   153, CI_TEX )
#define RES_CONTINUE_TEX                CINDER_RESOURCE( ../resources/, continue.png,                   154, CI_TEX )



//#ifdef __APPLE__    // Apple devices only load MP3's via cinder
//#define RES_HIT1_SOUND                CINDER_RESOURCE( ../resources/, sound.mp3,                      160, CI_SOUND )
//#else               // Windows only loads WAV's via cinder
#define RES_SFX_CHARGE_SOUND            CINDER_RESOURCE( ../resources/, sfx_charge.wav,                 160, CI_SOUND )
#define RES_SFX_SHOOT_SOUND             CINDER_RESOURCE( ../resources/, sfx_shoot.wav,                  161, CI_SOUND )
#define RES_SFX_SCORE_HIT_SOUND         CINDER_RESOURCE( ../resources/, sfx_score_hit.wav,              162, CI_SOUND )
#define RES_SFX_SCORE_MISS_SOUND        CINDER_RESOURCE( ../resources/, sfx_score_miss.wav,             163, CI_SOUND )

#define RES_BGM1_SOUND                  CINDER_RESOURCE( ../resources/, bgm1.wav,                       170, CI_SOUND )
#define RES_BGM2_SOUND                  CINDER_RESOURCE( ../resources/, bgm2.wav,                       171, CI_SOUND )
#define RES_BGM3_SOUND                  CINDER_RESOURCE( ../resources/, bgm3.wav,                       172, CI_SOUND )
#define RES_BGM4_SOUND                  CINDER_RESOURCE( ../resources/, bgm4.wav,                       173, CI_SOUND )
//#endif

#define RES_VERT_PASSTHROUGH_SHADER     CINDER_RESOURCE( ../resources/, vert_passthrough.glsl,          190, CI_SHADER )
#define RES_FRAG_PASSTHROUGH_SHADER     CINDER_RESOURCE( ../resources/, frag_passthrough.glsl,          191, CI_SHADER )
#define RES_VERT_FLATSHADE_SHADER       CINDER_RESOURCE( ../resources/, vert_flatshade.glsl,            192, CI_SHADER )
#define RES_FRAG_FLATSHADE_SHADER       CINDER_RESOURCE( ../resources/, frag_flatshade.glsl,            193, CI_SHADER )
#define RES_FRAG_DIAGONALS_SHADER       CINDER_RESOURCE( ../resources/, frag_diagonals.glsl,            194, CI_SHADER )
#define RES_FRAG_DIAGONALS2_SHADER      CINDER_RESOURCE( ../resources/, frag_diagonals2.glsl,           195, CI_SHADER )

#define RES_SCORE_FONT                  CINDER_RESOURCE( ../resources/, orbitron-light.ttf,             200, CI_FONT )
