//
// Created by Alberto Campi on 2019-09-24.
//

#include "AuxiliaryVariable.h"

Common::AuxiliaryVariable::AuxiliaryVariable(const std::string &auxiliaryVariableName,
                                             const std::string &auxiliaryVariableType,
                                             const std::string &auxiliaryVariableSubType,
                                             const std::string &auxiliaryVariableExpression) :
         m_auxVarName(auxiliaryVariableName),
         m_auxType(auxiliaryVariableType),
         m_auxSubType(auxiliaryVariableSubType),
         m_auxVarExpression(auxiliaryVariableExpression)
{

}

std::string Common::AuxiliaryVariable::getAuxiliaryVariableName() const
{
    return m_auxVarName;
}

std::string Common::AuxiliaryVariable::getAuxiliaryVariableType() const
{
    return m_auxType;
}

std::string Common::AuxiliaryVariable::getAuxiliaryVariableSubType() const
{
    return m_auxSubType;
}

std::string Common::AuxiliaryVariable::getAuxiliaryVariableExpression() const
{
    return m_auxVarExpression;
}

Common::InvertibleAuxiliaryVariable::InvertibleAuxiliaryVariable(const std::string &auxiliaryVariableName,
                                                                 const std::string &auxiliaryVariableType,
                                                                 const std::string &auxiliaryVariableSubType,
                                                                 const std::string &auxiliaryVariableExpression,
                                                                 const std::string &inverseAuxiliaryVariableName,
                                                                 const std::string &inverseAuxiliaryVariableExpression) :
        AuxiliaryVariable(auxiliaryVariableName, auxiliaryVariableType, auxiliaryVariableSubType, auxiliaryVariableExpression),
        m_inverseAuxVarName(inverseAuxiliaryVariableName),
        m_inverseAuxVarExpression(inverseAuxiliaryVariableExpression)
{

}

std::string Common::InvertibleAuxiliaryVariable::getInverseAuxiliaryVariableName() const
{
    return m_inverseAuxVarName;
}

std::string Common::InvertibleAuxiliaryVariable::getInverseAuxiliaryVariableExpression() const
{
    return m_inverseAuxVarExpression;
}