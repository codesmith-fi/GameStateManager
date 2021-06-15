#ifndef __PGEEXGAMESTATE_H_DEFINED__
#define __PGEEXGAMESTATE_H_DEFINED__

#include <vector>
#include <memory>
#include <map>
#include "DebugLogger.h"

namespace olc {
namespace gamestate {

	class GameStateLayer
	{
	public:
		GameStateLayer(uint16_t id) : m_id(id) { };
		virtual ~GameStateLayer() { };
		GameStateLayer(const GameStateLayer&) = delete;
		GameStateLayer& operator=(const GameStateLayer&) = delete;

	public:
		inline uint16_t id() const { return m_id; };
		inline bool enabled() const { return m_status; };
		inline bool setEnabled(bool enabled) { m_status = enabled; };
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fElapsedTime) = 0;
		virtual bool OnUserDestroy() = 0;

	private:
		uint16_t m_id;
		bool m_status;
	};

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
		bool _onUserCreate() {
			auto i = m_layers.begin();
			bool res = true;
			while(res && i != m_layers.end()) {
				res = i->second->OnUserCreate();
				++i;
			}

			// Only State can end the gameloop, layers can't
			return OnUserCreate();
		}
		bool _onUserUpdate(float fElapsedTime) {
			auto i = m_layers.begin();
			bool res = true;
			while(res && i != m_layers.end()) {
				res = i->second->OnUserUpdate(fElapsedTime);
				++i;
			}

			// Only State can end the gameloop, layers can't
			return OnUserUpdate(fElapsedTime);
		}
		bool _onUserDestroy() {
			auto i = m_layers.begin();
			bool res = true;
			while(res && i != m_layers.end()) {
				res = i->second->OnUserDestroy();
				++i;
			}

			// Only State can end the gameloop, layers can't
			return OnUserDestroy();
		}

	private:
		std::map<uint16_t, std::shared_ptr<GameStateLayer>> m_layers;
		uint16_t m_id;
		bool m_status;

		// GameStateManager can call the internal update hooks which
		// enable the multilayer updating automatically, if layers
		// are used
		friend bool GameStateManager::OnUserCreate();
		friend bool GameStateManager::OnUserDestroy();
		friend bool GameStateManager::OnUserUpdate(float fElapsedTime);
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
			m_states.try_emplace(state->id(), state);
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
		std::map<uint16_t, std::shared_ptr<GameState>> m_states;
	};

}
}

#endif

