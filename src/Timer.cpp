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
 * Timer.cpp
 *
 *  Created on: May 12, 2013
 *      Contributor: Mark Kampe, Xiaobing Zhou
 */

#include "Timer.h"

#include <fcntl.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

using namespace std;

namespace interview {
namespace inktank {
namespace q1 {

class call_back {
public:
	int due_time;
	void *(*routine)(void *);
	void *routine_arg;
};

const int Timer::MAX_EVENTS = 1;

Timer::Timer() {
}

Timer::~Timer() {
}

int Timer::set_fd_non_block(const int &fd) {

	/*memorize the flags*/
	int flags = fcntl(fd, F_GETFL, 0);

	if (flags == -1)
		return 0;

	flags |= O_NONBLOCK;

	return fcntl(fd, F_SETFL, flags) != -1;
}

void* Timer::async_epoll_wait(void * arg) {

	call_back *_call_back = (call_back*) arg;

	/*
	 * init and setup epoll fd and events array
	 * */
	int efd = epoll_create(MAX_EVENTS);

	set_fd_non_block(efd);

	struct epoll_event *epoll_evs = (struct epoll_event*) calloc(MAX_EVENTS,
			sizeof(struct epoll_event));

	/*
	 * init and setup timer fd
	 * */
	int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (tfd < 0) {

		perror("timerfd create error.");
		abort();
	}

	/*
	 * setup next expiration time of the timer
	 * */
	struct itimerspec new_val;
	struct itimerspec old_val;
	memset(&new_val, 0, sizeof(new_val));
	memset(&old_val, 0, sizeof(old_val));

	struct timespec ts;
	ts.tv_sec = _call_back->due_time;
	ts.tv_nsec = 0;

	new_val.it_value = ts;
	new_val.it_interval = ts;

	if (timerfd_settime(tfd, 0, &new_val, &old_val) < 0) {

		perror("timerfd_settime error.");
		abort();
	}

	/*
	 * init an epoll event attached to the timer fd
	 * */
	struct epoll_event ev;
	ev.data.fd = tfd;
	ev.events = EPOLLIN | EPOLLET;

	/*
	 * add the epoll event(to be triggered) attached to the timer fd(being monitored) to the epoll instance
	 * */
	if (epoll_ctl(efd, EPOLL_CTL_ADD, tfd, &ev) < 0) {

		perror("epoll_ctl error error.");
		abort();
	}

	/*
	 * monitor the timer fd, and trigger the event attached whenever the interval elapsed.
	 * */
	while (1) {

		int n = epoll_wait(efd, epoll_evs, MAX_EVENTS, -1);

		int i;
		for (i = 0; i < n; i++) {

			if ((epoll_evs[i].events & EPOLLERR)
					|| (epoll_evs[i].events & EPOLLHUP)
					|| (!(epoll_evs[i].events & EPOLLIN))) {

				fprintf(stderr, "epoll error: %s\n", strerror(errno));
				close(epoll_evs[i].data.fd);
				continue;

			} else if (epoll_evs[i].data.fd == tfd) {

				uint64_t value;
				read(tfd, &value, sizeof(uint64_t));

				/*invoke the <CALLBACK_ROUTINE> with <CALLBACK_ROUTINE_ARG>*/
				if (_call_back->routine != NULL)
					_call_back->routine(_call_back->routine_arg);
			}
		}
	}

	close(tfd);

	close(efd);

	free(epoll_evs);

	free(_call_back);
}

void Timer::start(const int &due_time, CALLBACK_ROUTINE, CALLBACK_ROUTINE_ARG) {

	pthread_t *pthread = (pthread_t*) calloc(1, sizeof(pthread_t));

	call_back *pcb = (call_back*) calloc(1, sizeof(call_back));

	pcb->due_time = due_time;
	pcb->routine = __call_back_routine;
	pcb->routine_arg = __call_back_routine_arg;

	pthread_create(pthread, NULL, async_epoll_wait, (void*) pcb);

	_threads.push_back(pthread);

}

/*
 a thread can terminate in three ways :
 If the thread returns from its start routine.
 If it is canceled by some other thread. The function used here is pthread_cancel().
 If its calls pthread_exit() function from within itself.
 * */
void Timer::stop() {

	deque<pthread_t*>::iterator it;
	for (it = _threads.begin(); it != _threads.end(); it++) {

		pthread_cancel(*(*it));
		_threads.erase(it);
	}
}

} /* namespace q1 */
} /* namespace inktank */
} /* namespace interview */
