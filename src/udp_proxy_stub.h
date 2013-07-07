/*
 * udp_proxy_stub.h
 *
 *  Created on: Jun 21, 2013
 *      Author: Xiaobing Zhou
 */

#ifndef UDP_PROXY_STUB_H_
#define UDP_PROXY_STUB_H_

#include "ip_proxy_stub.h"

/*
 *
 */
class UDPProxy: public IPProtoProxy {
public:
	UDPProxy();
	virtual ~UDPProxy();

	virtual bool sendrecv(const void *sendbuf, const size_t sendcount,
			void *recvbuf, size_t &recvcount);
	virtual bool teardown();

protected:
	virtual int getSockCached(const string& host, const uint& port);
	virtual int makeClientSocket(const string& host, const uint& port);
	virtual int recvFrom(int sock, void* recvbuf, int recvbufsize);

private:
	int sendTo(int sock, const string &host, uint port, const void* sendbuf,
			int sendcount);

private:
	static int UDP_SOCKET;
};

class UDPStub: public IPProtoStub {
public:
	UDPStub();
	virtual ~UDPStub();

	virtual bool recvsend(ProtoAddr addr, const void *recvbuf);

protected:
	virtual int sendBack(ProtoAddr addr, const void* sendbuf, int sendcount);

};

#endif /* UDP_PROXY_STUB_H_ */