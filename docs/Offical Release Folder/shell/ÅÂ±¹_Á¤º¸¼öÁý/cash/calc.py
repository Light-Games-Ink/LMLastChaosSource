#!/usr/bin/python2.6
# -*- coding: cp949 -*-

import os
import sys
import MySQLdb as mysql
import datetime
import time

def mysqlconnect():
    myhost = '14.63.127.133'
    myid = 'laka'
    mypw = 'lakamcity'
    mydb = 'newproject_tld_cash_info'
    
    try:
        con = mysql.connect(myhost, myid, mypw, mydb)
        cur = con.cursor()
    except mysql.Error, e:
        print (e)
        return None, None

    return con, cur

def process_file(i, cur):
    filename = ('./result/%s_%s.txt') % (i, sys.argv[1])
    #print (filename)
    
    f = open(filename)
    alldata = f.readlines();
    f.close()
    #print (data)

    # �м��ϱ�
    cash_info_qry = []
    cash_item_qry = []
    
    for linedata in alldata:
        t = linedata.split('BUY_CASH_ITEM_START >')
        time = t[0].split(',')[0]
        
        tt = [a.strip(' \n') for a in t[1].split(':')]
        userIndex = tt[0]
        price = tt[-1]
        goods = tt[1:-1]

        cash_info_qry.append((i, time, userIndex, price))

        for item in goods:
            cash_item_qry.append((i, time, userIndex, item))

    # insert to db
    cur.executemany('''INSERT INTO cash_info(server_no, time, user_index, price)
        VALUES(%s,%s,%s,%s)''',
        cash_info_qry)

    cur.executemany('''INSERT INTO cash_item(server_no, time, user_index, item_index)
        VALUES(%s,%s,%s,%s)''',
        cash_item_qry)

def process_sum(cur):
    # �������� 30�� ���� �����͸� ����
    result = []
    for i in range(30, 0, -1):
        yesterday = datetime.date.fromtimestamp(time.time() - (i * 86400))
        yesterday_str = yesterday.strftime('%Y-%m-%d')
        #print (yesterday)
        qry = ("select sum(price) from cash_info where DATE_FORMAT(time, '%%Y-%%m-%%d') = '%s'" \
               % yesterday_str)
        cur.execute(qry)

        rows = cur.fetchall()
        if rows[0][0] == None:
            continue;
        
        #print (qry, rows[0][0])
        sum_price = int(rows[0][0]) / 200
        result.append( (yesterday_str, sum_price) )

    #print (result)
    f = open('sum_one_month.txt', 'wt')
    f.write('''To: ��Ʈī���� ������
From: ��ī�����ϼ���<icebug0@barunsongames.com>
Subject: ��Ʈī����[�±�] - ĳ�� ����\n\n''')
    for i in result:
        f.write('%s : %s\n' % i)
    f.close()

#################################################################    
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print ('\nUsage: ./calc.py work_date\n')
        sys.exit(1)

    con, cur = mysqlconnect()
    if con == None:
        sys.exit(1)
    
    for i in range(1, 5):
        process_file (i, cur)

    process_sum(cur)
    
    if cur:
        cur.close()
        
    if con:
        con.close()
        
