Implements a multiple state system to be used in applications and especially
in games. This is a port from my 2013 .NET C# game engine called smithNgine
which is available here: https://github.com/codesmith-fi/smithngine . This
is just one small sub-component from that engine.

For more details on how to use this component, check the example project
GameStateDemo.cpp. Or read this file.

--- Description

GameStateManager owns and manages sub states. a GameState can be made
active using the manager. Each GameState object can have zero or more 
layers via derived GameStateLayer class.

By separating the game logic and rendering into states and layers makes
controlling the game more easier. Of course you could clump everything
together in one class but that is quite hard to maintain, unreadable and
quite messsy.

For example a 2D top down space shooter game could have following setup
1) a simple setup with states but no layers

GameStateManager
		StateMainMenu
		StateSettings
		StatePlay				<- Renders and handles all for play state
		StatePause
		StateCredits

2) In more complex situations the rendering could be separated into separate
layer as follows:

State-layer setup:
GameStateManager
		StateMainMenu
		StateSettings
		StatePlay
			LayerEntities		<- for moving entities like player and enemies
			LayerParticles		<- Particle effects
			LayerBackground		<- Starfields
			LayerAsteroids		<- Random asteroid fields that pass by etc.
			etc. etc.
		StatePause
		StateCredits

Add your states to the GameStateManager instance, make one state active and
call the GameStateManager::Update(float fTimeElapsed) to do stuff in your
active state. To switch state, call activateState(id). Currently state
switching is immediate but in the future I will add state transitions 
controlled by the GameStateManager. This could be used for example to fade 
in/out the current and incoming new state.

--- Simple setup

1) Create your states inheriting from GameState class, for example:

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
        bool Update(float fElapsedTime) {
            m_pge->Clear(olc::RED);

            // Cause update for all owned layers, if any
            GameState::update(fElapsedTime);

            std::string txt = "Handling state: " + std::to_string(id());
            m_pge->DrawStringDecal(olc::vf2d(10.0f, 10.0f), txt);
            return true;
        }
    private:
        olc::PixelGameEngine* m_pge;
        olc::Renderable m_background;
    };

    class GSDStateSecondary : public GameState
    {
        ...
    }

        ...

In your application setup, create a statemanager and states and add them
to the manager:

	m_stateManager = std::make_unique<GameStateManager>();

    // Pass a state id and what ever other information you need for your
    // concrete state class. In here I pass "this" pointer which in the 
    // included demo project is the instance to Pixel Game Engine for 
    // rendering stuff.
    std::shared_ptr<GameState> state1 = std::make_shared<GSDStatePrimary>(
        0, this);
    std::shared_ptr<GameState> state2 = std::make_shared<GSDStateSecondary>(
        1, this);
    m_stateManager->addState(state1, true);
    m_stateManager->addState(state2);

In your active game loop, do the following (your own game loop might be
different :). In this example the game engine (PGE) calls this whenever 
possible as fast as possible giving a time difference (float) from the 
last call to this same handler. 

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Update state(s). Pass the elapsed time to the statemanager
        // which then calls the active state.
        // GameStateManager::update() will return true or false, depending
        // on should the game application be terminated or continued.
        // The concrete active state you have implemented has to have logic
        // to use this feature if you decide to utilize it. You do not have to
		bool continue_loop = m_stateManager->update(fElapsedTime);

        // This example just switches a state when F1 or F2 is pressed.
        // If state 0 is active and state 0 is tried to activate again, 
        // nothing happens
		if(GetKey(olc::Key::F1).bPressed) {
			m_stateManager->activateState(0);
		} else if(GetKey(olc::Key::F2).bPressed) {
			m_stateManager->activateState(1);
		}
		else if(GetKey(olc::Key::ESCAPE).bPressed) {
			continue_loop = false;
		}
		
		return continue_loop;
	}

That's it. Usage is rather simple and in my opinion it makes controlling your
game logic more straightforward, understandable and simple. Especially when 
concerning different states your game can be in.

--- Final notes

Oh, one more thing, a note about the rendering order.
Currently everything rendered by the state manager is done in the order
you have added sub components in there. For example if you do this:

StateManager
    State1
        S1layer1
        S1layer2
    State2
        S2layer

If your State1 is active then the state will render S1Layer1 first, then S1layer2.
Only the active state will be rendered when GameStateManager::update() is called.

--- Future improvements, feel free to do so if you want :)

This is a first version of this component so there are todo's:

    1) Transition period
    
    activateState() is immediate when called. There is no transition
    period enforced by the statemanager. I will add this feature
    later. You can for example call activateState like this:

        m_stateManager->activateState(nextStateId, 2.0f);

    to cause the current state have a 2 second transition period
    where you can for example do a fadeout for all your graphics or what
    ever.

    I have not yet decided should I have the current state and the next
    state active at the same time when this transition period is active.
    This might be of course be made optional and configurable.

    2) Resources are kept in memory all the time for states and layers

    I might add support where the resources (images, sprites, audio...)
    could be freed when needed and reloaded back when needed. But 
    currently there is no such feature.

    3) Rendering order improvements

    4) Other fine and tandy improvements :)

I hope this proves to be useful for you, if not, do not use it. Comments
and improvement ideas are welcome.

See the blog about this small project here: 

--- Legal notice

Although the demo project uses Pixel Game Engine (by javidx9) 
for the game application setup and rendering, the GameStateSystem and it's 
subclasses have no dependencies nor they use any components from Pixel Game
Engine.

See the appropriate license for Pixel Game Engine in the subfolder

    pge/olcPixelGameEngine.h

Thank you!
