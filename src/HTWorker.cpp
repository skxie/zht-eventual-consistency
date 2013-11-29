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
 * HTWorker.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "HTWorker.h"

#include "Const-impl.h"
#include "Env.h"
#include "lock_guard.h"
#include "ConfHandler.h"
#include "StrTokenizer.h"

#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <stdlib.h>

using namespace std;
using namespace iit::datasys::zht::dm;

WorkerThreadArg::WorkerThreadArg() :
		_stub(NULL), _proxy(NULL), _msg_maxsize(0) {
}

WorkerThreadArg::WorkerThreadArg(const ZPack &zpack, const ProtoAddr &addr,
		const ProtoStub * const stub) :
		_zpack(zpack), _addr(addr), _stub(stub), _proxy(NULL), _msg_maxsize(0) {
}

WorkerThreadArg::WorkerThreadArg(const ZPack &zpack, const ProtoAddr &addr,
		const ProtoStub * const stub, ProtoProxy * proxy, const int msg_maxsize) :
		_zpack(zpack), _addr(addr), _stub(stub), _proxy(proxy), _msg_maxsize(msg_maxsize) {
}

WorkerThreadArg::~WorkerThreadArg() {
}

NoVoHT* HTWorker::PMAP = NULL;

HTWorker::QUEUE* HTWorker::PQUEUE = new QUEUE();

bool HTWorker::INIT_SCCB_MUTEX = false;
pthread_mutex_t HTWorker::SCCB_MUTEX;

HTWorker::HTWorker() :
		_stub(NULL), _instant_swap(get_instant_swap()) {

	init_store();

	init_sscb_mutex();

	init_proxy();
}

HTWorker::HTWorker(const ProtoAddr& addr, const ProtoStub* const stub) :
		_addr(addr), _stub(stub), _instant_swap(get_instant_swap()) {

	init_store();

	init_sscb_mutex();

	init_proxy();
}

HTWorker::~HTWorker() {

	if (_proxy != NULL) {
		delete _proxy;
		_proxy = NULL;
	}
}

string HTWorker::run(const char *buf) {

	string result;

	ZPack zpack;
	string str(buf);
	zpack.ParseFromString(str);

	if (zpack.opcode() == Const::ZSC_OPC_LOOKUP) {

		result = lookup(zpack);

	} else if (zpack.opcode() == Const::ZSC_OPC_INSERT) {

		if (zpack.replicanum() == Const::ZSI_REP_ORIG && ConfHandler::REPLICA_VECTOR_POSITION == 0) {
			zpack.set_opcode(Const::ZSC_OPC_LOOKUP);
			result = lookup_shared(zpack);
			string res_code = result.substr(0, 3);
			result = result.substr(3);
			if (res_code == Const::ZSC_REC_SUCC) {
				zpack.set_versionnum(extract_versionnum(result)+1);
			} else {
				zpack.set_versionnum(0);
			}
			zpack.set_opcode(Const::ZSC_OPC_INSERT);
		}
		result = insert(zpack);

	} else if (zpack.opcode() == Const::ZSC_OPC_APPEND) {

		if(zpack.replicanum() == Const::ZSI_REP_ORIG && ConfHandler::REPLICA_VECTOR_POSITION == 0){ //request received by primary and sent by client
			zpack.set_opcode(Const::ZSC_OPC_LOOKUP);
			result = lookup_shared(zpack);
			string res_code = result.substr(0, 3);
			result = result.substr(3);
			if (res_code == Const::ZSC_REC_SUCC) {
				zpack.set_versionnum(extract_versionnum(result)+1);
			} else {
				zpack.set_versionnum(0);
			}
			zpack.set_opcode(Const::ZSC_OPC_APPEND);
		}
		result = append(zpack);

	} else if (zpack.opcode() == Const::ZSC_OPC_CMPSWP) {

		result = compare_swap(zpack);

	} else if (zpack.opcode() == Const::ZSC_OPC_REMOVE) {

		result = remove(zpack);

	} else if (zpack.opcode() == Const::ZSC_OPC_STCHGCB) {

		result = state_change_callback(zpack);

	} else if (ConfHandler::ZC_NUM_REPLICAS > 0 && zpack.opcode() == Const::ZSC_OPC_CMPVER && zpack.replicanum() == Const::ZSI_REP_REPLICA && ConfHandler::REPLICA_VECTOR_POSITION == 0) {

		//compare versionnum
		result = compversion(zpack);

	} else if (ConfHandler::ZC_NUM_REPLICAS > 0 && zpack.opcode() == Const::ZSC_OPC_EXISTS && zpack.replicanum() == Const::ZSI_REP_REPLICA && ConfHandler::REPLICA_VECTOR_POSITION == 0) {

		//check whether the key-value pair exists in primary
		result = lookup_shared(zpack);

	} else {

		result = Const::ZSC_REC_UOPC;
	}

	return result;
}

