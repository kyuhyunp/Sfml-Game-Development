#include "../Header/TitleState.h"

TitleState::TitleState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mText(context.fonts->get(Fonts::ID::Sansation))
	, mShowText(true)
	, mTextEffectTime(sf::Time::Zero)
{
	mText.setString("Press any key to start");
	centerOrigin(mText);
	mText.setPosition(context.window->getView().getCenter());
}

void TitleState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.draw(mBackgroundSprite);

	if (mShowText)
	{
		window.draw(mText);
	} 

}

bool TitleState::update(sf::Time dt)
{
	mTextEffectTime += dt;

	if (mTextEffectTime >= sf::seconds(0.5f))
	{
		mShowText = !mShowText;
		mTextEffectTime = sf::Time::Zero;
	}

	return true;
}

bool TitleState::handleEvent(const sf::Event& event)
{
	if (event.getIf<sf::Event::KeyReleased>())
	{
		requestStackPop();
		requestStackPush(States::Menu);
	}

	return true;
}