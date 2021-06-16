/**
 * Game State System
 * Author: Erno Pakarinen
 * Email : codesmith.fi@gmail.com
 * Date: 16th of June 2021
 *
 * Implements a multiple state support to be used in applications and
 * especially in games.
 * 
 * See README.md and the example project for details on how to use the 
 * system.
 * 
 * GameStateManager owns and manages sub states. a GameState can be made
 * active using the manager. Each GameState object can have zero or more 
 * layers via derived GameStateLayer class.
 * 
 * For example a 2D top down space shooter game could have following setup
 * 1) a simple setup with states but no layers
 * 
 * GameStateManager
 *		StateMainMenu
 *		StateSettings
 *		StatePlay				<- Renders and handles all for play state
 *		StatePause
 *		StateCredits
 *
 * 2) In more complex situations the rendering could be separated into separate
 * layer as follows:
 * 
 * State-layer setup:
 * GameStateManager
 *		StateMainMenu
 *		StateSettings
 *		StatePlay
 *			LayerEntities		<- for moving entities like player and enemies
 *			LayerParticles		<- Particle effects
 *			LayerBackground		<- Starfields
 *			LayerAsteroids		<- Random asteroid fields that pass by etc.
 *			etc. etc.
 *		StatePause
 *		StateCredits
 * 
 * By separating the game logic and rendering into states and layers makes
 * controlling the game more easier. Of course you could clump everything
 * together in one class but that is quite hard to maintain, unreadable and
 * quite messsy.
 *
 * Add your states to the GameStateManager instance, make one state active and
 * call the GameStateManager::Update(float fTimeElapsed) to do stuff in your
 * active state. To switch state, call activateState(id). Currently state
 * switching is immediate but in the future I will add state transitions 
 * controlled by the GameStateManager. This could be used for example to fade 
 * in/out the current and incoming new state.
 * 
 * ------------------
 * CSMV1.1 - Codesmith License
 * Copyright(c) 1999 - 2021 Erno Pakarinen
 *
 *This licence is based on the MIT license model with very few exceptions.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this softwareand associated documentation files(the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and /or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions :
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.Also the original
 * author shall be credited of the work related to this Software in all
 * software based on or using this Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
		virtual bool update(float fElapsedTime) = 0;

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
		virtual bool update(float fElapsedTime) {
			auto i = m_layers.begin();
			bool res = true;
			while(res && i != m_layers.end()) {
				res = i->get()->update(fElapsedTime);
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

		bool update(float fElapsedTime) {
			bool res = true;

			if(m_currentState) {
				res = m_currentState->update(fElapsedTime);
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

