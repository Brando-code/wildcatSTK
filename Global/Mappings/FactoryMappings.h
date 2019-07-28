//
// Created by Alberto Campi on 23/07/2019.
//

#ifndef WILDCATSTKCORE_FACTORYMAPPINGS_H
#define WILDCATSTKCORE_FACTORYMAPPINGS_H


#include <string>
#include <map>
#include "../Global.h"
#include "../../Common/Config/ConfigVariable.h"
#include "../../Common/Math/Relative/RelativeModel.h"

class Global::TransformationTypeCodeFactoryMapping
{
public:
    static TransformationTypeCodeFactoryMapping* instance();
    std::map<std::string, Common::TransformationTypeFactory *> getMapping() const;
    Common::TransformationTypeFactory* getFactory(const std::string& transformationTypeCode) const;

private:
    TransformationTypeCodeFactoryMapping();
    static TransformationTypeCodeFactoryMapping* m_instance;
    std::map<std::string, Common::TransformationTypeFactory*> m_mapping;
};

class Global::RelativeModelFactoryMapping
{
public:
    static RelativeModelFactoryMapping* instance();
    std::map<std::string, Math::RelativeModelFactory *> getMapping() const;
    Math::RelativeModelFactory* getFactory(const std::string& relativeModelSubTypeName) const;

private:
    RelativeModelFactoryMapping();
    static RelativeModelFactoryMapping* m_instance;
    std::map<std::string, Math::RelativeModelFactory*> m_mapping;
};

#endif //WILDCATSTKCORE_FACTORYMAPPINGS_H
