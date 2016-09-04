#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, csv

from datetime import datetime, time

epoch = datetime.utcfromtimestamp(0)

class DexcomG4(object):
    def __init__(self): pass
        

    def readDataFile(self, filename):
        file = filename
        datarows = []
        with open(file, 'rb') as f:
            reader = csv.reader(f)
            for row in reader:
                datarows.append(row)
        return datarows

    def parseDataFile(self, filename):
        datarows = self.readDataFile(filename)
        data = []
        for r in datarows:
            rowdata = r.split(',')

            data.append(rowdata)
        return data

    def parseDataFileUnixTime(self, filename):
        datarows = self.readDataFile(filename)
        data = []
        r0 = datarows[0]
        print r0
        data.append(r0)
        for r in datarows[1:]:
            r[0] = self.convertToUnixTime(r[0])
            r[1] = self.convertToUnixTime(r[1])
            if(r[2] == 'Low'):
                r[2] = 2
       #         print r     
            data.append(r)
        return data

# 2015-11-09 09:07:26, convert this to unix time
    def convertToUnixTime(self, t):
        d = datetime.strptime(t, '%Y-%m-%d %H:%M:%S')
        return (d - epoch).total_seconds() #* 1000.0


    
    def cleanDataOutfile(self, outfile, data):
        with open(outfile, 'w') as file:
            output = csv.writer(file, delimiter=",")
            output.writerows(data)

    
