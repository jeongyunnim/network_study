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

void ChangeList::deleteEvent(uintptr_t socketFd)
{
	for (std::vector<struct kevent>::iterator it = _keventVector.begin(); it != _keventVector.end(); it++)
	{
		if (it->ident == socketFd)
		{
			std::cout << Colors::Magenta << "del: Client is disconnected" << socketFd << Colors::Reset << std::endl;
			_keventVector.erase(it);
			return ;
		}
	}
	std::cout << Colors::Red << "del: Client " << socketFd << "Not found error" << Colors::Reset << std::endl;
}

std::vector<struct kevent>& ChangeList::getKeventVector(void)
{
	return (_keventVector);
}

size_t ChangeList::getSize(void)
{
	return (_keventVector.size());
}
