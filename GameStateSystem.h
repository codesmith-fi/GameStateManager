/**
 * Game State System
 * Date: June 16th 2021
 * Author: Erno Pakarinen (codesmith.fi@gmail.com)
 */

#ifndef __GAMESTATESYSTEM_H_DEFINED__
#define __GAMESTATESYSTEM_H_DEFINED__

#include <memory>
#include <vector>
#include "DebugLogger.h"

namespace codesmith {
namespace gamestate {
	class GameStateLayer
	{
	public:
		GameStateLayer(uint16_t id, bool enabled) : 
			m_id(id), m_enabled(true) { };
		virtual ~GameStateLayer() { };
		GameStateLayer(const GameStateLayer&) = delete;
		GameStateLayer& operator=(const GameStateLayer&) = delete;

	public:
		inline uint16_t id() const { return m_id; };
		inline bool enabled() const { return m_enabled; };
		inline bool setEnabled(bool enabled) { m_enabled = enabled; };
		virtual bool Update(float fElapsedTime) = 0;

	private:
		uint16_t m_id;
		bool m_enabled;
	};

	class GameState
	{
	public:
		GameState(uint16_t id) : m_id(id) { };
		virtual ~GameState() { };
		GameState(const GameState&) = delete;
		GameState& operator=(const GameState&) = delete;
		std::size_t layers() const { return m_layers.size(); };
	public: // Public interface
		inline uint16_t id() const { return m_id; };

	public: 
		virtual bool Update(float fElapsedTime) {
			auto i = m_layers.begin();
			bool res = true;
			while(res && i != m_layers.end()) {
				res = i->get()->Update(fElapsedTime);
				++i;
			}
			return true;
		}

	private:
		std::vector<std::shared_ptr<GameStateLayer>> m_layers;
		uint16_t m_id;
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
		void addState(std::shared_ptr<GameState> state, bool isDefault = false) {
			auto st = std::find(m_states.begin(), m_states.end(), state);
			if(st == m_states.end()) {
				m_states.emplace_back(state);
				LOG_INFO() << "Added new state, id = " << state->id();
				if(isDefault) {
					LOG_INFO() << " State made default initial state";
					m_currentState = state.get();
				}
			}
		}

		void activateState(uint16_t id) {
			if(m_currentState->id() != id) {
				for(auto s : m_states) { 
					if(s->id() == id) {
						m_currentState = s.get();
						LOG_INFO() << "Activated state " << m_currentState->id();
					}
				}
			}
		}

		std::size_t count() const { return m_states.size(); };

		bool Update(float fElapsedTime) {
			bool res = true;

			if(m_currentState) {
				res = m_currentState->Update(fElapsedTime);
			}
			return res;
		}

	private:
		std::vector<std::shared_ptr<GameState>> m_states;
		// Hard pointer used, all states are owned and the life cycle is managed 
		// by this class.
		GameState* m_currentState = nullptr;
	};

} // namespace gamestate
} // namespace codesmith

#endif // __GAMESTATESYSTEM_H_DEFINED__

