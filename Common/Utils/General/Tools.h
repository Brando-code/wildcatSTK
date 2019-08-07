//
// Created by Alberto Campi on 24/07/2019.
//

#ifndef WILDCATSTKCORE_TOOLS_H
#define WILDCATSTKCORE_TOOLS_H

#include <boost/algorithm/string.hpp>
#include <vector>

namespace Common
{
    //StringSplit class declaration
    class StringSplit
    {
    public:
        std::vector<std::string> split(const std::string &string, const std::string &pattern) const;
    };


    //StringSplit decorator declarations
    class StringSplitDecorator
    {
    public:
        StringSplitDecorator();

        virtual void split(const std::string &string, const std::string &pattern) = 0;
        virtual std::vector<std::string> get() const = 0;

        virtual ~StringSplitDecorator() = default;

    protected:
        std::vector<std::string> m_components;
        Common::StringSplit m_strplit;
    };


    class StringSplitConfigVariableDecorator : public StringSplitDecorator
    {
    public:
        void split(const std::string &string, const std::string &pattern) final;

        std::vector<std::string> get() const final;
        std::string getBasename() const;
        std::string getTransformationTypeCode() const;
        unsigned int getLagDependency() const;
    };
}

#endif //WILDCATSTKCORE_TOOLS_H
