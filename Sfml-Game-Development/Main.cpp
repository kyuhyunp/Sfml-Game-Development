#include "Header/ResourceHolder.hpp"
#include <SFML/Graphics.hpp>

int main()
{
	ResourceHolder <sf::Texture, Textures::ID> textures;
	textures.load(Textures::ID::Airplane, "Media/Textures/Eagle.png");

	sf::Sprite playerPlane(textures.get(Textures::ID::Airplane));

	sf::RenderWindow window(sf::VideoMode({ 640, 480 }), "SFML Application", sf::Style::Close);
	
	ResourceHolder <sf::Font, Fonts::ID> fonts;
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
