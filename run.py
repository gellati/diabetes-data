#!/usr/bin/env python
# -*- coding: utf-8 -*-

from datetime import datetime
import pandas as pd
from bokeh.charts import TimeSeries, show, output_file
import numpy as np

from HealthKit import HealthKit
from Moodmetric import Moodmetric
from DexcomG4 import DexcomG4
from Firstbeat import Firstbeat

"""

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


"""

Firstbeatparser = Firstbeat()
firstbeatDir = 'Firstbeat/raw/'
firstbeatfiles = ['Mikael Rinnetmaki_20151110_065139_3501956.csv',
                  'Mikael Rinnetmaki_20151111_053000_3501957.csv',
                  'Mikael Rinnetmaki_20151112_073000_3501958.csv',
                  'Mikael Rinnetmaki_20151113_070000_3501959.csv',
                  'Mikael Rinnetmaki_20151114_091500_3501960.csv']

#firstbeatfiles = ['testdata.csv', 'testdata2.csv']

skipHeaders = False
count = 0
firstbeatdata = []

#skipheaders = True
for i in range(len(firstbeatfiles)):
    if count > 0:
        skipHeaders = True
    firstbeatdata.extend(Firstbeatparser.parseDataFile(firstbeatDir + firstbeatfiles[i], skipHeaders))
    count = count + 1

print firstbeatdata[:3]

#outfile = 'firstbeatOut.csv'    
#Firstbeatparser.cleanDataOutfile(outfile, firstbeatdata)
    
print len(firstbeatdata)

#firstbeatCD = 'Firstbeat_CD/'
firstbeatCD = 'CleanData/'

firstbeatOutfiles = ['Firstbeat_StateVector',
                     'Firstbeat_ArtifactCorrectedHRVector',
                     'Firstbeat_METMaxPercentageVector',
                     'Firstbeat_VO2Vector',
                     'Firstbeat_EPOCVector',
                     'Firstbeat_RespRVector',
                     'Firstbeat_VentilationVector',
                     'Firstbeat_EEVector',
                     'Firstbeat_EEpFatVector',
                     'Firstbeat_ResourceVector',
                     'Firstbeat_AbsoluteStressVector',
                     'Firstbeat_AbsoluteRelaxationVector',
                     'Firstbeat_ScaledStressVector',
                     'Firstbeat_ScaledRelaxationVector',
                     'Firstbeat_VLFVector',
                     'Firstbeat_LFVector',
                     'Firstbeat_HFVector',
                     'Firstbeat_HF2Vector',
                     'Firstbeat_RSAAmplitudeVector',
                     'Firstbeat_Splits',
                     'Firstbeat_JournalMarkers']

npfirstbeat = np.array(firstbeatdata)

for i in range(len(firstbeatOutfiles)):
    Firstbeatparser.cleanDataOutfile(firstbeatCD + firstbeatOutfiles[i], npfirstbeat[:,[1, i+2]])

print Firstbeatparser.convertToUnixTime('13.11.2015-23:51:46')

    

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
