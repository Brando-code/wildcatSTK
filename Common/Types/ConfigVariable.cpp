//
// Created by Alberto Campi on 22/07/2019.
//

#include "ConfigVariable.h"
#include <cmath>

using namespace Common;

/*std::unique_ptr<TransformationType> TransformationTypeFactory::create(const std::string& transformationTypeCode)
{
    const enum allowedTransformationCodes {D, R, L, LR};
    switch (allowedTransformationCodes)
    {
        case D :
            return std::make_unique<TransformationType>(FirstDifferenceTransformation());
        case R :
            return std::make_unique<TransformationType>(SimpleReturnTransformation());
        case L :
            return std::make_unique<TransformationType>(LevelTransformation());
        case LR :
            return std::make_unique<TransformationType>(LogReturnTransformation());
        default:
            throw std::runtime_error("E: ");
    }
}*/

double FirstDifferenceTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    return ts.values.at(index) - ts.values.at(index - 1);
}

double FirstDifferenceTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return transformedValue + ts.values.at(index - 1);
}

double SimpleReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    if (ts.values.at(index - 1) != 0)
        return ts.values.at(index)/ts.values.at(index - 1) - 1;
    else
        throw std::overflow_error("E: SimpleReturnTransformation::getTransformedVariableValue : Division by zero error for variable " +
        ts.name + " at index " + std::to_string(index));
}

double SimpleReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return ts.values(index - 1) * (1 + transformedValue);
}

double LogReturnTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    return log(ts.values(index)) - log(ts.values(index - 1));
}

double LogReturnTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return exp(transformedValue) * ts.values(index - 1);
}

double LevelTransformation::getTransformedVariableValue(const TimeSeries &ts, int index) const
{
    return ts.values(index);
}

double LevelTransformation::getLevel(const TimeSeries &ts, double transformedValue, int index) const
{
    return ts.values(index);
}