int HTWorker::extract_versionnum(const string &returnStr) {

	int vn = -1;
	StrTokenizer strtok(returnStr, ":");

	if (strtok.has_more_tokens()) {

		while (strtok.has_more_tokens()) {

			ZPack zpack;
			zpack.ParseFromString(strtok.next_token());

			if (!zpack.valnull())
				vn = max(vn, zpack.versionnum());
		}

	} else {

		ZPack zpack;
		zpack.ParseFromString(returnStr);

		if (!zpack.valnull())
			vn = max(vn, zpack.versionnum());
	}

	return vn;
}

string HTWorker::check_exists_with_primary(const string &key, string &msgFromPrimary) {

	//generate a zpack with versionnum
	ZPack zpack;
	zpack.set_opcode(Const::ZSC_OPC_EXISTS);
	zpack.set_replicanum(Const::ZSI_REP_REPLICA);

	if (key.empty())
		return Const::ZSC_REC_EMPTYKEY; //-1, empty key not allowed.
	else
		zpack.set_key(key);
	zpack.set_val("^"); //coup, to fix ridiculous bug of protobuf! //to debug
	zpack.set_valnull(true);
	zpack.set_newval("?"); //coup, to fix ridiculous bug of protobuf! //to debug
	zpack.set_newvalnull(true);

	//compare the zpack with versionnum with primary
	ConfHandler::HIT primary = ConfHandler::ReplicaVector.begin();
	string msg = zpack.SerializeAsString();
	char *buf = (char*) calloc(_msg_maxsize, sizeof(char));
	size_t msz = _msg_maxsize;
	/*send to and receive from*/
	_proxy->sendrecv(*primary, msg.c_str(), msg.size(), buf, msz);

	/*...parse status and result*/
	string sstatus;

	string srecv(buf);

	if (srecv.empty()) {
		sstatus = Const::ZSC_REC_SRVEXP;
	} else {
		msgFromPrimary = srecv.substr(3); //the left, if any, is lookup result or second-try zpack
		sstatus = srecv.substr(0, 3); //status returned, the first three chars, like 001, -98...
	}

	free(buf);
	return sstatus;

}


