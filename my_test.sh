#!/bin/bash
# Need a remote_pub privkey and pubkey in this folder
our_id=$1 #Can be 1 or 2
their_ip=$2 #IPv4
listen_port=$our_id #Our listening port
echo To listen on $listen_port
their_pub=$(cat remote_pub) #Should've got earlier
echo their_pub $their_pub
if [[ $our_id -eq 1 ]]; then 
    their_id=2
else 
    their_id=1
fi

our_dev=wg$our_id
our_wg_ip4=192.168.241.$our_id
their_wg_ip4=192.168.241.$their_id
our_wg_ip6=fd00::$our_id
their_wg_ip6=fd00::$their_id
ip link add dev $our_dev type wireguard    
ip addr add $our_wg_ip4/24 dev $our_dev
ip addr add $our_wg_ip6/24 dev $our_dev
wg set $our_dev private-key privkey  \
                listen-port "$listen_port" \
                peer "$their_pub" \
                allowed-ips $their_wg_ip4,$their_wg_ip6 \
                endpoint $their_ip:$their_id
ip link set up dev $our_dev
