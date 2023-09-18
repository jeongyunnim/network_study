#include "ChangeList.hpp"

ChangeList::ChangeList(void)
	:	_keventVector(std::vector<struct kevent>())
{}

ChangeList::~ChangeList(void) {}

void ChangeList::changeEvent(uintptr_t ident, int filter, int flags, void *udata)
{
	struct kevent target;

	target.ident = ident;
	target.filter = filter;
	target.flags = flags;
	target.fflags = 0;
	target.data = 0;
	target.udata = udata;
	_keventVector.push_back(target);
}

void ChangeList::clearEvent(void)
{
	_keventVector.clear();
}

std::vector<struct kevent>& ChangeList::getKeventVector(void)
{
	return (_keventVector);
}

size_t ChangeList::getSize(void)
{
	return (_keventVector.size());
}
