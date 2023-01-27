#include "cpp_wrap.h"
/* ************************************************************************
 *       Filename:  cpp_wrap.cpp
 *    Description:	便于网络编程的包裹函数实现
 *        Version:  1.0
 *        Created:  2023年01月17日 22时31分24秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  wsq
 *        Company:
 * ************************************************************************/
wsq::cppWrap::cppWrap()
{


}

wsq::cppWrap::cppWrap(short unsigned int domain, short unsigned int type)
{
	setDomain_(domain);
	setType_(type);
}

int wsq::cppWrap::tcpBind_(const char* ip, uint16_t port)
{

	int lfd = Socket_(m_domain, m_type);
	memset(&m_addr, 0, sizeof(m_addr));
	if (ip == NULL)
	{
		m_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if (inet_pton(this->m_domain, ip, &m_addr.sin_addr.s_addr) <= 0)
			perror("inet_pton");
		exit(1);
	}

	m_addr.sin_family = m_domain;
	m_addr.sin_port = htons(port);

	SetSocketOpt_();

	Bind_((struct sockaddr*)&m_addr, sizeof(m_addr));

	return lfd;
}

void wsq::cppWrap::setDomain_(short unsigned int domain)
{
	this->m_domain = domain;
}

void wsq::cppWrap::setType_(short unsigned int type)
{
	this->m_type = type;
}

int wsq::cppWrap::setFdBlock_(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags < 0)
	{
		return flags;
	}
	flags &= ~O_NONBLOCK;

	int ret = fcntl(fd, F_SETFL, flags);

	return ret;
}

int wsq::cppWrap::setFdNoBlock_(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags < 0)
	{
		return flags;
	}
	flags |= O_NONBLOCK;

	int ret = fcntl(fd, F_SETFL, flags);

	return ret;
}

void wsq::cppWrap::Listen_(int fd, int backlog)
{
	if (listen(fd, backlog) < 0)
		perror("listen");
}

int wsq::cppWrap::closeFd(int fd)
{
	int n;

	if ((n = close(fd)) < 0)
		perror("close");


	return n;
}

int wsq::cppWrap::Accept_()
{
	socklen_t len = sizeof(this->m_addr);
again:
	if ((this->m_connfd = accept(this->m_listenfd, (struct sockaddr*)&this->m_addr, &len)) < 0)
	{
		if ((errno == ECONNABORTED) || (errno == EINTR))//如果是被信号中断和软件层次中断,不能退出
			goto again;
		else
			perror("accept");
	}
	char ip[16] = "";
	printf("new client connect ip=%s,port=%d\n",
		inet_ntop(AF_INET, &m_addr.sin_addr.s_addr, ip, 16), ntohs(m_addr.sin_port));

	return m_connfd;
}

ssize_t wsq::cppWrap::Read_(int fd, void* ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ((n = read(fd, ptr, nbytes)) < 0)
	{
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}

	return n;
}

ssize_t wsq::cppWrap::Write_(int fd, void* ptr, size_t nbytes)
{
	ssize_t n;
again:
	if ((n = write(fd, ptr, nbytes)) < 0)
	{
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}

	return n;
}

ssize_t wsq::cppWrap::Readn_(int fd, void* ptr, size_t nbytes)
{
	char* vptr = (char*)ptr;
	ssize_t nRead;
	size_t nleft = nbytes;

	while (nleft > 0)
	{
		if ((nRead = read(fd, vptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nRead = 0;
			}
			else
			{
				return -1;
			}
		}
		else if (nRead == 0)
		{
			break;
		}

		nleft -= nRead;
		vptr += nRead;

	}


	return nbytes - nleft;
}

ssize_t wsq::cppWrap::Writen_(int fd, const void* ptr, size_t nbytes)
{
	char* vptr = (char*)ptr;
	ssize_t nWrite;
	size_t nleft = nbytes;

	while (nleft > 0)
	{
		if ((nWrite = write(fd, vptr, nleft)) < 0)
		{
			if (nWrite < 0 && errno == EINTR)
			{
				nWrite = 0;
			}
			else
			{
				return -1;
			}
		}
		else if (nWrite == 0)
		{
			continue;
		}

		nleft -= nWrite;
		vptr += nWrite;

	}

	return nbytes - nleft;
}

int wsq::cppWrap::Socket_(int domain, int type, int protocol)
{

	if ((this->m_listenfd = socket(domain, type, protocol)) < 0)
		perror("socket");

	return m_listenfd;
}

void wsq::cppWrap::Bind_(const struct sockaddr* addr, socklen_t addrlen)
{

	if (bind(this->m_listenfd, addr, addrlen) < 0)
		perror("bind");

	return;
}

void wsq::cppWrap::SetSocketOpt_()
{
	int opt = 1;
	setsockopt(this->m_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

wsq::cppWrap::~cppWrap()
{

}