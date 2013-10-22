/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2013  Intel Corporation. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stddef.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>

#include <glib.h>

#include "hal-msg.h"
#include "ipc.h"
#include "log.h"

void ipc_send(GIOChannel *io, uint8_t service_id, uint8_t opcode, uint16_t len,
							void *param, int fd)
{
	struct msghdr msg;
	struct iovec iv[2];
	struct hal_msg_hdr hal_msg;
	char cmsgbuf[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;

	memset(&msg, 0, sizeof(msg));
	memset(&hal_msg, 0, sizeof(hal_msg));

	hal_msg.service_id = service_id;
	hal_msg.opcode = opcode;
	hal_msg.len = len;

	iv[0].iov_base = &hal_msg;
	iv[0].iov_len = sizeof(hal_msg);

	iv[1].iov_base = param;
	iv[1].iov_len = len;

	msg.msg_iov = iv;
	msg.msg_iovlen = 2;

	if (fd >= 0) {
		cmsg = CMSG_FIRSTHDR(&msg);
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));

		/* Initialize the payload */
		memcpy(CMSG_DATA(cmsg), &fd, sizeof(int));

		msg.msg_control = cmsgbuf;
		msg.msg_controllen = sizeof(cmsgbuf);
	}

	if (sendmsg(g_io_channel_unix_get_fd(io), &msg, 0) < 0) {
		error("IPC send failed, terminating :%s", strerror(errno));
		raise(SIGTERM);
	}
}

void ipc_send_error(GIOChannel *io, uint8_t service_id, uint8_t status)
{
	struct hal_msg_rsp_error err;

	err.status = status;

	ipc_send(io, service_id, HAL_MSG_OP_ERROR, sizeof(err), &err, -1);
}