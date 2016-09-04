#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, csv, re
from datetime import datetime, time

epoch = datetime.utcfromtimestamp(0)

class Firstbeat(object):
    def __init__(self): pass

    def readDataFile(self, filename):
        file = filename
        datarows = []
        with open(file, 'rb') as f:
            reader = csv.reader(f, delimiter=';')
            for row in reader:
                datarows.append(row)
        return datarows

    def parseDataFile(self, filename, skipHeaders):
        datarows = self.readDataFile(filename)
        data = []
        sessionStartDate = ""
        dataStarts = False
        timePattern = re.compile('24')
        for r in datarows:
#            print r
            if not r:
                continue
            if r[0].startswith('SessionStartDate'):
                sessionStartDate = r[1]
                continue
            if r[0].startswith('VECTORS'):
                dataStarts = True
                continue
            if skipHeaders == False and r[0].startswith('CumulativeSecondVector'):
                data.append(r)
                skipHeaders = True
                continue
            if skipHeaders == True and r[0].startswith('CumulativeSecondVector'):
                continue                
            if dataStarts == True:
                if r[1] == '24:00:00':   # turn of the date at midnight
                    sessionStartDate = self.incrementDate(sessionStartDate)
                if timePattern.match(r[1]):
                    r[1] = r[1].replace('24', '00', 1)
#                    print r[1]
                r[1] = sessionStartDate + '-' + r[1]
                r[1] = self.convertToUnixTime(r[1])
                    
                data.append(r)

        return data

    # increment date string of following format: 14.11.2015
    def incrementDate(self, d):
        dc = d.split('.')
        dc[0] = str(int(dc[0]) + 1)
        return ".".join(dc)
        
    def convertToUnixTime(self, t):
        d = datetime.strptime(t, '%d.%m.%Y-%H:%M:%S')
        return (d - epoch).total_seconds() # * 1000.0

    def cleanDataOutfile(self, outfile, data):
        with open(outfile, 'w') as file:
            output = csv.writer(file, delimiter=",")
            output.writerows(data)
