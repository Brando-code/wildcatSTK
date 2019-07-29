#!/usr/local/bin/python3.6

import sys
import pandas as pd
import json
from collections import OrderedDict

inputRelativePath = "ExcelInputs/"

if len(sys.argv) < 2:
    raise RuntimeError("E: JSONExcelDataSerialiser : input file name argument is expected. Aborting..")
else:
    print("Serialising to JSON Excel file: " + inputRelativePath + sys.argv[1])

inputFileName = sys.argv[1]

outputRelativePath = "JSONOutputs/"
outputFileName = inputFileName.split(".")[0] + ".json"

excelData = pd.read_excel(inputRelativePath + inputFileName)
dataList = list()

dateColumnHeader = excelData.columns[0]

for column in excelData.columns:
    if column == dateColumnHeader:
        pass
    else:
        record_asDf = pd.DataFrame()
        record_asDf[dateColumnHeader] = excelData[dateColumnHeader]
        record_asDf[column] = excelData[column]
        record_asDf.dropna(inplace=True)

        record = OrderedDict()
        record[dateColumnHeader] = record_asDf[dateColumnHeader].dt.strftime("%Y/%m/%d").tolist()
        record[column] = record_asDf[column].tolist()

        dataList.append(record)

j = json.dumps(dataList, sort_keys=True, indent=4, separators=(',', ': '))

print("Writing JSON to file: " + outputRelativePath + outputFileName)
with open(outputRelativePath + outputFileName, 'w') as f:
    f.write(j)

print("Done.")
