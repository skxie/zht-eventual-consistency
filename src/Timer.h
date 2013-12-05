/*
 * Copyright (C) 2010-2020 inktank(www.inktank.com) and datasysLab at cs.iit.edu(datasys.cs.iit.edu).
 *      Director: Mark Kampe(mark.kampe@inktank.com)
 *
 * This file is part of my interview with inktank, contributed by
 *      Mark Kampe(mark.kampe@inktank.com),
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 *
 * This file is part of free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, write to the
 * Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 * Stuart Building, Room 003B, Chicago, IL 60616 USA. 
 * 
 * Timer.h
 *
 *  Created on: May 12, 2013
 *      Contributor: Mark Kampe, Xiaobing Zhou
 */

#ifndef TIMER_H_
#define TIMER_H_

#define CALLBACK_ROUTINE void *(*__call_back_routine) (void *)
#define CALLBACK_ROUTINE_ARG  void *__call_back_routine_arg

#include <sys/epoll.h>
#include <stddef.h>
#include <pthread.h>
#include <deque>

using namespace std;

namespace interview {
namespace inktank {
namespace q1 {

/*
 * <Timer> class,
 * built on top of linux epoll that monitors the read/write events on the timer fd(file descriptor).
 * When the timeout event occurs, data is written to timer fd, at this point,
 * epoll event attahced to timer fd is triggered
 * */
class Timer {
public:
	/*
	 * @brief: Timer constructor
	 * */
	Timer();

	/*
	 * @brief: Timer destructor
	 * */
	virtual ~Timer();

private:
	/*
	 * @brief:   Timer copy constructor.
	 * @details: lock the copy constructor for use.
	 * @param other [in]: another instance of Timer to be copied in constructor.
	 * */
	explicit Timer(const Timer &other);

	/*
	 * @brief:   Timer assignment operator.
	 * @details: lock the assignment operator for use.
	 * @param other [in]: another instance of Timer to be copied in assignment.
	 * */
	Timer operator=(const Timer &other);

public:
	/*
	 * @brief:   start the timer
	 * @details: the <CALLBACK_ROUTINE> will be called after every <due_time> seconds.
	 * @param due_time [in]: the interval in terms of seconds, after which timeout event is triggered
	 * @param CALLBACK_ROUTINE [in]: the pointer to a function called by the timer, after every <due_time> seconds.
	 * @param CALLBACK_ROUTINE_ARG [in]: the customized argument passed into <CALLBACK_ROUTINE>
	 * */
	void start(const int &due_time, CALLBACK_ROUTINE, CALLBACK_ROUTINE_ARG =
			NULL);

	/*
	 * @brief: stop the timer, and cancel the worker thread behind(s) the timer
	 * */
	void stop();

private:
	/*
	 * @brief:   a callback method that will be invoked by the worker thread behind the timer.
	 * @details: since <Timer> is built on top of linux epoll, introducing a infinite loop to
	 *           wait timeout event to occur, asynchronous epoll_wait is needed to un-block the thread
	 *           that invokes Timer::start.
	 * @param due_time [in]: the interval in terms of seconds, after which timeout event is triggered
	 * @param arg [in]: the customized argument passed into this method.
	 * */
	static void* async_epoll_wait(void * arg);

	/*
	 * @brief:   set the file descpritor to non-blocking mode.
	 * @param fd [in]: a file descriptor to be set.
	 * */
	static int set_fd_non_block(const int &fd);

private:
	/*@field: a queue of worker thread(s) behind the <Timer>*/
	deque<pthread_t*> _threads;

	/*@field: the max timeout events notified by epoll, somehow, in parallel*/
	static const int MAX_EVENTS;

};

} /* namespace q1 */
} /* namespace inktank */
} /* namespace interview */
#endif /* TIMER_H_ */
