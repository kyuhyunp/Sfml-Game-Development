#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "SceneNode.h"
#include "ResourceIdentifiers.h"

#include <SFML/Graphics/Text.hpp>

#include <string>

class TextNode : public SceneNode
{
public:
	explicit TextNode(const FontHolder& fonts, const std::string& text);
	void setString(const std::string& text);

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Text mText;
};

#endif
