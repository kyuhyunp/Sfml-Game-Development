#include "../Header/TextNode.h"
#include "../Header/ResourceHolder.hpp"
#include "../Header/Utility.hpp"


TextNode::TextNode(const FontHolder& fonts, const std::string& text)
	: mText(fonts.get(Fonts::ID::Sansation), text, 20)
{
	centerOrigin(mText);
}

void TextNode::setString(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mText, states);
}
