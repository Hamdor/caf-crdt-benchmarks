#!/usr/bin/python

import sys, getopt

from collections import defaultdict

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import CPULimitedHost
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel

HOST_NAMES = []
HOST_IPS = []
HOST_IP_PREFIX = '192.168.128.'
IP_CIDR = '/24'

SWITCHES = ['s1', 's2', 's3', 's4']

def generate_host_infos(num):
    global HOST_NAMES
    global HOST_IPS
    for h in range(num):
        HOST_NAMES.append('{}{}'.format('h', h+1))
        HOST_IPS.append('{}{}'.format(HOST_IP_PREFIX, (h+1)))

class MyTopo(Topo):
    def build(self):
        # Init Switches ###########################
        for name in SWITCHES:
            self.addSwitch(name)
        # Set links between switches
        self.addLink(SWITCHES[0], SWITCHES[1])  # TODO: loss and delay !
        self.addLink(SWITCHES[0], SWITCHES[2])  # delay='250ms', loss=5
        self.addLink(SWITCHES[0], SWITCHES[3])
        ###########################################
        # Init Hosts
        switch_idx = 0
        for idx in range(len(HOST_NAMES)):
            host = self.addHost(HOST_NAMES[idx], ip='{}{}'.format(HOST_IPS[idx], IP_CIDR))
            self.addLink(SWITCHES[switch_idx], host)
            switch_idx += 1
            if switch_idx == 4:
                switch_idx = 0

def print_help():
    print 'run.py -t <num_ticks> -l <num_listeners> -n <num_nodes>'
    sys.exit(2)

def main(argv):
    # Parse arguments
    num_ticks = 0
    num_listeners = 0
    num_nodes = 0
    try:
        opts, args = getopt.getopt(argv,"ht:l:n:",["ticks=,listeners=","nodes="])
    except getopt.GetoptError:
        print_help()
    for opt, arg in opts:
        if opt == '-h':
            print_help()
        elif opt in ("-t", "--ticks"):
            num_ticks = int(arg)
        elif opt in ("-l", "--listeners"):
            num_listeners = int(arg)
        elif opt in ("-n", "--nodes"):
            num_nodes = int(arg)
    # Generate host infos
    generate_host_infos(num_nodes)
    # Generate network
    topo = MyTopo()
    net = Mininet(topo, link=TCLink)
    net.start()
    # Put all ips into one string
    ips = ""
    for ip in HOST_IPS:
      ips += (ip + " ")
    ips[:-1] # remove last empty char
    print ips
    # Start benchmark
    fcommand = '{} {} {} {} {}'.format('./build/bin/caf-crdt-convergence-dist', num_ticks, 0, num_nodes, ips)
    command = '{} {} {} {} {}'.format('./build/bin/caf-crdt-convergence-dist', 0, num_listeners, num_nodes, ips)
    first = True
    for node in HOST_NAMES:
        if first:
            net.get(node).sendCmd('{} {}'.format(fcommand, node))
            first = None
        else:
            net.get(node).sendCmd('{} {}'.format(command, node))
    with open('dist_outputs/dist_output_{}.txt'.format(num_nodes), "a") as outfile:
        for node in HOST_NAMES:
            outfile.write(net.get(node).waitOutput())
    net.stop()

if __name__ == '__main__':
    main(sys.argv[1:])
