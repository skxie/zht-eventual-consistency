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
 * HTWorker.h
 *
 *  Created on: Sep 10, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef HTWORKER_H_
#define HTWORKER_H_

#include "zpack.pb.h"
#include "novoht.h"
#include "proxy_stub.h"
#include "ProxyStubFactory.h"
#include <string>
#include <queue>
#include "TSafeQueue-impl.h"

using namespace std;
using namespace iit::cs550::finalproj;

class HTWorker;

class WorkerThreadArg {

public:
	WorkerThreadArg();
	WorkerThreadArg(const ZPack &zpack, const ProtoAddr &addr,
			const ProtoStub * const stub);
	WorkerThreadArg(const ZPack &zpack, const ProtoAddr &addr,
			const ProtoStub * const stub, ProtoProxy * proxy, const int msg_maxsize);
	virtual ~WorkerThreadArg();

	ZPack _zpack;
	ProtoAddr _addr;
	const ProtoStub *_stub;
	ProtoProxy *_proxy;
	int _msg_maxsize;
};
/*
 *
 */
class HTWorker {
public:
	typedef TSafeQueue<WorkerThreadArg*> QUEUE;

public:
	HTWorker();
	HTWorker(const ProtoAddr& addr, const ProtoStub* const stub);
	virtual ~HTWorker();

public:
	string run(const char *buf);

private:
	string insert(const ZPack &zpack);
	string lookup(ZPack &zpack);
	string append(const ZPack &zpack);
	string remove(const ZPack &zpack);
	string compare_swap(const ZPack &zpack);
	string state_change_callback(const ZPack &zpack);
	string checkexists(const ZPack &zpack);
	string compversion(const ZPack &zpack);

	string insert_shared(const ZPack &zpack);
	string lookup_shared(const ZPack &zpack);
	string append_shared(const ZPack &zpack);
	string remove_shared(const ZPack &zpack);
	string compversion_shared(const ZPack &zpack);

private:
	int extract_versionnum(const string &returnStr);
	string compare_versionnum_with_primary(const string &key, const int versionnum, string &msgFromPrimary);
	string check_exists_with_primary(const string &key, string &msgFromPrimary);
	string UpdateVersion(const string &key, const string &vale);
	string extract_value(const string val);

private:
	static void *threaded_state_change_callback(void *arg);
	static string state_change_callback_internal(const ZPack &zpack);

private:
	string compare_swap_internal(const ZPack &zpack);

private:
	string erase_status_code(string &val);
	string get_novoht_file();
	void init_store();
	bool get_instant_swap();

//private:
//	static void init_sscb_mutex();

private:
	int init_proxy();
	void strong_consistency_primary_replica(ZPack &zpack);
	void strong_consistency(ZPack &zpack);
	void eventual_consistency(ZPack &zpack);
	static void *threaded_eventual_consistnecy(void *arg);

private:
	ProtoAddr _addr;
	const ProtoStub * const _stub;
	bool _instant_swap;

private:
	static bool INIT_PROXY;
	static ProtoProxy *_PROXY;
	static int _MSG_MAXSIZE;

private:
	static NoVoHT *PMAP;
	static QUEUE *PQUEUE;
	static bool FIRST_ASYNC;
//	static bool INIT_SCCB_MUTEX;
//	static pthread_mutex_t SCCB_MUTEX;
};

#endif /* HTWORKER_H_ */
