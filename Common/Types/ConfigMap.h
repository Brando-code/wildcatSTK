//
// Created by Alberto Campi on 2019-08-16.
//

#ifndef WILDCATSTKCORE_CONFIGMAP_H
#define WILDCATSTKCORE_CONFIGMAP_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>


namespace Common
{
    template <typename T>
    class ConfigMap
    {
    public:
        ConfigMap() = default;

        ConfigMap(const ConfigMap<T>& other)
        {
            for (const auto& it: other.m_configMap)
                m_configMap.emplace(it.first, it.second -> clone());
        }

        ConfigMap& operator=(const ConfigMap<T>& other)
        {
            if (&other != this)
            {
                m_configMap.clear();
                for (const auto& it: other.m_configMap)
                    m_configMap.emplace(it.first, it.second -> clone());
            }
            return *this;
        }

        ConfigMap(ConfigMap<T>&& other) : m_configMap(std::move(other.m_configMap))
        {

        }

        ConfigMap& operator=(ConfigMap<T>&& other)
        {
            if (&other != this)
            {
                m_configMap.clear();
                m_configMap = std::move(other.m_configMap);
            }
            return *this;
        }

        void addConfigItem(const std::string &key, const T &value)
        {
            if (m_configMap.find(key) == m_configMap.end())
                m_configMap.emplace(key, value.clone());
            else
            {
                std::cerr << "W: Common::ConfigModelSpecTable::addModelSpec : existing specification for model " << key
                          << " is being replaced."
                          << std::endl;
                m_configMap.at(key) = value.clone();
            }
        }

        void removeConfigItem(const std::string &key)
        {
            m_configMap.erase(key);
        }

        void clearAllConfigItems()
        {
            m_configMap.clear();
        }

        std::unordered_map<std::string, std::unique_ptr<T>> getConfigMap() const
        {
            Common::ConfigMap<T> temp(*this);
            return std::move(temp.m_configMap);
        }

        std::unique_ptr<T> getValue(const std::string &key) const
        {
            return m_configMap.at(key) -> clone();
        }

        bool operator==(const Common::ConfigMap<T>& other) const
        {
            bool rv = true;
            if (other.m_configMap.size() == m_configMap.size())
            {
                for (const auto &it: other.m_configMap)
                {
                    if (m_configMap.find(it.first) != m_configMap.end() and
                    *m_configMap.at(it.first) == *other.m_configMap.at(it.first))
                        continue;
                    else
                    {
                        rv = false;
                        break;
                    }
                }
            }
            else
                rv = false;

            return rv;
        }

        bool operator!=(const Common::ConfigMap<T>& other) const
        {
            return !(*this == other);
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<T>> m_configMap;
    };
}

#endif //WILDCATSTKCORE_CONFIGMAP_H