string HTWorker::compare_versionnum_with_primary(const string &key, const int versionnum, string &msgFromPrimary) {

	//generate a zpack with versionnum
	ZPack zpack;
	zpack.set_opcode(Const::ZSC_OPC_CMPVER);
	zpack.set_replicanum(Const::ZSI_REP_REPLICA);

	if (key.empty())
		return Const::ZSC_REC_EMPTYKEY; //-1, empty key not allowed.
	else
		zpack.set_key(key);
	zpack.set_val("^"); //coup, to fix ridiculous bug of protobuf! //to debug
	zpack.set_valnull(true);
	zpack.set_newval("?"); //coup, to fix ridiculous bug of protobuf! //to debug
	zpack.set_newvalnull(true);
	zpack.set_versionnum(versionnum);

	//compare the zpack with versionnum with primary
	ConfHandler::HIT primary = ConfHandler::ReplicaVector.begin();
	string msg = zpack.SerializeAsString();
	char *buf = (char*) calloc(_msg_maxsize, sizeof(char));
	size_t msz = _msg_maxsize;
	/*send to and receive from*/
	_proxy->sendrecv(*primary, msg.c_str(), msg.size(), buf, msz);

	/*...parse status and result*/
	string sstatus;

	string srecv(buf);

	if (srecv.empty()) {
		sstatus = Const::ZSC_REC_SRVEXP;
	} else {
		msgFromPrimary = srecv.substr(3); //the left, if any, is lookup result or second-try zpack
		sstatus = srecv.substr(0, 3); //status returned, the first three chars, like 001, -98...
	}

	free(buf);
	return sstatus;
}

string HTWorker::insert_shared(const ZPack &zpack) {

	string result;

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1

	string key = zpack.key();
	int ret = PMAP->put(key, zpack.SerializeAsString());

	if (ret != 0) {

		printf("thread[%lu] DB Error: fail to insert, rcode = %d\n",
				pthread_self(), ret);
		fflush(stdout);

		result = Const::ZSC_REC_NONEXISTKEY; //-92
	} else {

		if (_instant_swap) {
			PMAP->writeFile();
			//PMAP->flushDbfile();
		}

		result = Const::ZSC_REC_SUCC; //0, succeed.
	}

	return result;
}

string HTWorker::insert(const ZPack &zpack) {

	string result = insert_shared(zpack);

	cout << "The result of insert is " << result << endl;
	cout << "THe item" << zpack.SerializeAsString() << "has been inserted" << endl;

	if (ConfHandler::ZC_NUM_REPLICAS > 0 && result == Const::ZSC_REC_SUCC) {

		ZPack msg = zpack;
		//strong consistency
		strong_consistency(msg);

		//eventual consistency
		eventual_consistency(msg);
	}

#ifdef SCCB
	_stub->sendBack(_addr, result.data(), result.size());
	return "";
#else
	return result;
#endif
}

string HTWorker::compversion(const ZPack &zpack) {

	string result;

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY;

	string lookup_result = lookup_shared(zpack);
	string sstatus = lookup_result.substr(0, 3);
	if (sstatus == Const::ZSC_REC_SUCC)
		lookup_result = lookup_result.substr(3); //the left, if any, is lookup result or second-try zpack
	else
		return lookup_result;

	if (zpack.versionnum() == extract_versionnum(lookup_result)) {
		return Const::ZSC_REC_SUCC;
	}

	result = Const::ZSC_REC_VERSIONCONFLICT;
	result.append(lookup_result);

	return result;
}

string HTWorker::lookup_shared(const ZPack &zpack) {

	string result;

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1

	string key = zpack.key();
	string *ret = PMAP->get(key);

	if (ret == NULL) {

		printf("thread[%lu] DB Error: lookup found nothing\n", pthread_self());
		fflush(stdout);

		result = Const::ZSC_REC_NONEXISTKEY;
		result.append("Empty");
	} else {

		result = Const::ZSC_REC_SUCC;
		result.append(*ret);
	}

	return result;
}

