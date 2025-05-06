#ifndef BOOK_GAME_HPP
#define BOOK_GAME_HPP

#include "World.h"
#include <SFML/Graphics.hpp>


class Game
{
	public:
								// Delete copy constructor and assignment operator
								Game(const Game&) = delete;
								Game& operator=(const Game&) = delete;

								Game(const FontHolder& fonts);
		void					run();

		

	private:
		void					processEvents();
		void					update(sf::Time elapsedTime);
		void					render();

		void					updateStatistics(sf::Time elapsedTime);	
		void					handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
		

	private:
		static const float		PlayerSpeed;
		static const sf::Time	TimePerFrame;

		sf::RenderWindow		mWindow;
		World					mWorld;
		const FontHolder&		mFonts;
		sf::Text				mStatisticsText;
		sf::Time				mStatisticsUpdateTime;
		std::size_t				mStatisticsNumFrames;
};

#endif // BOOK_GAME_HPP
