#pragma once
#include "Server.hpp"
#include <vector>

class ChangeList // 얘는 kqueue manage class가 될 수 있음.
{
public:
	ChangeList(void);
	~ChangeList(void);
	void changeEvent(uintptr_t nIdent, int nFilter, int nFlags, void *pUdata);
	void clearEvent(void);
	std::vector<struct kevent>& getKeventVector(void);
	size_t getSize(void);

private:

	ChangeList& operator=(const ChangeList& rhs);
	ChangeList(const ChangeList& other);

	std::vector<struct kevent> _keventVector;
};