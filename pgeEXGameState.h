#ifndef __PGEEXGAMESTATE_H_DEFINED__
#define __PGEEXGAMESTATE_H_DEFINED__

#include <vector>
#include <memory>
#include <map>
#include "DebugLogger.h"

namespace olc {
namespace gamestate {

	class GameState
	{
	public:
		GameState(uint16_t id) : m_id(id), m_status(true) { };
		virtual ~GameState() { };
		GameState(const GameState&) = delete;
		GameState& operator=(const GameState&) = delete;
	public: // Public interface
		inline uint16_t id() const { return m_id; };
		inline bool enabled() const { return m_status; };
		inline bool setEnabled(bool enabled) { m_status = enabled; };

	public: // Virtual, must be implemented by the concrete state
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fElapsedTime) = 0;
		virtual bool OnUserDestroy() = 0;

	private:
		uint16_t m_id;
		bool m_status;
	};

	/**
	 * GameStateManager owns and handles all game states
	 */
	class GameStateManager
	{
	public:
		GameStateManager() { };
		virtual ~GameStateManager() { };
		GameStateManager(const GameStateManager&) = delete;
		GameStateManager& operator=(const GameStateManager&) = delete;

	public: // Public interface
		void addState(std::unique_ptr<GameState> state) {
			// try_emplace does nothing if key already exists in the map
			m_states.try_emplace(state->id(), std::move(state));
		}

		std::uint16_t count() const { return m_states.size(); };

		bool setStateEnabled(uint16_t id, bool enabled) {
			if(m_states.find(id) != m_states.end()) {
				m_states[id]->setEnabled(enabled);
			}
		}

	protected:
		bool OnUserCreate() {
			auto i = m_states.begin();
			bool res = true;
			while(res && i != m_states.end()) {
				res = i->second->OnUserCreate();
				++i;
			}
			return res;
		}

		bool OnUserUpdate(float fElapsedTime) {
			auto i = m_states.begin();
			bool res = true;
			while(res && i != m_states.end()) {
				res = i->second->OnUserUpdate(fElapsedTime);
				++i;
			}
			return res;
		}

		bool OnUserDestroy() {
			auto i = m_states.begin();
			bool res = true;
			while(res && i != m_states.end()) {
				res = i->second->OnUserDestroy();
				++i;
			}
			return res;
		}


	private:
		std::map<uint16_t, std::unique_ptr<GameState>> m_states;
	};

}
}

#endif

