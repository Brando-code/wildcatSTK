//
// Created by Alberto Campi on 23/07/2019.
//

#include "FactoryMappings.h"

using namespace Global;

TransformationTypeCodeFactoryMapping* TransformationTypeCodeFactoryMapping::m_instance = nullptr;

TransformationTypeCodeFactoryMapping::TransformationTypeCodeFactoryMapping()
{
    const std::vector<std::string> allowedTransformationCodes = {"D", "R", "L", "LR"};
    m_mapping.emplace(allowedTransformationCodes[0], new Common::FirstDifferenceTransformationFactory());
    m_mapping.emplace(allowedTransformationCodes[1], new Common::SimpleReturnTransformationFactory());
    m_mapping.emplace(allowedTransformationCodes[2], new Common::LevelTransformationFactory());
    m_mapping.emplace(allowedTransformationCodes[3], new Common::LogReturnTransformationFactory());
}

TransformationTypeCodeFactoryMapping* TransformationTypeCodeFactoryMapping::instance()
{
    if (!m_instance)
        m_instance = new TransformationTypeCodeFactoryMapping();

    return m_instance;
}

std::map<std::string, Common::TransformationTypeFactory *> TransformationTypeCodeFactoryMapping::getMapping() const
{
    return m_mapping;
}

Common::TransformationTypeFactory* TransformationTypeCodeFactoryMapping::getFactory(const std::string &transformationTypeCode) const
{
    if (m_mapping.find(transformationTypeCode) != m_mapping.end())
        return m_mapping.at(transformationTypeCode);
    else
        throw std::out_of_range("E: Common::TransformationTypeFactory::getFactory : unknown transformation code " +
                                transformationTypeCode);
}