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



#include "sa46t.h"

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>
#include <signal.h>

#ifndef linux
#include <string.h>
#include <sys/uio.h>
#endif


/* SA46T Configuration Related */
struct in6_addr sa46t_prefix = IN6ADDR_ANY_INIT;
u_int32_t       plane_id     = 0x00000000;

int tun_fd;		/* Tunnel Interface */



int
main (int argc, char * argv[])
{
	int ch;
	int d_flag = 0;
	extern char * optarg;
	extern int opterr;

	char tunname[IFNAMSIZ];

	int  ether_type;
	int  read_len;
	char buf[BUF_LEN];
	struct tun_pi * pi = (struct tun_pi *) buf;


	while ((ch = getopt (argc, argv, "s:p:d")) != -1) {
		switch (ch) {
		case 's' :
			if (optarg == NULL) {
				usage ();
				return -1;
			}
			if (inet_pton (AF_INET6, optarg, &sa46t_prefix) < 0) {
				usage ();
				return -1;
			}
			if (!IS_NETWORK_PREFIX (sa46t_prefix)) {
				printf (" invalid sa46t prefix %s",
					optarg);
				return -1;
			}
			break;

		case 'p' :
			if (optarg == NULL) {
				usage ();
				return -1;
			}
			if (sscanf (optarg, "%x", &plane_id) < 1) {
				printf ("Plane id ID id specified by hex.\n");
				return -1;
			}
			break;

		case 'd' :
			d_flag = 1;
			break;

		default :
			usage ();
			return -1;
		}
	}
	if (COMPARE_INET6_ADDR (sa46t_prefix, in6addr_any)) {
		usage ();
		return -1;
	}

#ifdef linux
	strncpy (tunname, TUNIF_NAME, IFNAMSIZ);
#else
	sprintf (tunname, "tun%d",tun_get_next_intnum ());
#endif

	if ((tun_fd = tun_alloc (tunname)) < 0) 
		return -1;
	
	if (tun_up (tunname) < 0)
		return -1;

	if (signal (SIGINT, cleanup_sigint)  == SIG_ERR) 
		err (EXIT_FAILURE, "failt to register SIGINT");

	if (d_flag > 0) {
		if (daemon (1, 1) != 0) 
			err (EXIT_FAILURE, "fail to run as a daemon\n");
	}


	while ((read_len = read (tun_fd, buf, sizeof (buf))) >= 0) {
		
		ether_type = ntohs (pi->proto);

		switch (ether_type) {
		case ETH_P_IP :
			process_ipv4_packet (buf + sizeof (struct tun_pi), 
					     (size_t)read_len);
			break;

		case ETH_P_IPV6 :
			process_ipv6_packet (buf + sizeof (struct tun_pi),
					     (size_t)read_len);
			break;
			
		default :
			break;
		}
	}
	
	/* read(2) error */
	err (EXIT_FAILURE, "read from %s failed.", tunname);

	/* not reached */
	return -1;
}


void
process_ipv4_packet (const void * buf, size_t data_len)
{
	struct ip * ip = (struct ip *) buf;
	struct ip6_hdr 	ip6;
	struct tun_pi 	pi;
	struct iovec 	iov[4];

	if (ntohs (ip->ip_len) > data_len) {
		warn ("datalen is shorter than ip_len");
		return;
	}

	sa46t_convert_4to6 (ip, &ip6, sa46t_prefix, plane_id);

	tun_set_af (&pi, AF_INET6);	
	iov[0].iov_base = &pi;
	iov[0].iov_len	= sizeof (pi);
	iov[1].iov_base = &ip6;
	iov[1].iov_len 	= sizeof (ip6);
	iov[2].iov_base = ip;
	iov[2].iov_len = ip->ip_hl * 4;
	iov[3].iov_base = (void *)(buf + (ip->ip_hl * 4));
	iov[3].iov_len 	= ntohs (ip6.ip6_plen);

	if (writev (tun_fd, iov, 4) < 0)
		warn ("writev IPv6 Packet failed ");

	return;
}



void
process_ipv6_packet (const void * buf, size_t data_len)
{
	struct ip6_hdr * ip6;
	struct ip ip;
	struct tun_pi pi;
	struct iovec iov[2];

	ip6 = (struct ip6_hdr *) buf;

	if (ntohs (ip6->ip6_plen) > data_len) {
		warn ("datalen is shorter than ip6_len");
		return;
	}

	/* if packet is from IPv6 Network except configured SA46T Prefix or 
	 * to IPv6 network except own SA46T Prefix, drop it.
	 */
	if (!SA46T_CHECK_PREFIX (ip6->ip6_dst, sa46t_prefix, plane_id) ||
	    !SA46T_CHECK_PREFIX (ip6->ip6_src, sa46t_prefix, plane_id) ) {
		warn ("sa46t prefix and plane id  mismatch.");
		return;
	}

	sa46t_convert_6to4 (&ip, ip6);
	ipv4_checksum (&ip);
	
	tun_set_af (&pi, AF_INET);
	iov[0].iov_base = &pi;
	iov[0].iov_len	= sizeof (pi);
	iov[1].iov_base = (void *)(buf + sizeof (struct ip6_hdr));
	iov[1].iov_len 	= ntohs (ip6->ip6_plen);

	if (writev (tun_fd, iov, 2) < 0)
		warn ("writev IPv6 Packet failed.");

	return;
}



u_int16_t
checksum (void * data, int bufsz)
{
        u_int32_t sum = 0;
        u_int16_t * buf;
        
        buf = (u_int16_t *) data;
        
        while (bufsz > 1) {
                sum += *buf;
                buf++;
                bufsz -= 2;
        }

        if (bufsz == 1) {
                sum += * (unsigned char *)buf;
        }
        
	sum = (sum & 0xffff) + (sum >> 16);
	sum += (sum >> 16);

        return ~sum;
}


void
ipv4_checksum (struct ip * ip)
{
	ip->ip_sum = checksum (ip, ip->ip_hl * 4);

	return;
}



void
cleanup_sigint (int sig)
{
	close (tun_fd);
	exit (0);
}

void
usage (void)
{
	printf ("\n");
	printf (" usage:\n");
	printf ("\tsa46t -s [SA46T PREFIX] -p [PLANE ID(HEX)] -d\n");
	printf ("\n");
	printf (" Option:\n");
	printf ("\t-s : SA46T Address Prefix(/64).\n");
	printf ("\t-p : Specify SA46T Plane ID with 32bit HEX.\n");
	printf ("\t-d : Run as a Daemon.\n");
	printf ("\n");
	return;
}
