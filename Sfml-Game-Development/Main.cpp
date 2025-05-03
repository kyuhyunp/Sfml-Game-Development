#include "Header/ResourceHolder.hpp"
#include "Header/ResourceIdentifiers.h"
#include <SFML/Graphics.hpp>

int main()
{
	TextureHolder textures;
	textures.load(Textures::ID::Eagle, "Media/Textures/Eagle.png");
	sf::Sprite playerPlane(textures.get(Textures::ID::Eagle));

	sf::RenderWindow window(sf::VideoMode({ 640, 480 }), "SFML Application", sf::Style::Close);
	
	FontHolder fonts;
	fonts.open(Fonts::ID::Sansation, "Media/Sansation.ttf");
	sf::Text text(fonts.get(Fonts::ID::Sansation), "Hello");
	text.setPosition({ 200.f, 200.f });

	while (true) {
		window.clear();
		window.draw(text);

		window.draw(playerPlane);
		window.display();
	}

	

}
