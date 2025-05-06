#include "Header/Game.hpp"
#include "Header/ResourceHolder.hpp"
#include "Header/ResourceIdentifiers.h"

#include <iostream>
#include <SFML/Graphics.hpp>


int main()
{
	try
	{
		FontHolder fonts;
		fonts.open(Fonts::ID::Sansation, "Media/Sansation.ttf");

		Game game(fonts);
		game.run();
	}
	catch (std::exception& e)
	{
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}

}
