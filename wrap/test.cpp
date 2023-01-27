#include "cpp_wrap.h"
#include <sys/epoll.h>
#include <string.h>
using namespace wsq;

int main()
{

	cppWrap wraps;

	std::cout << "等待客户端连接...." << std::endl;

	int lfd = wraps.tcpBind_(NULL, 8888);

	wraps.Listen_(lfd);

	int epfd = epoll_create(1);

	struct epoll_event ev, evs[1024];
	ev.events = EPOLLIN;
	ev.data.fd = lfd;


	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

	while (1)
	{
		int n = epoll_wait(epfd, evs, 1024, -1);
		std::cout << "epoll_wait...\n";

		if (n < 0)
		{
			perror("epoll_wait");
			break;
		}
		else if (n == 0)
		{
			continue;
		}
		else
		{
			for (int i = 0; i < n; i++)
			{
				if (evs[i].data.fd == lfd && evs[i].events & EPOLLIN)
				{
					int cfd = wraps.Accept_();

					wraps.setFdNoBlock_(cfd);
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = cfd;

					epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
				}
				else if (evs[i].events & EPOLLIN)
				{
					while (1)
					{
						char buf[4];
						memset(buf, 0, sizeof(buf));
						ssize_t n = wraps.Read_(evs[i].data.fd, buf, sizeof(buf));
						if (n < 0)
						{
							if (errno == EAGAIN)
							{
								break;
							}

							perror("read");
							epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, &evs[i]);
							epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, &ev);

							wraps.closeFd(evs[i].data.fd);
							break;
						}
						else if (n == 0)
						{
							std::cout << "client close...\n";
							epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, &evs[i]);
							epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, &ev);
							wraps.closeFd(evs[i].data.fd);
							break;
						}
						else
						{
							std::cout << "【READ】: " << buf;
							wraps.Write_(evs[i].data.fd, buf, strlen(buf));
						}
					}

				}
			}
		}

	}
#if 0
	int main() {
		int listenfd = socket(AF_INET, SOCK_STREAM, 0);

		setnonblocking(listenfd);

		struct sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(SERV_PORT);
		bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

		listen(listenfd, 128);

		Epoll epoll;
		epoll.eventset(listenfd, [](int fd, int events) {
			struct sockaddr_in clientaddr;
			socklen_t clilen = sizeof(clientaddr);

			int connfd = accept(fd, (struct sockaddr*)&clientaddr, &clilen);
			if (connfd < 0) {
				std::cerr << "Accept Failed" << std::endl;
				return;
			}
			setnonblocking(connfd);
			std::cout << "Accept a client: " << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << std::endl;
			epoll.eventadd(g_efd_, EPOLLIN, connfd);
			});
		epoll.eventadd(g_efd_, EPOLLIN, listenfd);

		struct epoll_event events[MAX_EVENTS];
		while (true) {
			int nfd = epoll_wait(g_efd_, events, MAX_EVENTS, -1);
			if (nfd < 0) {
				std::cerr << "Epoll Wait Failed" << std::endl;
				break;
			}

			for (int i = 0; i < nfd; i++) {
				myevent_s* ev = (myevent_s*)events[i].data.ptr;
				if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
					ev->call_back(ev->fd, events[i].events);
				}
				if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
					ev->call_back(ev->fd, events[i].events);
				}
			}
		}

		return 0;
}
#endif // 0




	return 0;
}
