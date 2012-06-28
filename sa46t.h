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



#ifndef _SA46T_H_
#define _SA46T_H_

#include "iftun.h"
#include "inetcon.h"
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>

void usage (void);


#define BUF_LEN 2056
#define TUNIF_NAME "sa46t"

#define IS_NETWORK_PREFIX(a) (a.s6_addr32[2] == 0 && a.s6_addr32[3] == 0)

#define COMPARE_INET6_ADDR(a, b) \
	((a.s6_addr32[0] == b.s6_addr32[0]) &&	\
	 (a.s6_addr32[1] == b.s6_addr32[1]) &&	\
	 (a.s6_addr32[2] == b.s6_addr32[2]) &&	\
	 (a.s6_addr32[3] == b.s6_addr32[3]))


void process_ipv4_packet (const void * buf, size_t data_len);  
void process_ipv6_packet (const void * buf, size_t data_len);

void process_icmpv4_packet (const void * buf);
void process_icmpv6_packet (const void * buf);

void convert_icmp_4to6 (struct icmphdr * icmp, struct icmp6_hdr * icmp6);
void convert_icmp_4to6_unreach (struct icmphdr * icmp, 
				struct icmp6_hdr * icmp6);

void convert_icmp_6to4 (struct icmp6_hdr * icmp6, struct icmphdr * icmp);
void convert_icmp_6to4_unreach (struct icmp6_hdr * icmp6,
				struct icmphdr * icmphdr);

void process_ipv4_fragment_packet (const void * buf);
void process_ipv6_fragment_packet (const void * buf);

void convert_fragment_4to6 (struct ip * ip, struct ip6_frag * ip6_f);
void convert_fragment_6to4 (struct ip6_frag * ip6_f, struct ip * ip);

u_int16_t checksum (void * data, int bufsiz);
void ipv4_checksum (struct ip * ip);
void icmpv4_checksum (struct icmphdr * icmp, void * payload, int plen);
void icmpv6_checksum (struct icmp6_hdr * icmp6, struct ip6_hdr * ip6, 
		      void * pad, int len);

void cleanup_sigint (int sig);

#endif /* _SA46T_H_ */