string HTWorker::lookup(ZPack &zpack) {

	string result = lookup_shared(zpack);

	cout << "The number of replica is " << ConfHandler::ZC_NUM_REPLICAS << endl;
	cout << "The result of lookup is " << result << endl;

	if (ConfHandler::ZC_NUM_REPLICAS > 0) {
		if (zpack.replicanum() == Const::ZSI_REP_ORIG && ConfHandler::REPLICA_VECTOR_POSITION != 0) {
			string result_code = result.substr(0, 3);
			string result_zpack = result.substr(3);
			int versionNum = extract_versionnum(result_zpack);
			if (versionNum != -1) {
				//check versionnum with primary
				string msgFromPrimary;
				string status = compare_versionnum_with_primary(zpack.key(), versionNum, msgFromPrimary);
				if (status == Const::ZSC_REC_SUCC)
					return result;
				if (status == Const::ZSC_REC_NONEXISTKEY) {
					zpack.set_opcode(Const::ZSC_OPC_REMOVE_SELF);
					result = remove_shared(zpack);
					result = Const::ZSC_REC_NONEXISTKEY;
					result.append("Empty");
				} else if (status == Const::ZSC_REC_VERSIONCONFLICT) {
					ZPack val;
					val.ParseFromString(msgFromPrimary);
					val.set_opcode(Const::ZSC_OPC_INSERT_SELF);
					result = insert_shared(val);
					if (result == Const::ZSC_REC_SUCC) {
						result.append(msgFromPrimary);
					}
				}
			} else {
				//check the key-value pair with primary

				cout << "Not found in itself, try to update from primary" << endl;

				string msgFromPrimay;
				string status = check_exists_with_primary(zpack.key(), msgFromPrimay);
				if (status == Const::ZSC_REC_SUCC) {
					ZPack val;
					val.ParseFromString(msgFromPrimay);
					val.set_opcode(Const::ZSC_OPC_INSERT_SELF);
					result = insert_shared(val);
					if (result == Const::ZSC_REC_SUCC) {
						result.append(msgFromPrimay);
					}
				} else if (status == Const::ZSC_REC_NONEXISTKEY) {
					result = Const::ZSC_REC_NONEXISTKEY;
					result.append("Empty");
				}
			}
		}
	}

#ifdef SCCB
	_stub->sendBack(_addr, result.data(), result.size());
	return "";
#else
	return result;
#endif
}

string HTWorker::append_shared(const ZPack &zpack) {

	string result;

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1

	string key = zpack.key();
	int ret = PMAP->append(key, zpack.SerializeAsString());

	if (ret != 0) {

		printf("thread[%lu] DB Error: fail to append, rcode = %d\n",
				pthread_self(), ret);
		fflush(stdout);

		result = Const::ZSC_REC_NONEXISTKEY; //-92
	} else {

		if (_instant_swap) {
			PMAP->writeFile();
			//PMAP->flushDbfile();
		}

		result = Const::ZSC_REC_SUCC; //0, succeed.
	}

	return result;
}

string HTWorker::append(const ZPack &zpack) {

	string result = append_shared(zpack);

	if (ConfHandler::ZC_NUM_REPLICAS != 0 && result == Const::ZSC_REC_SUCC) {

		ZPack msg = zpack;
		//strong consistency
		strong_consistency(msg);

		//eventual consistency
		eventual_consistency(msg);
	}

#ifdef SCCB
	_stub->sendBack(_addr, result.data(), result.size());
	return "";
#else
	return result;
#endif
}

void HTWorker::strong_consistency(ZPack &zpack) {

	if (ConfHandler::ZC_NUM_REPLICAS > 0 && zpack.replicanum() == Const::ZSI_REP_ORIG && ConfHandler::REPLICA_VECTOR_POSITION == 0) {

		if (zpack.opcode() == Const::ZSC_OPC_INSERT || zpack.opcode() == Const::ZSC_OPC_REMOVE || zpack.opcode() == Const::ZSC_OPC_APPEND) {
			zpack.set_replicanum(Const::ZSI_REP_PRIM);
			string msg = zpack.SerializeAsString();
			char *buf = (char*) calloc(_msg_maxsize, sizeof(char));
			size_t msz = _msg_maxsize;
			for (ConfHandler::HIT iter = ConfHandler::ReplicaVector.begin() + 1; iter != ConfHandler::ReplicaVector.end(); iter++) {
				/*send to and receive from*/
				_proxy->sendrecv(*iter, msg.c_str(), msg.size(), buf, msz);
			}
		}
	}
}

