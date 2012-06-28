/*-
 * Copyright(C) 2012 Keio University Faculty of Environment and Information Studies 
 * 
 * Permission to use this material for noncommercial and/or evaluation purpose, 
 * copy this material for your own use, and distribute the copies via publicly 
 * accessible on-line media, without fee, is hereby granted provided that the 
 * above copyright notice and this permission notice appear in all copies.
 * 
 * UNIVERSITY MAKES NO REPRESENTATIONS ABOUT THE ACCURACY OR SUITABILITY OF 
 * THIS MATERIAL FOR ANY PURPOSE. IT IS PROVIDED "AS IS", WITHOUT ANY EXPRESS 
 * OR IMPLIED WARRANTIES.
 */



#ifndef _INETCON_H_
#define _INETCON_H_

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

#define SA46T_PLANEID_LEN	4		/* Plane id is 32bit */
#define IPV6_VERSION 		0x60

int sa46t_convert_4to6 (const struct ip * ip, struct ip6_hdr * ip6, 
			struct in6_addr sa46t_prefix, u_int32_t plane_id);

int sa46t_convert_6to4 (struct ip * ip, const struct ip6_hdr * ip6);

struct in6_addr sa46t_map_4to6_addr (struct in6_addr sa46t_prefix, 
				     u_int32_t plane_id,
				     struct in_addr ip);
struct in_addr  sa46t_extract_6to4_addr (struct in6_addr sa46t_dadr);




#define COMPARE_INET6_PREFIX(a, b)	\
	(a.s6_addr32[0] == b.s6_addr32[0] && a.s6_addr32[1] == b.s6_addr32[1])

#define CHECK_PLANEID(a, p) (ntohl(a.s6_addr32[2]) == p)

#define SA46T_CHECK_PREFIX(a1, a2, pid) \
	(COMPARE_INET6_PREFIX(a1, a2) && CHECK_PLANEID(a1, pid))


#endif /* _INETCON_H_ */
