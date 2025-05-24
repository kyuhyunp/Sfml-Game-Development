#include "../Header/Label.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


namespace GUI
{
	Label::Label(const std::string& text, const FontHolder& fonts)
		: mText(fonts.get(Fonts::ID::Sansation), text, 16)
	{
	}

	bool Label::isSelectable() const
	{
		return false;
	}

	void Label::setText(const std::string& text)
	{
		mText.setString(text);
	}

	void Label::handleEvent(const sf::Event& event)
	{
	}

	void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{ 
		states.transform *= getTransform();
		target.draw(mText, states);
	}
}