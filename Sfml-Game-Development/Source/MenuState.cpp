#include "../Header/MenuState.h"


MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mOptions()
	, mOptionIndex(0)
{
	mOptions.reserve(2);
	sf::RenderWindow& window = *context.window;

	mOptions.emplace_back(context.fonts->get(Fonts::ID::Sansation));
	sf::Text& playOption = mOptions[Play];
	playOption.setString("Play");
	centerOrigin(mOptions[Play]);
	playOption.setPosition(window.getView().getCenter());

	mOptions.emplace_back(context.fonts->get(Fonts::ID::Sansation));
	sf::Text& exitOption = mOptions[Exit];
	exitOption.setString("Exit");
	centerOrigin(exitOption);
	exitOption.setPosition(playOption.getPosition() + sf::Vector2f(0.f, 30.f));

	updateOptionText();
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	
	window.setView(window.getDefaultView());
	window.draw(mBackgroundSprite);

	for (const sf::Text& text : mOptions)
	{
		window.draw(text);
	}
}

bool MenuState::update(sf::Time dt)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPress->code == sf::Keyboard::Key::Enter)
		{
			requestStackPop();

			if (mOptionIndex == Play)
			{
				requestStackPush(States::Game);
			}
			// Else, Exit
		}
		else if (keyPress->code == sf::Keyboard::Key::Up)
		{
			if (mOptionIndex > 0) 
			{
				--mOptionIndex;
			}
			else 
			{
				mOptionIndex = mOptions.size() - 1;
			}

			updateOptionText();
		}
		else if (keyPress->code == sf::Keyboard::Key::Down)
		{
			if (mOptionIndex < mOptions.size() - 1)
			{
				++mOptionIndex;
			}
			else 
			{
				mOptionIndex = 0;
			}

			updateOptionText();
		}

		// Doesn't matter if any other key is pressed (One State)
		return true;
	}

	return false;
}

void MenuState::updateOptionText()
{
	if (mOptions.empty())
	{
		return;
	}

	for (sf::Text& text : mOptions)
	{
		text.setFillColor(sf::Color::White);
	}

	mOptions[mOptionIndex].setFillColor(sf::Color::Red);
}