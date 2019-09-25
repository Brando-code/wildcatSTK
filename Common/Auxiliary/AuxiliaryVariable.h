//
// Created by Alberto Campi on 2019-09-24.
//

#ifndef WILDCATSTKCORE_AUXILIARYVARIABLE_H
#define WILDCATSTKCORE_AUXILIARYVARIABLE_H

#include <string>


namespace Common
{
    class AuxiliaryVariable
    {
    public:
        AuxiliaryVariable(const std::string &auxiliaryVariableName,
                          const std::string &auxiliaryVariableType,
                          const std::string &auxiliaryVariableSubType,
                          const std::string &auxiliaryVariableExpression);

        std::string getAuxiliaryVariableName() const;
        std::string getAuxiliaryVariableType() const;
        std::string getAuxiliaryVariableSubType() const;
        std::string getAuxiliaryVariableExpression() const;

    private:
        const std::string m_auxVarName, m_auxType, m_auxSubType, m_auxVarExpression;
    };

    class InvertibleAuxiliaryVariable : public AuxiliaryVariable
    {
    public:
        InvertibleAuxiliaryVariable(const std::string &auxiliaryVariableName,
                                    const std::string &auxiliaryVariableType,
                                    const std::string &auxiliaryVariableSubType,
                                    const std::string &auxiliaryVariableExpression,
                                    const std::string &inverseAuxiliaryVariableName,
                                    const std::string &inverseAuxiliaryVariableExpression);

        std::string getInverseAuxiliaryVariableName() const;
        std::string getInverseAuxiliaryVariableExpression() const;

    private:
        const std::string m_inverseAuxVarName, m_inverseAuxVarExpression;
    };
}


#endif //WILDCATSTKCORE_AUXILIARYVARIABLE_H
