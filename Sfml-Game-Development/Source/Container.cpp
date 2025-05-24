#include "../Header/Container.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


namespace GUI
{
	Container::Container()
		: mChildren()
		, mSelectedChild(-1)
	{
	}

	void Container::pack(Component::Ptr component)
	{
		mChildren.push_back(component);

		if (!hasSelection() && component->isSelectable())
		{
			select(mChildren.size() - 1);
		}
	}

	bool Container::isSelectable() const
	{
		return false;
	}

	void Container::handleEvent(const sf::Event& event)
	{
		if (hasSelection() && mChildren[mSelectedChild]->isActive())
		{
			mChildren[mSelectedChild]->handleEvent(event);
		}
		else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
		{
			const auto &key = keyReleased->code;
			if (key == sf::Keyboard::Key::W ||
				key == sf::Keyboard::Key::Up)
			{
				selectPrevious();
			}
			else if (key == sf::Keyboard::Key::S || 
				key == sf::Keyboard::Key::Down)
			{
				selectNext();
			}
			else if (key == sf::Keyboard::Key::Enter || 
				key == sf::Keyboard::Key::Space)
			{
				if (hasSelection())
				{
					mChildren[mSelectedChild]->activate();
				}
			}
		}
	}


	void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();

		for (const Component::Ptr &child : mChildren)
		{
			target.draw(*child, states);
		}
	}

	bool Container::hasSelection() const
	{
		return mSelectedChild >= 0;
	}

	void Container::select(size_t index)
	{
		if (mChildren[index]->isSelectable())
		{
			if (hasSelection())
			{
				mChildren[mSelectedChild]->deselect();
			}

			mChildren[index]->select();
			mSelectedChild = index;
		}
	}

	void Container::selectNext()
	{
		if (!hasSelection())
		{
			return;
		}

		int next = mSelectedChild;
		do
		{
			next = (next + 1) % mChildren.size();
		} while (!mChildren[next]->isSelectable());

		select(next);
	}

	void Container::selectPrevious()
	{
		if (!hasSelection())
		{
			return;
		}

		int prev = mSelectedChild;
		do
		{
			prev = (mChildren.size() + prev - 1) % mChildren.size();
		} while (!mChildren[prev]->isSelectable());

		select(prev);
	}
}