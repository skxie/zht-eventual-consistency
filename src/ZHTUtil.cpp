/*
 * Copyright 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html)
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname Tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Ke Wang(kwang22@hawk.iit.edu) with nickname KWang,
 *      Dongfang Zhao(dzhao8@@hawk.iit.edu) with nickname DZhao,
 *      Ioan Raicu(iraicu@cs.iit.edu).
 *
 * ZHTUtil.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: Tony
 *      Contributor: Xiaobingo, KWang, DZhao
 */

#include "ZHTUtil.h"

#include "Util.h"
#include "ConfHandler.h"

#include <arpa/inet.h>
#include <algorithm>
#include <netdb.h>
#include <time.h>

#include  "zpack.pb.h"

using namespace iit::datasys::zht::dm;

ZHTUtil::ZHTUtil() {
}

ZHTUtil::~ZHTUtil() {
}

HostEntity ZHTUtil::getHostEntityByKey(const string& msg) {

	int numOfReplica = ConfHandler::ZC_NUM_REPLICAS;
	ZPack zpack;
	zpack.ParseFromString(msg); //to debug
	int replicaNum = 0;

	/*
	printf("{%s}:{%s}:{%s,%s}:{%u, %u}\n", zpack.opcode().c_str(), zpack.key().c_str(), zpack.val().c_str(),
			zpack.newval().c_str()), zpack.replicanum(), zpack.versionnum();
	*/

	uint64_t hascode = HashUtil::genHash(zpack.key());
	size_t node_size = ConfHandler::NeighborVector.size();
	int index = hascode % node_size;

	if(zpack.opcode() == Const::ZSC_OPC_LOOKUP && numOfReplica > 0){
		/*randomly generate the index from all replicas*/
		srand(time(NULL));
		replicaNum = rand() % numOfReplica;
		replicaNum = 1;
		index = (index + replicaNum) % (numOfReplica + 1);
		index = 0;
		int portDiff = ConfHandler::getPortDiffFromConf();
		ConfEntry ce = ConfHandler::NeighborVector.at(index);

		//cout << "The index for lookup is " << index << endl;
		//cout << "The host would be served for the lookup is " << ce.name() << " " << atoi(ce.value().c_str()) + replicaNum * portDiff << endl;

		return buildHostEntity(ce.name(), atoi(ce.value().c_str()) + replicaNum * portDiff);
	}

	ConfEntry ce = ConfHandler::NeighborVector.at(index);

	return buildHostEntity(ce.name(), atoi(ce.value().c_str()));

}

HostEntity ZHTUtil::builtReplicaEntity (const uint& hostIndex, const uint& port){

	ConfEntry ce = ConfHandler::NeighborVector.at(hostIndex);

	return buildHostEntity(ce.name(), atoi(ce.value().c_str()) + port);
}

HostEntity ZHTUtil::buildHostEntity(const string& host, const uint& port) {

	HostEntity he;

	/*
	 struct sockaddr_in si_other;
	 hostent *record;
	 in_addr *address;
	 string ip_address;

	 record = gethostbyname(host.c_str());
	 address = (in_addr *) record->h_addr;
	 ip_address = inet_ntoa(*address);

	 memset((char *) &si_other, 0, sizeof(si_other));
	 si_other.sin_family = AF_INET;
	 si_other.sin_port = htons(port);
	 if (inet_aton(ip_address.c_str(), &si_other.sin_addr) == 0) {
	 fprintf(stderr, "inet_aton() failed\n");
	 }

	 he.si = si_other;
	 he.host = host;
	 he.port = port;
	 he.valid = true;
	 he.sock = -1;*/

	he.host = host;
	he.port = port;
	he.sock = -1;

	return he;
}

const uint IdHelper::ID_LEN = 20;

IdHelper::IdHelper() {
}

IdHelper::~IdHelper() {
}

uint64_t IdHelper::genId() {

	return HashUtil::genHash(HashUtil::randomString(62).c_str());
}

