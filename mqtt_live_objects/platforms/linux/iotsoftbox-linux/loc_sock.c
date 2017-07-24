/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the
 * 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package
 * distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 */

/**
 * @file  loc_sock.cpp
 * @brief TCP Socket Interface used by loc_wget
 * @note  Check only one socket
 */

#include "iotsoftbox-core/loc_sock.h"

#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "liveobjects-sys/LiveObjectsClient_Platform.h"
#include "liveobjects-sys/loc_trace.h"
#include "liveobjects-sys/socket_defs.h"

/*---------------------------------------------------------------------------------*/

void LO_sock_disconnect(socketHandle_t *pHdl) {
	if (pHdl) {
		if (*pHdl != SOCKETHANDLE_NULL)
			close(*pHdl);
		*pHdl = SOCKETHANDLE_NULL;
	}
}

int LO_sock_dnsSetFQDN(const char* domain_name, const char* ip_address) {
	return -1;
}

/*---------------------------------------------------------------------------------*/

int LO_sock_connect(short retry, const char *remoteHostAddress,
		uint16_t remoteHostPort, socketHandle_t *pHdl) {
	int sock_fd;
	struct sockaddr_in t_addr_in;
	const char* remote_host;

	if (pHdl) {
		*pHdl = -1;
	}
	LOTRACE_INF("Connecting to server %s:%d (retry=%d) ...", remoteHostAddress,
			remoteHostPort, retry);

	if ((*remoteHostAddress >= '0') && (*remoteHostAddress <= '9')) {
		remote_host = remoteHostAddress;
	} else {
		struct addrinfo hints, *res;
		int errcode;
		char addrstr[100];
		void *ptr;

		memset (&hints, 0, sizeof (hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_CANONNAME;

		errcode = getaddrinfo (remoteHostAddress, NULL, &hints, &res);
		if (errcode == 0) {
			inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

			switch (res->ai_family)
			{
			case AF_INET:
				ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
				break;
			case AF_INET6:
				ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
				break;
			}
			inet_ntop (res->ai_family, ptr, addrstr, 100);
			remote_host = addrstr;
			LOTRACE_INF("   DNS Ok !! => IP address used = %s  ...", remote_host);
		} else {
			LOTRACE_ERR("   DNS failed -> Check the server name. Address used : %s", remoteHostAddress);
			return -1;
		}
	}

	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		LOTRACE_ERR("Could not create socket value : %d", sock_fd);
		return sock_fd;
	}

	memset(&t_addr_in, '0', sizeof(t_addr_in));

	t_addr_in.sin_family = PF_INET;
	t_addr_in.sin_port = htons(remoteHostPort);

	/* convert IPv4 addresses from text to binary form*/
	if (inet_pton(PF_INET, remote_host, &t_addr_in.sin_addr) <= 0) {
		LOTRACE_INF("\n inet_pton error occured\n");
		return -1;
	}

	int ret = connect(sock_fd, (struct sockaddr *) &t_addr_in,
			sizeof(t_addr_in));
	if (ret < 0) {
		LOTRACE_INF("Socket error and ret value is %d", ret);
		return ret;
	}

	LOTRACE_INF("Connected to server %s:%d", remoteHostAddress, remoteHostPort);
	if (pHdl) {
		*pHdl = sock_fd;
	}
	return 0;
}

/*---------------------------------------------------------------------------------*/

int LO_sock_send(socketHandle_t hdl, const char *buf_ptr) {
	int len;
	const char *pc = buf_ptr;
	if ((hdl < 0) || (pc == NULL)) {
		LOTRACE_ERR("LO_sock_send: ERROR - Invalid parameter hdl=%d buf=%p",
				hdl, pc);
		return -1;
	}
	len = strlen(buf_ptr);

	LOTRACE_DBG1("send_data: len=%d\r\n%s", len, buf_ptr);

	while (len > 0) {
		int ret = (int) send(hdl, pc, len, 0);
		if (ret < 0) {
			if (errno == EINTR) {
				LOTRACE_WARN("send_data: INTERRUPT !!");
				continue;
			}
			if (errno == EPIPE || errno == ECONNRESET) {
				LOTRACE_WARN("send_data: Closed by peer");
				return -1;
			}

			LOTRACE_WARN(
					"send_data: ERROR (errno=%d) while sending data , len=%d/%d",
					errno, strlen(buf_ptr) - len, strlen(buf_ptr));
			return -1;
		}
		if (ret == 0) {
			LOTRACE_WARN(
					"send_data: ret=0 => WOULD_BLOCK or Closed by peer ???");
		}
		pc += ret;
		len -= ret;
	}

	LOTRACE_DBG1("send_data: OK");
	return 0;
}

/*---------------------------------------------------------------------------------*/

int LO_sock_recv(socketHandle_t hdl, char *buf_ptr, int buf_len) {
	int ret;
	if ((hdl < 0) || (buf_ptr == NULL) || (buf_len <= 0)) {
		LOTRACE_ERR(
				"LO_sock_recv: Invalid parameters - hdl=%d buf_ptr=%p buf_len=%d",
				hdl, buf_ptr, buf_len);
		return -1;
	}

	ret = (int) recv(hdl, buf_ptr, buf_len, 0);
	if (ret < 0) {
#if 0
		if (netw_would_block(_netw_socket) != 0) {
			LOTRACE_INF("(_netw_socket=%d len=%x) ret=%d x%x", _netw_socket, len, ret, MBEDTLS_ERR_SSL_WANT_READ);
			return( MBEDTLS_ERR_SSL_WANT_READ );
		}
		if (errno == EINTR) {
			LOTRACE_INF("(ctx=%p _netw_socket=%d len=%x) ret=%d x%x", _netw_socket, len, ret, MBEDTLS_ERR_SSL_WANT_READ);
			return( MBEDTLS_ERR_SSL_WANT_READ );
		}
		if (errno == EPIPE || errno == ECONNRESET) {
			_netw_bSockState |= 0x02;
			LOTRACE_ERR("(_netw_socket=%d len=%x) ret=%d errno=%d x%x", _netw_socket, len, ret, errno, MBEDTLS_ERR_NET_CONN_RESET);
			return( MBEDTLS_ERR_NET_CONN_RESET );
		}
#endif
		LOTRACE_ERR("(LO_sock_recv (len=%d) ret=%d errno=%d", buf_len, ret,
				errno);
		return -1;
	}

	buf_ptr[ret] = 0;

	if (ret == 0) {
		LOTRACE_ERR("LO_sock_recv(len=%d) ->  ret = 0 !!!", buf_len);
		return 0;
	}

	LOTRACE_DBG1("LO_sock_recv(len=%d)", ret);
	return ret;
}

/*---------------------------------------------------------------------------------*/

int LO_sock_read_line(socketHandle_t hdl, char *buf_ptr, int buf_len) {
	int len = 0;

	char cc;

	if ((hdl < 0) || (buf_ptr == NULL) || (buf_len <= 0)) {
		LOTRACE_ERR(
				"LO_sock_recv: Invalid parameters - hdl=%d buf_ptr=%p buf_len=%d",
				hdl, buf_ptr, buf_len);
		return -1;
	}

	cc = 0;
	while (1) {
		short retry = 0;
		int ret = (int) recv(hdl, &cc, 1, 0);
		if (ret < 0) {
			/*if (netw_would_block(hdl) != 0) {
			 LOTRACE_INF(
			 "LO_sock_read_line(len=%d) retry=%d -> ERROR_WOULD_BLOCK (%d)", len,
			 retry, ret);
			 if (++retry < 6) {
			 delay(200);
			 continue;
			 }
			 }*/
			LOTRACE_ERR("LO_sock_read_line(len=%d) -> ERROR %d", len, ret);
			return ret;
		}
		if (ret == 0) {
			LOTRACE_ERR(
					"LO_sock_read_line(len=%d) ->  ret=0 -> Closed by peer  !!!",
					len);
			return -1;
		}
		if (cc == '\n') {
			LOTRACE_DBG1("LO_sock_read_line(len=%d) -> EOL", len);
			break;
		}

		buf_ptr[len++] = cc;
		if (len >= buf_len) {
			LOTRACE_ERR("LO_sock_read_line(len=%d) ->  TOO SHORT  !!!", len);
			return -1;
		}
	}

	if ((cc == '\n') && (len >= 1) && (buf_ptr[len - 1] == '\r')) {
		len--;
		if (len == 0)
			LOTRACE_DBG1("LO_sock_read_line ->  BODY  !!!");
	}
	buf_ptr[len] = 0;

	return len;
}
