#ifndef __CPPWRAP_H__
#define __CPPWRAP_H__

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdio>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <cstdlib>
#include <fcntl.h>
/* ************************************************************************
 *       Filename:  cpp_wrap.h
 *    Description:	便于网络编程的包裹函数类
 *        Version:  1.0
 *        Created:  2023年01月17日 22时31分24秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  wsq
 *        Company:
 * ************************************************************************/
namespace wsq {

	class cppWrap
	{
	public:
		cppWrap();
		cppWrap(short unsigned int domain, short unsigned int type);
		~cppWrap();
		int tcpBind_(const char* ip, uint16_t port);
		void setDomain_(short unsigned int domain);
		void setType_(short unsigned int type);
		int setFdBlock_(int fd);
		int setFdNoBlock_(int fd);
		void Listen_(int fd, int backlog = 128);
		int closeFd(int fd);
		int Accept_();
		ssize_t Read_(int fd, void* ptr, size_t nbytes);
		ssize_t Write_(int fd, void* ptr, size_t nbytes);
		ssize_t Readn_(int fd, void* ptr, size_t nbytes);
		ssize_t Writen_(int fd, const void* ptr, size_t nbytes);


	private:
		int m_listenfd;
		int m_connfd;
		short unsigned int m_domain = AF_INET;
		short unsigned int m_type = SOCK_STREAM;
		struct sockaddr_in m_addr;
	private:
		int Socket_(int domain, int type, int protocol = 0);
		void Bind_(const struct sockaddr* addr, socklen_t addrlen);
		void SetSocketOpt_();

	};


}

#endif // !__WRAP_H__