#!/bin/bash
# set -e

exec 3>&1
export WG_HIDE_KEYS=never
netns0="wg-test-$$-0"
netns1="wg-test-$$-1"
netns2="wg-test-$$-2"
pretty() { echo -e "\x1b[32m\x1b[1m[+] ${1:+NS$1: }${2}\x1b[0m" >&3; }
pp() { pretty "" "$*"; "$@"; }
maybe_exec() { if [[ $BASHPID -eq $$ ]]; then "$@"; else exec "$@"; fi; }
n0() { pretty 0 "$*"; maybe_exec ip netns exec $netns0 "$@"; }
n1() { pretty 1 "$*"; maybe_exec ip netns exec $netns1 "$@"; }
n2() { pretty 2 "$*"; maybe_exec ip netns exec $netns2 "$@"; }
ip0() { pretty 0 "ip $*"; ip -n $netns0 "$@"; }
ip1() { pretty 1 "ip $*"; ip -n $netns1 "$@"; }
ip2() { pretty 2 "ip $*"; ip -n $netns2 "$@"; }

tear_nodes() {
	ip1 link del dev wg0
	ip2 link del dev wg0
	pp ip netns del $netns1
	pp ip netns del $netns2
}
cleanup() {
	set +e
	exec 2>/dev/null
	printf "$orig_message_cost" > /proc/sys/net/core/message_cost
	ip0 link del dev wg0
	ip1 link del dev wg0
	ip2 link del dev wg0
	local to_kill="$(ip netns pids $netns0) $(ip netns pids $netns1) $(ip netns pids $netns2)"
	[[ -n $to_kill ]] && kill $to_kill
	pp ip netns del $netns1
	pp ip netns del $netns2
	pp ip netns del $netns0
	exit
}
trap cleanup EXIT

setup_nodes() {
ip netns del $netns0 2>/dev/null || true
ip netns del $netns1 2>/dev/null || true
ip netns del $netns2 2>/dev/null || true
pp ip netns add $netns0
pp ip netns add $netns1
pp ip netns add $netns2
ip0 link set up dev lo
ip0 link add dev wg0 type wireguard
ip0 link set wg0 netns $netns1
ip0 link add dev wg0 type wireguard
ip0 link set wg0 netns $netns2
}
setup_nodes
key1="$(pp wg genkey)"
key2="$(pp wg genkey)"
pub1="$(pp wg pubkey <<<"$key1")"
pub2="$(pp wg pubkey <<<"$key2")"
psk="$(pp wg genpsk)"
[[ -n $key1 && -n $key2 && -n $psk ]]
configure_peers() {
	ip1 addr add 192.168.241.1/24 dev wg0
	ip1 addr add fd00::1/24 dev wg0

	ip2 addr add 192.168.241.2/24 dev wg0
	ip2 addr add fd00::2/24 dev wg0

	n1 wg set wg0 \
		private-key <(echo "$key1") \
		listen-port 1 \
		peer "$pub2" \
			preshared-key <(echo "$psk") \
			allowed-ips 192.168.241.2/32,fd00::2/128
	n2 wg set wg0 \
		private-key <(echo "$key2") \
		listen-port 2 \
		peer "$pub1" \
			preshared-key <(echo "$psk") \
			allowed-ips 192.168.241.1/32,fd00::1/128

	ip1 link set up dev wg0
	ip2 link set up dev wg0

    n1 wg set wg0 peer "$pub2" endpoint 127.0.0.1:2
    n2 wg set wg0 peer "$pub1" endpoint 127.0.0.1:1
}
configure_peers

for packet_loss in 0.1% 0.5% 1% 1.5% 2.0%
do
for attempt in {1..50}
do
n1 tc qdisc add dev wg0 root netem loss $packet_loss
n1 ping -c 1 192.168.241.2 > tmp_results
# sudo make test > tmp_results
cat tmp_results | grep "packets transmitted" >> transmitted_results_$packet_loss.txt
cat tmp_results | grep rtt >> rtt_results_$packet_loss.txt
tear_nodes
setup_nodes
configure_peers
done
done
cleanup