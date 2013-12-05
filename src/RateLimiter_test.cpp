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
 * RateLimiter_test.cpp
 *
 *  Created on: May 12, 2013
 *      Contributor: Mark Kampe, Xiaobing Zhou
 */

#include "RateLimiterImpl.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

using namespace interview::inktank::q1;

void test_RateLimiter(const unsigned &total_requests,
		const unsigned &requests_threshold, const unsigned &time_window) {

	RateLimiter *rl = new RateLimiterImpl(requests_threshold, time_window);

	int j = 0;
	unsigned i;
	for (i = 0; i < total_requests; i++) {

		rl->throttleRequest();

		fprintf(stdout, "No.%d request\n", ++j);
	}

	delete rl;
}

void printUsage(char *argv_0);

int main(int argc, char **argv) {

	extern char *optarg;

	int printHelp = 0;

	unsigned total_requests = 0;
	unsigned requests_threshold = 0;
	unsigned time_window = 0;

	int c;
	while ((c = getopt(argc, argv, "i:r:w:h")) != -1) {
		switch (c) {
		case 'i':
			total_requests = atoi(optarg);
			break;
		case 'r':
			requests_threshold = atoi(optarg);
			break;
		case 'w':
			time_window = atoi(optarg);
			break;
		case 'h':
			printHelp = 1;
			break;
		default:
			fprintf(stdout, "Illegal argument \"%c\"\n", c);
			printUsage(argv[0]);
			exit(1);
		}
	}

	if (printHelp) {
		printUsage(argv[0]);
		exit(1);
	}

	if (requests_threshold > 0 && time_window > 0) {

		test_RateLimiter(total_requests, requests_threshold, time_window);
	} else {

		printUsage(argv[0]);
		exit(1);
	}
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"{-i totoal_requests -r requests_threshold -w time_window(seconds)} | {-h(help)}");
}

