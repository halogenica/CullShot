#include "System/Common.h"
#include "System/ObjectManager.h"
#include "Utils/Stats.h"

#define STRINGIFY( name ) # name

const char* gameStateNames [] =
{
    STRINGIFY(TITLE),
    STRINGIFY(RULES),
    STRINGIFY(PLAYING),
    STRINGIFY(GAME_OVER),
    STRINGIFY(CREDITS),
};

using namespace std;
using namespace gen;

Stats::Stats()
: m_FPS(0.0f), m_UPS(0.0f), m_milliSecPerFrame(0.0f), m_numTris(0), m_numVertices(0), m_numRigidBodies(0), m_cursorX(0), m_cursorY(0)
{
    ASSERT(NUM_GAME_STATES == (sizeof(gameStateNames) / sizeof (gameStateNames[0])));
    m_textFont = ci::gl::TextureFont::create(ci::Font(ci::app::loadResource(RES_STATS_FONT), 24)); // pf_tempesta_seven.ttf has problems with new line
    m_textBoundsRect = ci::Rectf(40.f, m_textFont->getAscent() + 20.f, getWindowWidth(), getWindowHeight());
}

Stats::~Stats()
{
}

void Stats::OnLostDevice()
{
}

void Stats::OnResetDevice()
{
}

void Stats::AddVertices(unsigned int n)         {m_numVertices += n;            }
void Stats::SubVertices(unsigned int n)         {m_numVertices -= n;            }
void Stats::AddTriangles(unsigned int n)        {m_numTris += n;                }
void Stats::SubTriangles(unsigned int n)        {m_numTris -= n;                }
void Stats::AddRigidBodies(unsigned int n)      {m_numRigidBodies += n;         }
void Stats::SubRigidBodies(unsigned int n)      {m_numRigidBodies -= n;         }
void Stats::SetTriCount(unsigned int n)         {m_numTris = n;                 }
void Stats::SetVertexCount(unsigned int n)      {m_numVertices = n;             }
void Stats::SetRigidBodyCount(unsigned int n)   {m_numRigidBodies = n;          }
void Stats::SetCursorPosition(float x, float y) {m_cursorX = x; m_cursorY = y;  }

void Stats::UpdateFPS(float dt)
{
	// Make static so that their values persist accross function calls.
	static float numFrames   = 0.0f;
	static float timeElapsed = 0.0f;

	// Increment the frame count.
	numFrames += 1.0f;

	// Accumulate how much time has passed.
	timeElapsed += dt;

	// Has one second passed?--we compute the frame statistics once 
	// per second.  Note that the time between frames can vary so 
	// these stats are averages over a second.
	if( timeElapsed >= 1.0f )
	{
		// Frames Per Second = numFrames / timeElapsed,
		// but timeElapsed approx. equals 1.0, so 
		// frames per second = numFrames.

		m_FPS = numFrames;

		// Average time, in miliseconds, it took to render a single frame.
		m_milliSecPerFrame = 1000.0f / m_FPS;

		// Reset time counter and frame count to prepare for computing
		// the average stats over the next second.
		timeElapsed = 0.0f;
		numFrames   = 0.0f;
	}
}

void Stats::UpdateUPS(float dt)
{
	// Make static so that their values persist accross function calls.
	static float numUpdates  = 0.0f;
	static float timeElapsed = 0.0f;

	// Increment the frame count.
	numUpdates += 1.0f;

	// Accumulate how much time has passed.
	timeElapsed += dt;

	// Has one second passed?--we compute the frame statistics once 
	// per second.  Note that the time between frames can vary so 
	// these stats are averages over a second.
	if( timeElapsed >= 1.0f )
	{
		// Frames Per Second = numFrames / timeElapsed,
		// but timeElapsed approx. equals 1.0, so 
		// frames per second = numFrames.

		m_UPS = numUpdates;

		// Average time, in miliseconds, it took to render a single frame.
		m_milliSecPerUpdate = 1000.0f / m_UPS;

		// Reset time counter and frame count to prepare for computing
		// the average stats over the next second.
		timeElapsed = 0.0f;
		numUpdates   = 0.0f;
	}
}

void Stats::Display()
{
    stringstream displayString;
    displayString << "Frames Per Second: "  << setiosflags(ios::fixed) << setprecision(2) << m_FPS << endl;
    displayString << "Updates Per Second: " << setiosflags(ios::fixed) << setprecision(2) << m_UPS << endl;
    displayString << "Ms Per Frame: "       << setiosflags(ios::fixed) << setprecision(4) << m_milliSecPerFrame << endl;
    displayString << "Triangle Count: "     << m_numTris << endl;
    displayString << "Vertex Count: "       << m_numVertices << endl;
    displayString << "Rigid Bodies: "       << m_numRigidBodies << endl;
    displayString << "Game State: "         << gameStateNames[g_pObjectManager->m_gameState] << endl;
    
    gl::pushMatrices();
    gl::setMatricesWindow(getWindowSize());
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::enableAlphaBlending();

    ci::gl::color(Color::white());
    m_textFont->drawStringWrapped(displayString.str(), m_textBoundsRect, ci::Vec2f(0,0));

    gl::disableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::color(ColorA(1,1,1,1));
    gl::popMatrices();
}