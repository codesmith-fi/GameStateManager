#include <iostream>
#include <memory>

#define OLC_PGE_APPLICATION
#include "pge/olcPixelGameEngine.h"
#include "GameStateSystem.h"
#include "DebugLogger.h"

/**
 * States and layers
 */
using namespace codesmith::gamestate;

class GSDStatePrimary : public GameState
{
public:
	GSDStatePrimary(uint16_t id) : GameState(id) { };
	~GSDStatePrimary() = default;

	bool Update(float fElapsedTime) {
		return true;
	}
};

class GSDStateSecondary : public GameState
{
public:
	GSDStateSecondary(uint16_t id) : GameState(id) { };
	~GSDStateSecondary() = default;

	bool Update(float fElapsedTime) {
		return true;
	}
};

class GSDStatePause: public GameState
{
public:
	GSDStatePause(uint16_t id) : GameState(id) { };
	~GSDStatePause() = default;

	bool Update(float fElapsedTime) {
		return true;
	}
};


/**
 *
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
		auto state1 = std::make_shared<GameState>(0);
		auto state2 = std::make_shared<GameState>(1);
		auto state3 = std::make_shared<GameState>(2);
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
		m_stateManager->Update(fElapsedTime);

		if(GetKey(olc::Key::F1).bPressed) {
			m_stateManager->activateState(0);
		} else if(GetKey(olc::Key::F2).bPressed) {
			m_stateManager->activateState(1);
		} else if(GetKey(olc::Key::F3).bPressed) {
			m_stateManager->activateState(2);
		}

		return true;
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
