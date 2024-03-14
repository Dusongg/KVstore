#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <chrono>


#define BUFFER_LENGTH 512

int connect_tcpserver(std::string ip, uint16_t port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &(server.sin_addr));

    int ret = connect(sockfd, (struct sockaddr*)&server, sizeof(server));
    if (ret) {
        perror("connect");
        return -1;
    }
    return sockfd;
}


void _testcase(int sockfd, const char* msg, const char* hope) {
    int ret = send(sockfd, msg, strlen(msg), 0);
    if (ret < 0) {
        perror("send");
        return;
    }
    char result[BUFFER_LENGTH]{0};

    recv(sockfd, result, BUFFER_LENGTH, 0);

    if (strcmp(result, hope) == 0) {
		// printf("==> PASS --> %s\n", msg);
	} else {
		printf("==> FAILED --> %s, '%s' != '%s'\n", msg, result, hope);
	}
}


void testcase(int sockfd, int send_count) {
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < send_count; i++) {
        _testcase(sockfd, "SET NAME Dusong", "set success!\n");
        _testcase(sockfd, "GET NAME", "get value: Dusong\n");
        _testcase(sockfd, "MOD NAME Moli", "modify success!\n");
        _testcase(sockfd, "GET NAME", "get value: Moli\n");
        _testcase(sockfd, "DEL NAME", "delete success!\n");
        _testcase(sockfd, "GET NAME", "get failed!\n");
    } 

    auto end_time = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
    std::cout << "Serail task finish, " << seconds << " seconds consumed, qps: "<< 6 * send_count / seconds << std::endl;
}

// ./testcase -s 134.175.52.60 -p 8888 -c 100000
int main(int argc, char *argv[])
{

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);
    int send_count;
    int opt;
	while ((opt = getopt(argc, argv, "s:p:c:?")) != -1) {
		switch (opt) {
			case 's':
				serverip = optarg;
				break;

			case 'p':
				serverport = atoi(optarg);
				break;
            case 'c':
                send_count = atoi(optarg);
                break;
			default:
				return -1;
		}
	}
    int sockfd = connect_tcpserver(serverip, serverport);

    testcase(sockfd, send_count);
    // char* msg = "SET 1 1";
    // int ret = send(sockfd, msg, strlen(msg), 0);
    // char result[BUFFER_LENGTH]{0};
    // recv(sockfd, result, BUFFER_LENGTH, 0);
    // std::cout << result << std::endl;
    return 0;
}