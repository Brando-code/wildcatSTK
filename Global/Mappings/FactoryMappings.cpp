//
// Created by Alberto Campi on 23/07/2019.
//

#include "FactoryMappings.h"

using namespace Global;

TransformationTypeCodeFactoryMapping* TransformationTypeCodeFactoryMapping::m_instance = nullptr;

TransformationTypeCodeFactoryMapping::TransformationTypeCodeFactoryMapping()
{
    const std::vector<std::string> allowedTransformationCodes = {"D", "R", "L", "LR"};
    m_mapping.at(allowedTransformationCodes[0]) = Common::FirstDifferenceTransformationFactory();
    m_mapping.at(allowedTransformationCodes[1]) = Common::SimpleReturnTransformationFactory();
    m_mapping.at(allowedTransformationCodes[2]) = Common::LevelTransformationFactory();
    m_mapping.at(allowedTransformationCodes[3]) = Common::LogReturnTransformationFactory();
}

TransformationTypeCodeFactoryMapping* TransformationTypeCodeFactoryMapping::instance()
{
    if (!m_instance)
        m_instance = new TransformationTypeCodeFactoryMapping();

    return m_instance;
}

std::map<std::string, Common::TransformationTypeFactory> TransformationTypeCodeFactoryMapping::getMapping() const
{
    return m_mapping;
}