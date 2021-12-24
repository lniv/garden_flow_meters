#!/usr/bin/python3.7
'''
server collecting data from flow meter nodes

for now, just log what you get.
'''

import logging
import argparse
import socket

logging.basicConfig(level = logging.INFO)


def setup_server(udp_ip, udp_port):
    '''
    Args:
        udp_ip : a quad, in the form of a string
        udp_port : port number, integer
    '''
    logger = logging.getLogger('data server')
    logger.info(f'Listening on {udp_ip}:{udp_port}')
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind((udp_ip, udp_port))

        while True:
            try:
                data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
                logger.info(f'received from {addr} message: {data}')
            except KeyboardInterrupt:
                logger.info('terminating data collection')
                break


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description= 'data collection server for flow meters')
    parser.add_argument('--udp_ip', required = True, help='udp address, quad string')
    parser.add_argument('--udp_port', help = 'udp port, integer', default = 5557)
    args = parser.parse_args()
    setup_server(args.udp_ip, args.udp_port)