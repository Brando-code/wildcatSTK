//
// Created by Alberto Campi on 23/07/2019.
//

#ifndef WILDCATSTKCORE_FACTORYMAPPINGS_H
#define WILDCATSTKCORE_FACTORYMAPPINGS_H


#include <string>
#include <map>
#include "../Global.h"
#include "../../Common/Types/ConfigVariable.h"

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


#endif //WILDCATSTKCORE_FACTORYMAPPINGS_H
