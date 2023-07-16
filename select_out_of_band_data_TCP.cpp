#include<iostream>
using namespace std;
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<assert.h>
#include<sys/select.h>
#include<string.h>
#include<unistd.h>

int 
main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "usage:" << basename(argv[0]) << " ip port backlog" << endl;
		return 0;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);
	int backlog = atoi(argv[3]);

	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listenfd != -1);

	sockaddr_in sin;
	inet_pton(AF_INET, ip, &sin.sin_addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	int ret = bind(listenfd, (sockaddr*)&sin, sizeof(sin));
	assert(ret != -1);
	ret = listen(listenfd, backlog);
	assert(ret != -1);

	sockaddr_in cin;
	socklen_t cin_len = sizeof(sockaddr_in);
	int connfd = accept(listenfd, (sockaddr*)&cin, &cin_len);
	assert(connfd != -1);

	char buf[1024];
	fd_set rset;
	fd_set xset;
	FD_ZERO(&rset);
	FD_ZERO(&xset);

	while(1) {
		memset(buf, '\0', sizeof(buf));
		FD_SET(connfd, &rset);
		FD_SET(connfd, &xset);
		ret = select(connfd + 1, &rset, NULL, &xset, NULL);
		if (ret < 0) {
			cout << "selset failure" << endl;
			break;
		}	
		if (FD_ISSET(connfd, &rset)) {
			ret = recv(connfd, buf, sizeof(buf) - 1, 0);
			if (ret < 0) break;
			cout << "get" << ret << "bytes of normal data:" << buf << endl;
		}
		else if (FD_ISSET(connfd, &xset)) {
			ret = recv(connfd, buf, sizeof(buf) - 1, MSG_OOB);
			if (ret < 0) break;
			cout << "get" << ret <<"bytes of oob data:" << buf << endl; 
		}	
	}
	close(connfd);
	close(listenfd);
	return 0;
}
