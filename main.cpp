#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

int main(int argc, char** argv) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	const unsigned char buff_handshake_p1[] = {
		0x00, 0x00, 0x00, 0x00
	};
	const unsigned char buff_handshake_p2[] = {
		0x00, 0x00, 0x00, 0x2f, 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xff, 0x8b, 0x2e, 0x4b, 0x2d, 0x2a, 0xce,
		0xcc, 0xcf, 0x8b, 0xe5, 0xe2, 0x84, 0xb2, 0x6c, 0x95, 0x0c,
		0xf5, 0x0c, 0x4d, 0xf4, 0x2c, 0x95, 0xb8, 0xa2, 0xf5, 0xe1,
		0x92, 0x5c, 0x00, 0xc0, 0x38, 0xd3, 0xd7, 0x28, 0x00, 0x00,
		0x00
	};
	const unsigned char buff_send_name[] = {
		0x00, 0x00, 0x00, 0x3a, 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xff, 0x8b, 0xce, 0xc9, 0x4f, 0xcf, 0xcc,
		0x8b, 0xe5, 0xe2, 0x2c, 0x2d, 0x4e, 0x2d, 0xca, 0x4b, 0xcc,
		0x4d, 0xb5, 0x55, 0x72, 0x73, 0x73, 0x73, 0x74, 0x74, 0xf4,
		0xf6, 0xf6, 0x76, 0x75, 0x75, 0x55, 0xe2, 0x8a, 0xd6, 0x87,
		0xaa, 0xe0, 0x02, 0x00, 0xa1, 0xfc, 0x19, 0x4c, 0x2b, 0x00,
		0x00, 0x00
	};

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("127.0.0.1", "15000", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	iResult = send(ConnectSocket, (const char*)buff_handshake_p1, (int)sizeof(buff_handshake_p1), 0);
	printf("Bytes Sent: %ld\n", iResult);

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	printf("Bytes received: %d\n", iResult);

	iResult = send(ConnectSocket, (const char*)buff_handshake_p2, (int)sizeof(buff_handshake_p2), 0);
	printf("Bytes Sent: %ld\n", iResult);

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	printf("Bytes received: %d\n", iResult);

	iResult = send(ConnectSocket, (const char*)buff_send_name, (int)sizeof(buff_send_name), 0);
	printf("Bytes Sent: %ld\n", iResult);

	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
