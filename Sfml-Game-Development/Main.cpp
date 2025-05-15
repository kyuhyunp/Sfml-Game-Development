#include "Header/Application.h"
#include "Header/ResourceHolder.hpp"
#include "Header/ResourceIdentifiers.h"

#include <iostream>
#include <stdexcept>


int main()
{
	try
	{
		FontHolder fonts;
		fonts.open(Fonts::ID::Sansation, "Media/Sansation.ttf");
		Application app(fonts);
		app.run();
	}
	catch (std::exception& e)
	{
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}
}
