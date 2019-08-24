//
// Created by Alberto Campi on 2019-08-22.
//

#ifndef WILDCATSTKCORE_ALGEBRAICOPERATORSYMBOLS_H
#define WILDCATSTKCORE_ALGEBRAICOPERATORSYMBOLS_H

#include <string>


namespace Global
{
    class AlgebraicOperatorSymbols
    {
    public:
        static AlgebraicOperatorSymbols* instance();
        std::string getAdditionSymbol() const;
        std::string getSubstractionSymbol() const;
        std::string getMultiplicationSymbol() const;
        std::string getDivisionSymbol() const;
        std::string getExponentiationSymbol() const;
        std::string getLeftBracketSymbol() const;
        std::string getRightBracketSymbol() const;

    private:
        AlgebraicOperatorSymbols();

        static AlgebraicOperatorSymbols* m_instance;
        const std::string m_additionSymbol;
        const std::string m_substractionSymbol;
        const std::string m_multiplicationSymbol;
        const std::string m_divisionSymbol;
        const std::string m_exponentiationSymbol;
        const std::string m_leftBracketSymbol;
        const std::string m_rightBracketSymbol;
    };
}



#endif //WILDCATSTKCORE_ALGEBRAICOPERATORSYMBOLS_H
