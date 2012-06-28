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



#ifndef _IFTUN_H_
#define _IFTUN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>


#ifdef linux
#include <linux/if_tun.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>

/* for linux/if_tun.h compatibility */
struct tun_pi {
        u_int16_t flags;
        u_int16_t proto;
};
#define ETH_P_IP	ETHERTYPE_IP
#define ETH_P_IPV6	ETHERTYPE_IPV6

int tun_get_next_intnum (void);
#endif


int tun_alloc (char * dev);
int tun_up (char * dev);

int tun_set_af(void * buf, u_int32_t af);


#endif /* _IFTUN_H_ */
