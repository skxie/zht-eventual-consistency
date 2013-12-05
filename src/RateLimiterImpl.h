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
 * RateLimiterImpl.h
 *
 *  Created on: May 12, 2013
 *      Contributor: Mark Kampe, Xiaobing Zhou
 */

#ifndef RATELIMITERIMPL_H_
#define RATELIMITERIMPL_H_

#include "RateLimiter.h"
#include "Timer.h"

#include  <pthread.h>
#include  <semaphore.h>
#include  <queue>

using namespace std;

namespace interview {
namespace inktank {
namespace q1 {

/*
 *
 */
class RateLimiterImpl: public RateLimiter {
public:
	/*
	 * @brief: RateLimiterImpl constructor.
	 * @param requests [in]: no more than <requests> in time window(second) <per_time>.
	 * @param per_time [in]: no more than <requests> in time window(second) <per_time>.
	 * */
	RateLimiterImpl(const unsigned &requests, const unsigned &per_time);

	/*
	 * brief: RateLimiterImpl destructor.
	 * */
	virtual ~RateLimiterImpl();

private:
	/*
	 * @brief:   RateLimiterImpl copy constructor.
	 * @details: lock the copy constructor for use.
	 * @param other [in]: another instance of RateLimiterImpl to be copied in constructor.
	 * */
	explicit RateLimiterImpl(const RateLimiterImpl& other);

	/*
	 * @brief:   RateLimiterImpl assignment operator.
	 * @details: lock the assignment operator for use.
	 * @param other [in]: another instance of RateLimiterImpl to be copied in assignment.
	 * */
	RateLimiterImpl operator=(const RateLimiterImpl &other);

public:
	/*
	 * @brief:   a method used to throttle more requests than <requests> allowed.
	 * @details: blocked until there have been fewer than <requests> calls to this method over the last <per_time>.
	 * */
	void throttleRequest();

private:
	/*
	 * @brief:   sets limit for RateLimiter
	 * @details: will be called only between object creation and the first call to throttleRequest.
	 * @param requests [in]: allow no more than <requests> requests per <per_time>.
	 * @param per_time [in]: time over which no more than requests must be allowed (seconds).
	 * */
	void setLimit(unsigned requests, unsigned per_time);

private:
	/*
	 * @brief:   call back method invoked by timer when <due_time> arrived.
	 * @param arg [in]: customized argument passed into the call back method.
	 * */
	static void* timer_call_back(void *arg);

private:
	unsigned _requests;
	unsigned _per_time;

private:
	/*@field _timer: an instance of Timer.*/
	Timer _timer;

	/*@field _sem_throttle_open: semaphore used to throttle the number of requests per <per_time>.*/
	sem_t _sem_throttle_open;

	/*@field _time_queue: a FIFO queue to store time when the <_sem_throttle_open> must be signaled.*/
	queue<double> _time_queue;
};

} /* namespace q1 */
} /* namespace inktank */
} /* namespace interview */
#endif /* RATELIMITERIMPL_H_ */