void HTWorker::eventual_consistency(ZPack &zpack) {

	if (ConfHandler::ZC_NUM_REPLICAS > 0 && ConfHandler::REPLICA_VECTOR_POSITION < ConfHandler::ZC_NUM_REPLICAS) {

		if (zpack.opcode() == Const::ZSC_OPC_INSERT || zpack.opcode() == Const::ZSC_OPC_REMOVE || zpack.opcode() == Const::ZSC_OPC_APPEND) {

			cout << "update copy with replica" << endl;

			lock_guard lock(&SCCB_MUTEX);
			WorkerThreadArg *wta = new WorkerThreadArg(zpack, _addr, _stub, _proxy, _msg_maxsize);
			PQUEUE->push(wta); //queue the WorkerThreadArg to be used in thread function

			pthread_t tid;
			pthread_create(&tid, NULL, threaded_eventual_consistnecy, NULL);
		}
	}
}

void *HTWorker::threaded_eventual_consistnecy(void *arg) {

	lock_guard lock(&SCCB_MUTEX);

	if (!PQUEUE->empty()) { //dequeue the WorkerThreadArg

		WorkerThreadArg* pwta = PQUEUE->front();
		PQUEUE->pop();

		lock.unlock();

		if (ConfHandler::REPLICA_VECTOR_POSITION == 0)
			pwta->_zpack.set_replicanum(Const::ZSI_REP_PRIM);
		else
			pwta->_zpack.set_replicanum(Const::ZSI_REP_REPLICA);

		ConfHandler::HIT receiver = ConfHandler::ReplicaVector.begin() + ConfHandler::REPLICA_VECTOR_POSITION;
		string msg = pwta->_zpack.SerializeAsString();
		char *buf = (char*) calloc(pwta->_msg_maxsize, sizeof(char));
		size_t msz =pwta-> _msg_maxsize;

		cout << "The receiver for eventual is " << receiver->host << " " << receiver->port << endl;
		cout << "The operation code for eventual is " << pwta->_zpack.opcode() << endl;
		cout << "The zpack would be transferred is " << msg << endl;

		pwta->_proxy->sendrecv(*receiver, msg.c_str(), msg.size(), buf, msz);

		delete pwta;

	}

}


string HTWorker::state_change_callback(const ZPack &zpack) {

	lock_guard lock(&SCCB_MUTEX);
	WorkerThreadArg *wta = new WorkerThreadArg(zpack, _addr, _stub);
	PQUEUE->push(wta); //queue the WorkerThreadArg to be used in thread function

	pthread_t tid;
	pthread_create(&tid, NULL, threaded_state_change_callback, NULL);

	return "";
}

void *HTWorker::threaded_state_change_callback(void *arg) {

	lock_guard lock(&SCCB_MUTEX);

	if (!PQUEUE->empty()) { //dequeue the WorkerThreadArg

		WorkerThreadArg* pwta = PQUEUE->front();
		PQUEUE->pop();

		lock.unlock();

		string result = state_change_callback_internal(pwta->_zpack);

		int mslapsed = 0;
		int lease = atoi(pwta->_zpack.lease().c_str());
		int poll_interval = Env::get_sccb_poll_interval();
		//printf("poll_interval: %d\n", poll_interval);

		while (result != Const::ZSC_REC_SUCC) {

			mslapsed += poll_interval;
			usleep(poll_interval * 1000);

			if (mslapsed >= lease)
				break;

			result = state_change_callback_internal(pwta->_zpack);
		}

		pwta->_stub->sendBack(pwta->_addr, result.data(), result.size());

		/*pwta->_htw->_stub->sendBack(pwta->_htw->_addr, result.data(),
		 result.size());*/

		delete pwta;
	}
}

