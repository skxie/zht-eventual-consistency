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
 * RateLimiter.h
 *
 *  Created on: May 12, 2013
 *      Contributor: Mark Kampe, Xiaobing Zhou
 */

#ifndef RATELIMITER_H_
#define RATELIMITER_H_

/*
 A class implementing RateLimiter will be used to ensure that
 no more than <requests> calls to throttleRequest are allowed
 in any <per_time> second window by blocking in throttleRequest
 as needed.

 Give an implementation of this interface. Use of STL and Boost is
 *highly* encouraged over implementing common data structures
 yourself.

 Your implementation will only ever be used from a single thread.
 * */

namespace interview {
namespace inktank {
namespace q1 {

/*
 *
 */
class RateLimiter {

public:
	/**
	 * throttleRequest: blocks until there have been fewer than
	 * <requests> calls to throttleRequest over the last <per_time>
	 * seconds.
	 */
	virtual void throttleRequest() = 0;

protected:
	/**
	 * setLimit: sets limit for RateLimiter
	 *
	 * Will be called only between object creation and the first call to
	 * throttleRequest.
	 *
	 * @param requests [in]: Allow no more than <requests> requests per
	 <per_time>
	 * @param per_time [in]: time over which no more than requests must
	 *                       be allowed (seconds).
	 */
	virtual void setLimit(unsigned requests, unsigned per_time) = 0;
};

} /* namespace q1 */
} /* namespace inktank */
} /* namespace interview */
#endif /* RATELIMITER_H_ */
