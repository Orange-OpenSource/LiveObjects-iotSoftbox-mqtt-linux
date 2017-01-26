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
 * @file  netw_wrapper.cpp
 * @brief Network Interface.
 */

#include "iotsoftbox-core/netw_sock.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "iotsoftbox-core/loc_sock.h"
#include "liveobjects-sys/LiveObjectsClient_Platform.h"
#include "liveobjects-sys/MQTTLinux.h"
#include "liveobjects-sys/loc_trace.h"

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

static int _netw_socket;

/*---------------------------------------------------------------------------------*/

int f_netw_sock_init(Network *pNetwork, void *net_iface_handler) {
	if (pNetwork) {
		pNetwork->my_socket = -1;
		pNetwork->mqttread = NULL;
		pNetwork->mqttwrite = NULL;
	}
	_netw_socket = -1;
	return 0;
}

uint8_t f_netw_sock_isOpen(Network *pNetwork) {
	return (_netw_socket >= 0) ? 1 : 0;
}

uint8_t f_netw_sock_isLost(Network *pNetwork) {
	return 0;
}

int f_netw_sock_close(Network *pNetwork) {
	LOTRACE_DBG1("f_netw_sock_close(%d %p)...", _netw_socket, pNetwork);
	if (_netw_socket >= 0) {
		close(_netw_socket);
	}
	_netw_socket = -1;
	if (pNetwork) {
		pNetwork->my_socket = -1;
	}
	return 0;
}

int f_netw_sock_setup(Network *pNetwork) {
	return 0;
}

/*---------------------------------------------------------------------------------*/

int f_netw_sock_connect(Network *pNetwork, const char *RemoteHostAddress,
		uint16_t RemoteHostPort, uint32_t tmo_ms) {
	int ret;
	LOTRACE_DBG1(
			"(RemoteHostAddress=%s RemoteHostPort=%u) (_netw_socket=%d) ...",
			RemoteHostAddress, RemoteHostPort, _netw_socket);
	if (_netw_socket >= 0) {
		close(_netw_socket);
	}
	_netw_socket = -1;
	ret = LO_sock_connect(1, RemoteHostAddress, RemoteHostPort, &_netw_socket);

	if (pNetwork) {
		pNetwork->my_socket = _netw_socket;
	}
	return ret;
}

/*---------------------------------------------------------------------------------*/

int f_netw_sock_recv(void *pNetwork, unsigned char *buf, size_t len) {
	int ret;

	if (_netw_socket < 0)
		return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

	/* LOTRACE_DBG1("(pNetwork=%p _netw_socket=%d buf=%p len=%d) ...", pNetwork,*/
	/* _netw_socket, buf, len);*/
	ret = (int) recv(_netw_socket, buf, len, 0);
	if (ret < 0) {
		if (errno == EINTR) {
			LOTRACE_INF("(pNetwork=%p _netw_socket=%d len=%x) ret=%d x%x",
					pNetwork, _netw_socket, len, ret,
					MBEDTLS_ERR_SSL_WANT_READ);
			return (MBEDTLS_ERR_SSL_WANT_READ);
		}

		if (errno == EPIPE || errno == ECONNRESET) {
			LOTRACE_ERR(
					"(pNetwork=%p _netw_socket=%d len=%x) ret=%d errno=%d x%x",
					pNetwork, _netw_socket, len, ret, errno,
					MBEDTLS_ERR_NET_CONN_RESET);
			return (MBEDTLS_ERR_NET_CONN_RESET);
		}
		LOTRACE_ERR("(pNetwork=%p _netw_socket=%d len=%x) ret=%d errno=%d x%x",
				pNetwork, _netw_socket, len, ret, errno,
				MBEDTLS_ERR_NET_RECV_FAILED);
		return (MBEDTLS_ERR_NET_RECV_FAILED);
	}
	LOTRACE_DBG_VERBOSE("(pNetwork=%p _netw_socket=%d len=%d) ret=%d", pNetwork,
			_netw_socket, len, ret);
	return (ret);
}

/*---------------------------------------------------------------------------------*/

int f_netw_sock_recv_timeout(void *pNetwork, unsigned char *buf, size_t len,
		uint32_t timeout) {
	int ret;
	struct timeval tv;
	fd_set read_fds;

	LOTRACE_DBG_VERBOSE("(pNetwork=%p _netw_socket=%d buf=%p len=%d tmo=%u)...",
			pNetwork, _netw_socket, buf, len, timeout);

	if (_netw_socket < 0) {
		LOTRACE_ERR("Invalid context %d", _netw_socket);
		return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
	}

	FD_ZERO(&read_fds);
	FD_SET(_netw_socket, &read_fds);

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	struct timeval tv_const = tv;

	ret = select(_netw_socket + 1, &read_fds, NULL, NULL,
			timeout == ((uint32_t) -1) ? NULL : &tv_const);
	/* Zero fds ready means we timed out */
	if (ret == 0) {
		LOTRACE_DBG_VERBOSE("TIMEOUT (sock=%d len=%d tmo=%u) => x%x!",
				_netw_socket, len, timeout, MBEDTLS_ERR_SSL_TIMEOUT);
		return (MBEDTLS_ERR_SSL_TIMEOUT);
	}

	if (ret < 0) {
		if (errno == EINTR) {
			LOTRACE_WARN("SELECT INTERRUPT (sock=%d tmo=%u) %d !", _netw_socket,
					timeout, ret);
			return (MBEDTLS_ERR_SSL_WANT_READ);
		}
		LOTRACE_WARN("SELECT ERR (sock=%d tmo=%u) %d !", _netw_socket, timeout,
				ret);
		return (MBEDTLS_ERR_NET_RECV_FAILED);
	}

	/* This call will not block */
	return (f_netw_sock_recv(pNetwork, buf, len));
}

/*---------------------------------------------------------------------------------*/

int f_netw_sock_send(void *pNetwork, const unsigned char *buf, size_t len) {
	int ret;

	LOTRACE_DBG_VERBOSE("(pNetwork=%p _netw_socket=%d buf=%p len=%d)...",
			pNetwork, _netw_socket, buf, len);

	if (_netw_socket < 0) {
		LOTRACE_ERR("Invalid context %d", _netw_socket);
		return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
	}

	ret = (int) send(_netw_socket, buf, len, 0);
	if (ret < 0) {
		if (errno == EINTR) {
			return (MBEDTLS_ERR_SSL_WANT_WRITE);
		}

		LOTRACE_ERR("ERROR %d (errno=%d) returnd by send(len=%d)", ret, errno,
				len);

		if (errno == EPIPE || errno == ECONNRESET) {
			return (MBEDTLS_ERR_NET_CONN_RESET);
		}
		return (MBEDTLS_ERR_NET_SEND_FAILED);
	}

	LOTRACE_DBG_VERBOSE("(_netw_socket=%d len=%d) ret= %d", _netw_socket, len,
			ret);
	return (ret);
}
