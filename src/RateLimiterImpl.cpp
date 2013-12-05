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
 * RateLimiterImpl.cpp
 *
 *  Created on: May 12, 2013
 *      Contributor: Mark Kampe, Xiaobing Zhou
 */

#include "RateLimiterImpl.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include "Util.h"

using namespace std;
using namespace iit::datasys::zht::dm;

namespace interview {
namespace inktank {
namespace q1 {

RateLimiterImpl::RateLimiterImpl(const unsigned &requests,
		const unsigned &per_time) {

	setLimit(requests, per_time);

	/*init semaphore <_sem_throttle_open> to <requests> to allow max concurrent requests.*/
	sem_init(&_sem_throttle_open, 0, requests);

	/*start the time, passing in <this> instance of RateLimiterImpl.*/
	_timer.start(_per_time, timer_call_back, this);
}

void* RateLimiterImpl::timer_call_back(void *arg) {

	RateLimiterImpl *pthis = (RateLimiterImpl*) arg;

	double time_to_exit;
	while (!pthis->_time_queue.empty()) {

		time_to_exit = pthis->_time_queue.front();

		/*the non-positive time_to_exit difference means <_sem_throttle_open> must be signaled*/
		if (time_to_exit - TimeUtil::getTime_msec() <= 0) {

			sem_post(&(pthis->_sem_throttle_open));

			pthis->_time_queue.pop();
		}
	}

	/*
	 * adaptively change the interval after which to trigger timer event.
	 * */
	int due_time;
	if (!pthis->_time_queue.empty()) {

		/*generally, this is less than <per_time> */
		due_time = (pthis->_time_queue.front() - TimeUtil::getTime_msec())
				/ 1000;
	} else {

		/*still use <per_time> as interval after which to trigger timer event.*/
		due_time = pthis->_per_time;
	}

	/*stop the timer, and cancel the worker thread(s) behind the timer*/
	pthis->_timer.stop();

	/*re-start timer with new interval*/
	pthis->_timer.start(due_time, timer_call_back, pthis);

	return NULL;
}

void RateLimiterImpl::setLimit(unsigned requests, unsigned per_time) {

	_requests = requests;
	_per_time = per_time;
}

void RateLimiterImpl::throttleRequest() {

	/*wait <_sem_throttle_open> to be signaled*/
	int rc = sem_wait(&_sem_throttle_open);

	/*means requests is less than <requests>, invocation allowed.*/
	if (!rc) {

		double time_to_exit = TimeUtil::getTime_msec() + _per_time * 1000;

		/*enqueue next time stamp after which to signal the <_sem_throttle_open>*/
		_time_queue.push(time_to_exit);
	}
}

RateLimiterImpl::~RateLimiterImpl() {

	sem_destroy(&_sem_throttle_open);
}

} /* namespace q1 */
} /* namespace inktank */
} /* namespace interview */
