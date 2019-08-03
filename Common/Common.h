//
// Created by Alberto Campi on 22/07/2019.
//

#ifndef WILDCATSTKCORE_COMMON_H
#define WILDCATSTKCORE_COMMON_H

namespace Common
{
    class TimeSeries;
    class DataSet;
    class ConfigVariable;
    class ConfigModelSpec;
    class ConfigModelSpecRelative;

    class TransformationType;
    class FirstDifferenceTransformation;
    class SimpleReturnTransformation;
    class LogReturnTransformation;
    //class LevelTransformation;

    class TransformationTypeFactory;
    class FirstDifferenceTransformationFactory;
    class SimpleReturnTransformationFactory;
    class LogReturnTransformationFactory;
    class LevelTransformationFactory;
    
    class StringSplit;
    class StringSplitDecorator;
    class StringSplitConfigVariableDecorator;
    class JSONParser;
    class JSONParserDecorator;
    class JSONParserDecoratorDataSet;
    class CSVParser;
    class CSVParserDataSet;
}

#endif //WILDCATSTKCORE_COMMON_H
