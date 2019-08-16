//
// Created by Alberto Campi on 23/07/2019.
//

#ifndef WILDCATSTKCORE_FACTORYMAPPINGS_H
#define WILDCATSTKCORE_FACTORYMAPPINGS_H


#include <string>
#include <map>

namespace Math
{
    class RelativeModelFactory;
    class InterpolatorFactory;
}

namespace Common
{
    class TransformationTypeFactory;
}

namespace Global
{
    class TransformationTypeCodeFactoryMapping
    {
    public:
        static TransformationTypeCodeFactoryMapping *instance();
        std::map<std::string, Common::TransformationTypeFactory *> getMapping() const;
        Common::TransformationTypeFactory *getFactory(const std::string &transformationTypeCode) const;

    private:
        TransformationTypeCodeFactoryMapping();

        static TransformationTypeCodeFactoryMapping *m_instance;
        std::map<std::string, Common::TransformationTypeFactory *> m_mapping;
    };


    class RelativeModelFactoryMapping
    {
    public:
        static RelativeModelFactoryMapping *instance();
        std::map<std::string, Math::RelativeModelFactory *> getMapping() const;
        Math::RelativeModelFactory *getFactory(const std::string &relativeModelSubTypeName) const;

    private:
        RelativeModelFactoryMapping();

        static RelativeModelFactoryMapping *m_instance;
        std::map<std::string, Math::RelativeModelFactory *> m_mapping;
    };


    class InterpolatorFactoryMapping
    {
    public:
        static InterpolatorFactoryMapping* instance();
        std::map<std::string, Math::InterpolatorFactory *> getMapping() const;
        Math::InterpolatorFactory *getFactory(const std::string &interpolationMethodName) const;

    private:
        InterpolatorFactoryMapping();

        static InterpolatorFactoryMapping* m_instance;
        std::map<std::string, Math::InterpolatorFactory *> m_mapping;
    };
}

#endif //WILDCATSTKCORE_FACTORYMAPPINGS_H