string HTWorker::state_change_callback_internal(const ZPack &zpack) {

	string result;

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1

	string key = zpack.key();
	string *ret = PMAP->get(key);

	if (ret == NULL) {

		printf("thread[%lu] DB Error: lookup found nothing\n", pthread_self());
		fflush(stdout);

		result = Const::ZSC_REC_NONEXISTKEY;
	} else {

		ZPack rltpack;
		rltpack.ParseFromString(*ret);

		if (zpack.val() == rltpack.val()) {

			result = Const::ZSC_REC_SUCC; //0, succeed.
		} else {

			result = Const::ZSC_REC_SCCBPOLLTRY;
		}
	}

	return result;
}

string HTWorker::compare_swap(const ZPack &zpack) {

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1

	string result = compare_swap_internal(zpack);

	string lkpresult = lookup_shared(zpack);

	result.append(erase_status_code(lkpresult));

#ifdef SCCB
	_stub->sendBack(_addr, result.data(), result.size());
	return "";
#else
	return result;
#endif
}

string HTWorker::compare_swap_internal(const ZPack &zpack) {

	string ret;

	/*get Package stored by lookup*/
	string lresult = lookup_shared(zpack);
	ZPack lzpack;
	lresult = erase_status_code(lresult);
	lzpack.ParseFromString(lresult);

	/*get seen_value passed in*/
	string seen_value_passed_in = zpack.val();

	/*get seen_value stored*/
	string seen_value_stored = lzpack.val();

	/*	printf("{%s}:{%s,%s}\n", zpack.key().c_str(), zpack.val().c_str(),
	 zpack.newval().c_str());*/

	/*they are equivalent, compare and swap*/
	if (!seen_value_passed_in.compare(seen_value_stored)) {

		lzpack.set_val(zpack.newval());

		return insert_shared(lzpack);

	} else {

		return Const::ZSC_REC_SRVEXP;
	}
}

string HTWorker::remove_shared(const ZPack &zpack) {

	string result;

	if (zpack.key().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1

	string key = zpack.key();
	int ret = PMAP->remove(key);

	if (ret != 0) {

		printf("thread[%lu] DB Error: fail to remove, rcode = %d\n",
				pthread_self(), ret);
		fflush(stdout);

		result = Const::ZSC_REC_NONEXISTKEY; //-92
	} else {

		if (_instant_swap) {
			PMAP->writeFile();
			//PMAP->flushDbfile();
		}

		result = Const::ZSC_REC_SUCC; //0, succeed.
	}

	return result;
}

string HTWorker::remove(const ZPack &zpack) {

	string result = remove_shared(zpack);

	if (ConfHandler::ZC_NUM_REPLICAS != 0 && result == Const::ZSC_REC_SUCC) {

		ZPack msg = zpack;
		//strong consistency
		strong_consistency(msg);

		//eventual consistency
		eventual_consistency(msg);
	}

#ifdef SCCB
	_stub->sendBack(_addr, result.data(), result.size());
	return "";
#else
	return result;
#endif
}

string HTWorker::erase_status_code(string & val) {

	return val.substr(3);
}

void HTWorker::init_sscb_mutex() {

	if (!INIT_SCCB_MUTEX) {

		pthread_mutex_init(&SCCB_MUTEX, NULL);
		INIT_SCCB_MUTEX = true;
	}
}

string HTWorker::get_novoht_file() {

	return ConfHandler::NOVOHT_FILE;
}

void HTWorker::init_store() {

	if (PMAP == NULL)
		PMAP = new NoVoHT(get_novoht_file(), 100000, 10000, 0.7);
}

int HTWorker::init_proxy() {

	_msg_maxsize = Env::get_msg_maxsize();

	_proxy = ProxyStubFactory::createProxy();

	if (_proxy == 0)
		return -1;
	else
		return 0;
}

bool HTWorker::get_instant_swap() {

	string swap = ConfHandler::get_zhtconf_parameter(Const::INSTANT_SWAP);

	int flag = atoi(swap.c_str());

	bool result;

	if (flag == 1)
		result = true;
	else if (flag == 0)
		result = false;
	else
		result = false;

	return result;
}
