/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		PF_INET6 protocol dispatch tables.
 *
 * Authors:	Pedro Roque	<roque@di.fc.ul.pt>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

/*
 *      Changes:
 *
 *      Vince Laviano (vince@cs.stanford.edu)       16 May 2001
 *      - Removed unused variable 'inet6_protocol_base'
 *      - Modified inet6_del_protocol() to correctly maintain copy bit.
 */
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/spinlock.h>
#include <net/protocol.h>

const struct inet6_protocol __rcu *inet6_protos[MAX_INET_PROTOS] __read_mostly;

int inet6_add_protocol(const struct inet6_protocol *prot, unsigned char protocol)
{
	int hash = protocol & (MAX_INET_PROTOS - 1);

	return !cmpxchg((const struct inet6_protocol **)&inet6_protos[hash],
			NULL, prot) ? 0 : -1;
}
EXPORT_SYMBOL(inet6_add_protocol);

int inet6_del_protocol(const struct inet6_protocol *prot, unsigned char protocol)
{
	int ret, hash = protocol & (MAX_INET_PROTOS - 1);

	ret = (cmpxchg((const struct inet6_protocol **)&inet6_protos[hash],
		       prot, NULL) == prot) ? 0 : -1;

	synchronize_net();

	return ret;
}
EXPORT_SYMBOL(inet6_del_protocol);
