#include "ChangeList.hpp"

ChangeList::ChangeList(void)
	:	_keventVector(std::vector<struct kevent>())
{}

ChangeList::~ChangeList(void) {}

static void deleteEvent(std::vector<struct kevent>& kevents, std::vector<struct udata>& udatas, uintptr_t fd)
{
	for (std::vector<struct kevent>::iterator it = kevents.begin(); it != kevents.end(); it++)
	{
		if (it->ident == fd)
		{
			kevents.erase(it);
			break ;
		}
	}
	for (std::vector<struct udata>::iterator it = udatas.begin(); it != udatas.end(); it++)
	{
		if (it->fd == fd)
		{
			udatas.erase(it);
			return ;
		}
	}
	std::cout << Colors::Red << "socket " << fd << "is disconnected" << Colors::Reset << std::endl;
}

void ChangeList::changeEvent(uintptr_t ident, int filter, int flags)
{
	// fflags로 CGI 처리와 signal 등 처리 해야함.
	struct kevent	target;
	struct udata	udataTemp;

	std::cout << "udata size: " << _udataVector.size() << std::endl;
	if (flags == EV_DELETE)
		return (deleteEvent(_keventVector, _udataVector, ident));
	udataTemp.fd = ident;
	_udataVector.push_back(udataTemp);
	target.ident = ident;
	target.filter = filter;
	target.flags = flags;
	target.fflags = 0;
	target.data = 0;
	target.udata = &_udataVector[_udataVector.size() - 1];
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
