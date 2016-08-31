#!/usr/bin/env python
# -*- coding: utf-8 -*-

from datetime import datetime
import pandas as pd
from bokeh.charts import TimeSeries, show, output_file
import numpy as np

from HealthKit import HealthKit
from Moodmetric import Moodmetric
from DexcomG4 import DexcomG4

healthKitDir = 'HealthKit/'

flightsClimbedFile = healthKitDir + 'FlightsClimbed.csv'
heartRateFile = healthKitDir + 'HeartRate.csv'
stepsFile = healthKitDir + 'Steps.csv'


HealthKit = HealthKit()
flightsData = HealthKit.parseDataFileUnixTime(flightsClimbedFile)
flightsOutfile = 'CleanData/FlightsClimbed_cd.csv'
HealthKit.cleanDataOutfile(flightsOutfile, flightsData)

heartRateData = HealthKit.parseDataFileUnixTime(heartRateFile)
heartRateOutfile = 'CleanData/HeartRate_cd.csv'
HealthKit.cleanDataOutfile(heartRateOutfile, heartRateData)

stepsData = HealthKit.parseDataFileUnixTime(stepsFile)
stepsOutfile = 'CleanData/Steps_cd.csv'
HealthKit.cleanDataOutfile(stepsOutfile, stepsData)
print HealthKit.convertToUnixTime('09-Nov-2015 00:00')

DexcomDir = 'DexcomG4/raw/'
glucoseFile = DexcomDir + 'DexcomG4_exported.csv'

Dexcom = DexcomG4()
glucoseData = Dexcom.parseDataFileUnixTime(glucoseFile)
npglucose = np.array(glucoseData)
glucoseOutfile = 'CleanData/Dexcom_cd.csv'
Dexcom.cleanDataOutfile(glucoseOutfile, npglucose[:,[0,2]]) #glucoseData)




moodDir = 'Moodmetric/raw/'
moodfile = moodDir + 'moodmetric-slush_dd.csv'
Moodmetricparser = Moodmetric()
moodData = Moodmetricparser.parseDataFile(moodfile)

moodOutFiles = ['CleanData/Moodmetric_c1_cd.csv',
                'CleanData/Moodmetric_c2_cd.csv',
                'CleanData/Moodmetric_c3_cd.csv',
                'CleanData/Moodmetric_c4_cd.csv',
                'CleanData/Moodmetric_c5_cd.csv']

npmood = np.array(moodData)

for i in range(len(moodOutFiles)):
    Moodmetricparser.cleanDataOutfile(moodOutFiles[i], npmood[:,[0,i+1]])











#### below code is for visualisation with bokeh

#parse = lambda x: datetime.strptime(x, '%d-%b-%Y %H:%M')
#FC = pd.read_csv(FlightsClimbedFile, parse_dates = ['Start'],
#            date_parser = parse)
#HR = pd.read_csv(HeartRateFile, parse_dates = ['Start'],
#            date_parser = parse)
#S = pd.read_csv(StepsFile, parse_dates = ['Start'],
#            date_parser = parse)

#flightsvalues = pd.DataFrame(dict(
#    climb= FC['Flights Climbed (count)'], # ok
#    rate=HR['Heart Rate (count/min)'],
#    steps=S['Steps (count)'],
#    time=FC['Start']
#    ))
#output_file("steps_timeseries.html")

#p = TimeSeries(flightsvalues, x='time', legend=True, title='steps', ylabel='steps')
#show(p)


