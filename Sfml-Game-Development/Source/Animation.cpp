#include "../Header/Animation.h"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

Animation::Animation(const sf::Texture& texture)
	: mSprite(texture)
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mRepeat(false)
{
}

void Animation::setTexture(const sf::Texture& texture)
{
	mSprite.setTexture(texture);
}

const sf::Texture* Animation::getTexture() const
{
	return &mSprite.getTexture();
}

void Animation::setFrameSize(sf::Vector2i frameSize)
{
	mFrameSize = frameSize;
}

sf::Vector2i Animation::getFrameSize() const
{
	return mFrameSize;
}

void Animation::setNumFrames(std::size_t numFrames)
{
	mNumFrames = numFrames;
}

std::size_t Animation::getNumFrames() const
{
	return mNumFrames;
}

void Animation::setDuration(sf::Time duration)
{
	mDuration = duration;
}

sf::Time Animation::getDuration() const
{
	return mDuration;
}

void Animation::setRepeating(bool flag)
{
	mRepeat = flag;
}

bool Animation::isRepeating() const
{
	return mRepeat;
}

void Animation::restart()
{
	mCurrentFrame = 0;
}

bool Animation::isFinished() const
{
	return mCurrentFrame >= mNumFrames;
}

// It’s safer to have the scene node define its own bounds consistently
sf::FloatRect Animation::getLocalBounds() const
{
	// Doesn't have much to do with the mSprite's transformation
	return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(mFrameSize));
}

sf::FloatRect Animation::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

void Animation::update(sf::Time dt)
{
	sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
	mElapsedTime += dt;

	sf::Vector2u textureBounds = mSprite.getTexture().getSize();
	sf::IntRect textureRect = mSprite.getTextureRect();

	if (mCurrentFrame == 0)
	{
		textureRect = sf::IntRect({ 0, 0 }, mFrameSize);
	}

	while (mElapsedTime >= timePerFrame && (mCurrentFrame < mNumFrames || mRepeat))
	{
		textureRect.position.x += textureRect.size.x;
		if (textureRect.position.x + textureRect.size.x > textureBounds.x)
		{
			textureRect.position.x = 0;
			textureRect.position.y += textureRect.size.y;
		}

		mElapsedTime -= timePerFrame;

		if (mRepeat)
		{
			mCurrentFrame = (mCurrentFrame + 1) % mNumFrames;
			if (mCurrentFrame == 0)
			{
				textureRect = sf::IntRect({ 0, 0 }, mFrameSize);
			}
		}
		else
		{
			++mCurrentFrame;
		}
	}

	mSprite.setTextureRect(textureRect);
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
}
