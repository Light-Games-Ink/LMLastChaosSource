#!/usr/bin/python2.6
#-*- coding: euc-kr -*-

import os
import sys
import time
import datetime

log_dir = './LogFiles'
yesterday = datetime.date.fromtimestamp(time.time() - (86400)).strftime('%Y-%m-%d')

# �����ڵ�, ������, [] - ��ȿ�� IP
nation_info = {
    '0' : ['�ѱ�',
           ['10.1.40.2', '10.1.40.82', '10.1.90.30', '10.1.90.8']],
    '4' : ['�±�',
           ['10.1.90.27', '10.1.90.82', '202.183.192.50', '202.183.192.51', '103.251.70.30']],
    '9' : ['�̱�',
           ['10.1.90.29', '10.1.90.84']],
    '10' : ['�����',
            ['10.1.90.25', '10.1.90.26', '10.1.90.80', '10.1.90.81']],
    '13' : ['����',
            ['10.1.90.28', '10.1.90.83']],
    '14' : ['������',
            []],
    '15' : ['������',
            ['10.1.90.31', '10.1.90.86']],
    '16' : ['������',
            []],
    '17' : ['���þ�',
            ['10.1.90.33', '10.1.90.88']],
    '18' : ['��Ű',
            []],
    '19' : ['��Ż����',
            []],
    '20' : ['�߽���',
            ['10.1.90.25', '10.1.90.26', '10.1.90.80', '10.1.90.81']],
    '24' : ['����',
            []]
}

server = {} # ��� ���� ������ ������ ����
client = {} # ��� Ŭ���̾�Ʈ ������ ������ ����

server_check_keyword = 'SERVER_INFO -'
client_check_keyword = 'CLIENT_INFO -'

def line_analysis(store, keyword, line):
    line = line[:-1]
    t = line.split(keyword)
    info = t[1].split(' : ')
    del(info[::2])  # �����͸� ����

    nation = info[0]
    ip = info[1]
    port = info[2]
    client_ip = info[3]

    # ���ο� ������� ���� �����͸� �߰�
    if nation not in store:
            store[nation] = []

    # ���� ��� �������, ��ȿ IP�� ����
    if nation in nation_info:
            if ip in nation_info[nation][1]:
                    return

    # ��ϵ� IP�� �ƴ϶��
    if ip not in store[nation]:
            store[nation].append(ip)
   
def print_result(f, store):
    for key in store.keys():
        # ������ ����ϱ�
        if key in nation_info:
            f.write('[%s] -----\n' % nation_info[key][0])
        else:
            f.write('[Unknown] -----\n')

        # IP ����ϱ�
        for ip in store[key]:
            f.write('%s\n' % ip)

        f.write('\n')

#########################################################
# �α� ���� ��������
logfile = log_dir + '/authentication.log.' + yesterday

with open(logfile) as f:
    lines = f.readlines();
    for line in lines:
        # ���� ������ �м�
        if server_check_keyword in line:
            line_analysis(server, server_check_keyword, line)
            
        # Ŭ���̾�Ʈ ������ �м�
        elif client_check_keyword in line:
            line_analysis(client, client_check_keyword, line)

#########################################################
# ������ ������ �������� ���� ����
result_filename = 'result.txt'
with open('result.txt', 'wt') as f:
    f.write('To: ��Ʈī����������\n')
    f.write('From: ��ī��������<icebug0@barunsonena.com>\n')
    f.write('Subject: ��Ʈī���� - ���� �⵿ ����\n\n')
    
    f.write('-------------------- ����� ������ IP --------------------\n\n')
    print_result(f, server)

    f.write('\n\n\n')

    f.write('-------------------- Ŭ���̾�Ʈ�� ������ ���Ӽ����� IP --------------------\n\n')
    print_result(f, client)
