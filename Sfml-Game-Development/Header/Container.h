#ifndef CONTAINER_H
#define CONTAINER_H

#include "Component.h"

#include <vector>


namespace GUI
{
	class Container : public Component
	{
	public:
		typedef std::shared_ptr<Container> Ptr;

		Container();
		void pack(Component::Ptr component);

		virtual bool isSelectable() const;
		virtual void handleEvent(const sf::Event& event);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		bool hasSelection() const;
		void select(size_t index);
		void selectNext();
		void selectPrevious();

		std::vector<Component::Ptr> mChildren;
		int mSelectedChild;
	};
}

#endif

