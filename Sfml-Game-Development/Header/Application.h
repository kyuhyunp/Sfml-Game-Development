#ifndef APPLICATION_H
#define APPLICATION_H

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"
#include "Player.h"
#include "StateStack.h"
#include "Utility.hpp"
#include "State.h"
#include "StateIdentifiers.h"
#include "TitleState.h"
#include "GameState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "SettingsState.h"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>


class Application
{
public:
	Application(FontHolder& fonts);

	// Delete copy constructor and assignment operator
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void run();

private:
	void processInput();
	void update(sf::Time dt);
	void render();

	void updateStatistics(sf::Time dt);
	void registerStates();


	static const sf::Time TimePerFrame;

	sf::RenderWindow mWindow;
	TextureHolder mTextures;
	FontHolder& mFonts;
	Player mPlayer;

	StateStack mStateStack;
	
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	size_t mStatisticsNumFrames;
};

#endif

