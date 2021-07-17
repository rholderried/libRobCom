/*
 * Helpers.cpp
 *
 *  Created on: 22.08.2019
 *      Author: roman
 */
#include "Helpers.h"
#include <functional>
#include <iostream>

using namespace std;

// Parametrized Constructor
ThreadWrapper::ThreadWrapper(std::function<void()> func) : m_threadHandler(func)
{
	cout << "Thread started!" << endl;
}

// Move Constructor
ThreadWrapper::ThreadWrapper(ThreadWrapper && obj) : m_threadHandler(std::move(obj.m_threadHandler))
{
	std::cout << "Thread move Constructor is called" << std::endl;
}

//Move Assignment Operator
ThreadWrapper & ThreadWrapper::operator=(ThreadWrapper && obj)
{
	std::cout << "Thread move Assignment is called" << std::endl;
	// Join the own thread before it gets terminated
	if (m_threadHandler.joinable())
		m_threadHandler.join();
	// Thread moving results in destruction our own thread (if there is one)
	m_threadHandler = std::move(obj.m_threadHandler);
	return *this;
}


// Destructor : Join the thread object
ThreadWrapper::~ThreadWrapper()
{
	// If the main thread destroys the thread wrapped by the ThreadWrapper
	// object, then it has to wait until this thread has exeeded its lifetime.
	if (m_threadHandler.joinable())
		m_threadHandler.join();
}

HelperFunctions::HelperFunctions()
{
	// Check the Byte Order of the system
	m_byteOrder = CheckByteOrder();
}

// Destructor
HelperFunctions::~HelperFunctions()
{
	cout << "Deleting Helpers" << endl;
}

BYTEORDER HelperFunctions::CheckByteOrder ()
{
    uint16_t word = 0x0001;
    uint8_t * byte = reinterpret_cast<uint8_t*>(&word);
    return (byte[0] ? LITTLE : BIG);
}


Timeout::Timeout(){}
Timeout::~Timeout(){}
// SetTimeout function
void Timeout::SetTimeout(int after, bool async)
{

	// If the async flag is set, then execute the Timer in another thread
	if (async)
	{
		std::thread([after, this]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(after));
			this->m_timerCallback();
		}).detach();
	}
	// Else use this thread (which obviously sleeps now for the amount of time)
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(after));
		m_timerCallback();
	}
}


