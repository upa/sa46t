SA46T
=====

This is SA46T (Stateless Automatic IPv4 over IPv6 Tunneling) open source
implementation using linux tap interface.

ID : http://datatracker.ietf.org/doc/draft-matsuhira-sa46t-spec/


Compile
-------

	 % git clone git://github.com/upa/sa46t.git
	 % cd sa46t
	 % make


How to Use
----------

Example) how to use sa46t with plane id 99, and IPv4 prefix "10.0.0.0/24", 
and mapping IPv6 prefix "2001:db8:1:1::/64". 

    /---------------------------------------------------\
    |                                                   |
    |                  Backbone Network                 |
    |                    (IPv6 only)                    |
    |                                                   |
    \---------------------------------------------------/
           |                                   |                
       +--------+                          +--------+            
       | SA46T1 |                          | SA46T2 |            
       +--------+                          +--------+           
           |                                   |              
     /--------------\                   /--------------\ 
     |              |                   |              |
     | Stub Network |                   | IPv4 Network |
     | 10.0.0.0/24  |                   | (Dual Stack) |
     |              |                   |              |
     \--------------/                   \--------------/


SA46T1 advertises prefix 2001:db8\:1\:1\:99::a000/120 on OSPF to Backbone
Network. And SA46T2 advertises 10.0.0.0/24 on OSPF to IPv4 Network.
Packets from IPv4 network to SA46T Stub Network, that have destination
address 10.0.0.0/24, are encapsulated by SA46T2 with IPv6 header. The
destination addrss of Encapsulated packets is
2001\:db8\:1\:1\:99::a00X. Thereby encapsulated packets arrives at SA46T1.
Then, packets are decapsulated and sent to the Stub Network.

The configuration of above topology is shown below.


### SA46T1 ###

	 % ./sa46t
	 
	  usage:
	         sa46t -s [SA46T PREFIX] -p [PLANE ID(HEX)] -d
	  
	  Option:
	         -s : SA46T Address Prefix(/64).
	         -p : Specify SA46T Plane ID with 32bit HEX.
	         -d : Run as a Daemon.
		 
	 # Create sa46t tunnel interface with plane ID 99
	 % sudo /sa46t -s 2001:db8:1:1:: -p 99 -d
	 % ifconfig sa46t
	 sa46t     Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  
	           UP POINTOPOINT RUNNING  MTU:1500  Metric:1
	           RX packets:0 errors:0 dropped:0 overruns:0 frame:0
	           TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
	           collisions:0 txqueuelen:500 
	           RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
	 %
	 % sudo ip route add to 10.0.0.0/24 dev sa46t
	 % 
	  
	 # and redistribute 10.0.0.0/24 to IPv4 Network using any routing daemonds.


### SA46T2 ###
 
	 % sudo /sa46t -s 2001:db8:1:1:: -p 99 -d
	 % ifconfig sa46t
	 sa46t     Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  
	           UP POINTOPOINT RUNNING  MTU:1500  Metric:1
	           RX packets:0 errors:0 dropped:0 overruns:0 frame:0
	           TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
	           collisions:0 txqueuelen:500 
	           RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
	 %
	 % sudo ip route add to 0.0.0.0/0 dev sa46t
	 % sudo ip -6 route add to 2001:db8:1:1:99::a000/108 dev sa46t
	 %
	  
	 # and redistribute 2001:db8:1:1:99::a000/108 to Backbone Network using any routing daemonds.



