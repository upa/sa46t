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



#include "inetcon.h"
#include <string.h>
#include <stdio.h>

int
sa46t_convert_4to6 (const struct ip * ip, struct ip6_hdr * ip6,
		    struct in6_addr sa46t_prefix, u_int32_t plane_id)
{
	memset (ip6, 0, sizeof (struct ip6_hdr));

	/* Basic IPv6 Header Preparation */
	ip6->ip6_vfc	= IPV6_VERSION;
	ip6->ip6_plen	= htons (ntohs (ip->ip_len));
	ip6->ip6_nxt	= IPPROTO_IPIP;;
	ip6->ip6_hlim	= ip->ip_ttl;

	/* Source and Destination Address Convert */
	ip6->ip6_src = sa46t_map_4to6_addr (sa46t_prefix, 
					    plane_id, ip->ip_src);

	ip6->ip6_dst = sa46t_map_4to6_addr (sa46t_prefix, 
					    plane_id, ip->ip_dst);

	return 0;
}

int 
sa46t_convert_6to4 (struct ip * ip, const struct ip6_hdr * ip6)
{
	memset (ip, 0, sizeof (struct ip));

	/* Basic IPV4 Header Preparation */
	ip->ip_v	= 4;
	ip->ip_hl	= 5;
	ip->ip_tos 	= 0;
	ip->ip_len	= htons (ntohs (ip6->ip6_plen));
	ip->ip_id 	= 0x0000;
	ip->ip_off	= htons (IP_DF);
	ip->ip_ttl 	= ip6->ip6_hlim;
	ip->ip_p	= IPPROTO_IPIP;
	
	ip->ip_src = sa46t_extract_6to4_addr (ip6->ip6_src);
	ip->ip_dst = sa46t_extract_6to4_addr (ip6->ip6_dst);

	return 0;
}




struct in6_addr
sa46t_map_4to6_addr (struct in6_addr sa46t_prefix, u_int32_t plane_id, struct in_addr ip) 
{
        struct in6_addr sa46t_addr;

        sa46t_addr.s6_addr32[0] = sa46t_prefix.s6_addr32[0];
        sa46t_addr.s6_addr32[1] = sa46t_prefix.s6_addr32[1];
        sa46t_addr.s6_addr32[2] = htonl (plane_id);
        sa46t_addr.s6_addr32[3] = ip.s_addr;

        return sa46t_addr;
}


struct in_addr
sa46t_extract_6to4_addr (struct in6_addr sa46t_addr)
{
        struct in_addr addr;
        addr.s_addr = sa46t_addr.s6_addr32[3];

        return addr;
}

