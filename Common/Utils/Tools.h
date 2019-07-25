//
// Created by Alberto Campi on 24/07/2019.
//

#ifndef WILDCATSTKCORE_TOOLS_H
#define WILDCATSTKCORE_TOOLS_H

#include <boost/algorithm/string.hpp>
#include <vector>
#include "../Common.h"


//StringSplit class declaration
class Common::StringSplit
{
public:
    std::vector<std::string> split(const std::string& string, const std::string& pattern) const;
};


//StringSplit decorator declarations
class Common::StringSplitDecorator
{
public:
    StringSplitDecorator() = default;

    virtual void split(const std::string& string, const std::string& pattern) = 0;
    virtual std::vector<std::string> get() const = 0;

    virtual ~StringSplitDecorator() = default;

protected:
    std::vector<std::string> m_components;
};


class Common::StringSplitConfigVariableDecorator : public Common::StringSplitDecorator
{
public:
    StringSplitConfigVariableDecorator();

    void split(const std::string& string, const std::string& pattern) final;

    std::vector<std::string> get() const final;
    std::string getBasename() const;
    std::string getTransformationTypeCode() const;
    unsigned int getLagDependency() const;

private:
    const Common::StringSplit m_strplit;
};


#endif //WILDCATSTKCORE_TOOLS_H
