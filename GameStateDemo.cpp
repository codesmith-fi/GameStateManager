/**
 * Demo game application made with Pixel Game Engine (by javid9x, see the 
 * header olcPixelGameEngine.h in pge/ folder within this project for 
 * copyrights and license(s) of the PGE component).
 * 
 * The supplied GameStateSystem does not depend on PGE, all dependencies
 * are here on the Game application side which is implemented with PGE.
 */

#include <iostream>
#include <memory>

#define OLC_PGE_APPLICATION
#include "pge/olcPixelGameEngine.h"

#include "GameStateSystem.h"
#include "DebugLogger.h"

using namespace codesmith::gamestate;

/**
 * States and layers
 * This demo does not use layers within states. Each state handles rendering 
 * of the state.
 */

class GSDStatePrimary : public GameState
{
public:
	GSDStatePrimary(uint16_t id, olc::PixelGameEngine* pge) :
		GameState(id), m_pge(pge) { 
		m_background.Load("assets/desert.png");
		LOG_INFO() << "Constructed state " << id;
	};
	GSDStatePrimary() = delete;
	~GSDStatePrimary() = default;
	bool update(float fElapsedTime) override {
		m_pge->Clear(olc::RED);

		// Cause update for all owned layers, if any
		GameState::update(fElapsedTime);

		std::string txt = "Rendering state: " + std::to_string(id());
		m_pge->DrawStringDecal(olc::vf2d(10.0f, 10.0f), txt);
		return true;
	}
private:
	olc::PixelGameEngine* m_pge;
	olc::Renderable m_background;
};

class GSDStateSecondary : public GameState
{
public:
	GSDStateSecondary(uint16_t id, olc::PixelGameEngine* pge) :
		GameState(id), m_pge(pge) {
		m_background.Load("assets/snowmountain.png");
		LOG_INFO() << "Constructed state " << id;
	}

	~GSDStateSecondary() = default;

	bool update(float fElapsedTime) override {
		m_pge->Clear(olc::GREEN);

		// Cause update for all owned layers, if any
		GameState::update(fElapsedTime);

		std::string txt = "Rendering state: " + std::to_string(id());
		m_pge->DrawStringDecal(olc::vf2d(10.0f, 10.0f), txt);
		return true;
	}
private:
	olc::PixelGameEngine* m_pge;
	olc::Renderable m_background;
};

class GSDStatePause: public GameState
{
public:
	GSDStatePause(uint16_t id, olc::PixelGameEngine* pge) :
		GameState(id), m_pge(pge) {
		m_background.Load("assets/desert.png");
		m_background.Load("assets/paused.png");
		LOG_INFO() << "Constructed state " << id;
	}

	~GSDStatePause() = default;

	bool update(float fElapsedTime) override {
		m_pge->Clear(olc::BLUE);

		// Cause update for all owned layers, if any
		GameState::update(fElapsedTime);

		std::string txt = "Rendering  state: " + std::to_string(id());
		m_pge->DrawStringDecal(olc::vf2d(10.0f, 10.0f), txt);
		return true;
	}
private:
	olc::PixelGameEngine* m_pge;
	olc::Renderable m_background;
	olc::Renderable m_pauselogo;
};

/**
 * The game application, we use PixelGameEngine for this demo.
 * It is great btw, thank you javidx9!
 */
class PGEApplication : public olc::PixelGameEngine
{
public:
	PGEApplication()
	{
		LOG_INFO() << "PGEApplication created";
		sAppName = "PGEApplication - SpriteBatch demo";
	}

	virtual ~PGEApplication() {
		LOG_INFO() << "PGEApplication destroyed";
	}

public:
	bool OnUserCreate() override
	{
		LOG_INFO() << "PGEApplication::OnUserCreate() initializing";
		m_stateManager = std::make_unique<GameStateManager>();
		std::shared_ptr<GameState> state1 = std::make_shared<GSDStatePrimary>(0, this);
		std::shared_ptr<GameState> state2 = std::make_shared<GSDStateSecondary>(1, this);
		std::shared_ptr<GameState> state3 = std::make_shared<GSDStatePause>(2, this);
		m_stateManager->addState(state1, true);
		m_stateManager->addState(state2);
		m_stateManager->addState(state3);

		LOG_INFO() << m_stateManager->count() << 
			" are now managed by the GameStateManager";
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Update state(s)
		bool continue_loop = m_stateManager->update(fElapsedTime);

		std::string txt = "Press F1, F2 and F3 to switch states, ESC to quit";
		DrawStringDecal(olc::vf2d(10.0f, 25.0f), txt, olc::BLUE);

		if(GetKey(olc::Key::F1).bPressed) {
			m_stateManager->activateState(0);
		} else if(GetKey(olc::Key::F2).bPressed) {
			m_stateManager->activateState(1);
		} else if(GetKey(olc::Key::F3).bPressed) {
			m_stateManager->activateState(2);
		}
		else if(GetKey(olc::Key::ESCAPE).bPressed) {
			continue_loop = false;
		}
		
		return continue_loop;
	}

	/**
	 * If OnUserDestroy() returns false, the application will not exit,
	 * it keeps running.
	 */
	bool OnUserDestroy() override
	{
		LOG_INFO() << "PGEApplication::OnUserDestroy()";
		return true;
	}
private:
	std::unique_ptr<GameStateManager> m_stateManager;
};

int main()
{
	PGEApplication demo;
	if(demo.Construct(1024, 768, 1, 1))
		demo.Start();
	return 0;
}
