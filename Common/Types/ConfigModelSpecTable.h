//
// Created by Alberto Campi on 2019-08-06.
//

#ifndef WILDCATSTKCORE_CONFIGMODELSPECTABLE_H
#define WILDCATSTKCORE_CONFIGMODELSPECTABLE_H

#include "../Config/ConfigModelSpec.h"
#include <vector>
#include <unordered_map>

namespace Common
{
    class ConfigModelSpecTable
    {
    public:
        ConfigModelSpecTable() = default;
        explicit ConfigModelSpecTable(const std::vector<std::unique_ptr<Common::ConfigModelSpec>> &modelSpecs);
        ConfigModelSpecTable(const ConfigModelSpecTable& other);
        ConfigModelSpecTable& operator=(const ConfigModelSpecTable& other);
        ConfigModelSpecTable(ConfigModelSpecTable&& other);
        ConfigModelSpecTable& operator=(ConfigModelSpecTable&& other);

        void addModelSpec(const Common::ConfigModelSpec &modelSpec);
        void removeData(const std::string &variableName);
        void clearAllData();

        std::unordered_map<std::string, std::unique_ptr<Common::ConfigModelSpec>> getConfigModelSpecTable() const;
        std::unique_ptr<Common::ConfigModelSpec> getConfigModelSpec(const std::string &variableName) const;
        
        bool operator==(const Common::ConfigModelSpecTable& other) const;
        bool operator!=(const Common::ConfigModelSpecTable& other) const;

    private:
        std::unordered_map<std::string, std::unique_ptr<Common::ConfigModelSpec>> m_configModelTable;
    };
}

#endif //WILDCATSTKCORE_CONFIGMODELSPECTABLE_H
