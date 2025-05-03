#ifndef RESOURCEHOLDER_H
#define RESOURCEHOLDER_H

#include <memory>
#include <map>
#include <cassert>
#include <string>
#include <stdexcept>


template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
	void load(Identifier id, const std::string& filename);

	template <typename Parameter>
	void load(Identifier id, const std::string& filename, const Parameter& secondParam);

	void open(Identifier id, const std::string& filename);
	
	Resource& get(Identifier id);

	const Resource& get(Identifier id) const;

private:
	std::map<Identifier, std::unique_ptr<Resource>> mResourceMap;

	void insertResource(Identifier id, std::unique_ptr<Resource> resource);
};

#include "ResourceHolder.inl"
#endif 