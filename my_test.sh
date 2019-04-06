#!/bin/bash
our_id = $1 #Can be 1 or 2
their_ip = $2 #IPv4
listen_port = $3 #Our listening port
their_pub = $4 #Should've got earlier

if [[ -z $their_ip || -z $their_pw || -z $our_port ]];then
    echo Inappropriate no. of args
    return 1;
fi

if [[$our_id -eq 1]]; then 
    their_id = 2
else 
    their_id = 1
fi

our_dev = wg$our_id
our_wg_ip4 = 192.168.241.$our_id
their_wg_ip4 = 192.168.241.$their_id
our_wg_ip6 = fd00::$our_id
their_wg_ip6 = fd00::$their_id
ip link add dev $our_dev type wireguard    
ip addr add $our_wg_ip4/24 dev $our_dev
ip addr add $our_wg_ip6/24 dev $our_dev
wg set $our_dev private-key privkey  \
                listen_port $listen_port \
                peer "$their_pub" \
                allowed-ips $their_wg_ip4,$their_wg_ip6 \
                endpoint $their_ip

