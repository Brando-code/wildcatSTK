//
// Created by Alberto Campi on 30/07/2019.
//

#include "JSONParser.h"

void Common::JSONParser::readJSON(const std::string& fileName, boost::property_tree::ptree& tree) const
{
    boost::property_tree::read_json(fileName, tree);
}

void Common::JSONParser::writeJSON(const std::string &fileName, const boost::property_tree::ptree &tree) const
{
    boost::property_tree::write_json(fileName, tree);
}

Common::DataSet Common::JSONParserDecoratorDataSet::getDataSet() const
{
    return m_ds;
}

void Common::JSONParserDecoratorDataSet::readJSON(const std::string& fileName)
{
    using namespace boost::property_tree;
    m_ds.clearAllData();
    ptree root;
    m_parser.readJSON(fileName, root);

    ptree::const_iterator it;
    for (it = root.begin(); it != root.end(); ++it)
    {
        const std::string variableName = it -> first;
        ptree childDates = it -> second.get_child("Dates");
        ptree childValues = it -> second.get_child("Values");

        std::vector<boost::gregorian::date> dates;
        for (const auto& jt: childDates)
        {
            const auto date = jt.second.get_value<std::string>();
            dates.push_back(boost::gregorian::from_string(date));
        }

        std::vector<double> values;
        for (const auto& jt: childValues)
        {
            values.push_back(jt.second.get_value<double>());
        }

        Common::TimeSeries ts(variableName, values, dates);
        m_ds.addData(ts);
    }
}

void Common::JSONParserDecoratorDataSet::writeJSON(const std::string &fileName, const Common::DataSet &ds) const
{
    using namespace boost::property_tree;
    ptree root;
    std::vector<boost::property_tree::ptree> v;

    for (const auto& it: ds.getData())
    {
        ptree child;

        ptree dates;
        for (const auto& jt: it.second.getDates())
        {
            ptree node;
            node.put_value(boost::gregorian::to_iso_extended_string(jt)); //change formatting
            dates.push_back(std::make_pair("", node));
        }
        child.add_child("Dates", dates);

        ptree values;
        for (const auto& jt: it.second.getValues())
        {
            ptree node;
            node.put_value(jt);
            values.push_back(std::make_pair("", node));
        }
        child.add_child("Values", values);
        root.add_child(it.first, child);
    }

    m_parser.writeJSON(fileName, root);
}
