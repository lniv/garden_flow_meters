#!/usr/bin/python3.7
'''
server collecting data from flow meter nodes

for now, just log what you get.
'''

import os
from datetime import datetime
import logging
import argparse
import socket

def clear_and_setup_loggers(log_folder = '/tmp', log_level = logging.INFO):
    '''
    clear all previous handlers, and set up a screen / file logger.
    Args:
        log_folder: where we'll place log files. if None, use tempfile's temp folder.
        log_level: as it says [logging.INFO]
    Returns:
        path to log file
    '''
    if not os.path.exists(log_folder):
        print(f'log folder {log_folder} does not existing, creating')
        os.makedirs(log_folder, exist_ok=False)

    rlogger = logging.getLogger()
    #remove existing handlers that may exist from previous tests.
    while len(rlogger.handlers) > 0:
        handler = rlogger.handlers[0]
        rlogger.removeHandler(handler)
        handler.close()

    rlogger.setLevel(log_level)

    #set up to log to file
    log_file_name = datetime.now().strftime('flow_meters_%Y-%m-%d-%H-%M-%S.log')
    log_file_path = os.path.join(log_folder, log_file_name)
    fh = logging.FileHandler(log_file_path)
    fh.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
    rlogger.addHandler(fh)
    #and to console
    sh = logging.StreamHandler()
    sh.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
    rlogger.addHandler(sh)

    return log_file_path


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
    parser.add_argument('--log_folder', help = 'folder for log file', default = '/tmp')
    args = parser.parse_args()
    log_file_path = clear_and_setup_loggers(args.log_folder)
    print(f'Saving log to {log_file_path}')
    setup_server(args.udp_ip, args.udp_port)
    print(f'logs in {log_file_path}')