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
dataDict = dict()

dateColumnHeader = excelData.columns[0]
valuesKeyName = "Values"
# variableKeyName = "VariableName"
datesKeyName = "Dates"

for column in excelData.columns:
    if column == dateColumnHeader:
        pass
    else:
        node_asDf = pd.DataFrame()
        node_asDf[dateColumnHeader] = excelData[dateColumnHeader]
        node_asDf[column] = excelData[column]
        node_asDf.dropna(inplace=True)

        node = OrderedDict()
        node[datesKeyName] = node_asDf[dateColumnHeader].dt.strftime("%Y-%m-%d").tolist()
        node[valuesKeyName] = node_asDf[column].tolist()
        # node[variableKeyName] = column

        dataDict[column] = node

j = json.dumps(dataDict, sort_keys=True, indent=4, separators=(',', ': '))

print("Writing JSON to file: " + outputRelativePath + outputFileName)
with open(outputRelativePath + outputFileName, 'w') as f:
    f.write(j)

print("Done.")
