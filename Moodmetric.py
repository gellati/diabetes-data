#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, csv, re, datetime
import numpy as np

class Moodmetric(object):
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
        patt = re.compile("[\t]+")
        for r in datarows:
            data.append(r[1:])
        #    t = datetime.datetime.fromtimestamp(int(float(r[1]))).strftime('%Y-%m-%d %H:%M:%S')
        #    r[1] = t

        return data

    def cleanDataOutfile(self, outfile, data):
        data = np.vstack([['Time', 'Value'], data])
#        data.insert(0, ['Time', 'Value'])
        with open(outfile, 'w') as file:
            output = csv.writer(file, delimiter=",")
            output.writerows(data)
        

    


    #            rowdata = patt.findall(r)
#            rowdata = r.split(',')

    #            rowdata = patt.findall(r)
#            rowdata = re.split(r'\t+', r)
#            rowdata = r.split('"\t+"') #('\t+\s+')
            
