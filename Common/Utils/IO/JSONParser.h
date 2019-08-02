//
// Created by Alberto Campi on 30/07/2019.
//

#ifndef WILDCATSTKCORE_JSONPARSER_H
#define WILDCATSTKCORE_JSONPARSER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../../Common.h"
#include "../../Types/DataSet.h"

class Common::JSONParser
{
public:
    void readJSON(const std::string& fileName, boost::property_tree::ptree& tree) const;
    void writeJSON(const std::string& fileName, const boost::property_tree::ptree& tree) const;

protected:
    //boost::property_tree::ptree m_tree;
};


class Common::JSONParserDecorator
{
public:
    virtual void readJSON(const std::string& fileName) = 0;

    virtual ~JSONParserDecorator() = default;

protected:
    Common::JSONParser m_parser;
};


class Common::JSONParserDecoratorDataSet : public Common::JSONParserDecorator
{
public:
    void readJSON(const std::string& fileName) final;
    void writeJSON(const std::string& fileName, const Common::DataSet& ds) const;
    Common::DataSet getDataSet() const;

private:
    Common::DataSet m_ds;
};

#endif //WILDCATSTKCORE_JSONPARSER_H
