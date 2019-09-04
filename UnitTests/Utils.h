//
// Created by Alberto Campi on 2019-08-11.
//

#ifndef WILDCATSTKCORE_UTILS_H
#define WILDCATSTKCORE_UTILS_H

#include <map>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>

//Utility functions to support BOOST unit testing framework

template<class T, class D>
std::istream &readMap(std::map<T, D> &outputMap, std::istream &rStream, bool header)
{
    if (header)
    {
        std::string headerLine;
        std::getline(rStream, headerLine);
    }

    outputMap.clear();
    while (true)
    {
        T tempKey;
        D tempValue;
        rStream >> tempKey >> tempValue;
        outputMap.insert(std::make_pair(tempKey, tempValue));

        if (rStream.eof())
            break;
    }

    return rStream;
}

template <typename T>
std::istream &readArray(std::vector<T>& outputArray, std::istream& rStream, bool header)
{
    if (header)
    {
        std::string headerLine;
        std::getline(rStream, headerLine);
    }

    outputArray.clear();
    while (true)
    {
        T tempValue;
        rStream >> tempValue;
        outputArray.push_back(tempValue);

        if (rStream.eof() or rStream.fail())
            break;
    }

    return rStream;
}
/*
template<class T>
std::ostream &writeArray(const std::vector<T> &inputArray, std::ostream &wStream, int precision, bool header)
{
    if (header)
        wStream << "Value" << std::endl;

    for (auto it = inputArray.begin(); it != inputArray.end(); ++it)
    {
        wStream << std::fixed << std::setprecision(precision) << *it;

        auto next = it + 1;
        if (next != inputArray.end())
            wStream << std::endl;
    }

    return wStream;
}

template<class T, class D>
std::ostream &writeMap(const std::map<T, D> &inputMap, std::ostream &wStream, int precision, bool header)
{
    if (header)
        wStream << "Key" << "\t" << "Value" << std::endl;

    for (auto it = inputMap.begin(); it != inputMap.end(); ++it)
    {
        wStream << std::fixed << std::setprecision(precision) << it -> first <<
                "\t" << std::fixed << std::setprecision(precision) << it -> second;

        auto next = it + 1;
        if (next != inputMap.end())
            wStream << std::endl;
    }

    return wStream;
}
*/
namespace boost
{
    //Tell BOOST.TEST how to send std::pair<K, V> to boost::wrap_stringstream
    template <typename K, typename V>
    inline wrap_stringstream&
    operator<<(wrap_stringstream& wrapped, const std::pair<const K, V>& item)
    {
        return wrapped << '<' << item.first << ',' << item.second << '>';
    }

    namespace test_tools
    {
        //Tell BOOST.TEST how to print std::pair<K, V> in collection assert macros
        template<typename K, typename V>
        struct tt_detail::print_log_value<std::pair<const K, V>>
        {
            void operator()(std::ostream& ostr, const std::pair<const K, V>& item)
            {
                ostr << '<' << item.first << ',' << item.second << '>';
            }
        };
    }
}

#endif //WILDCATSTKCORE_UTILS_H
