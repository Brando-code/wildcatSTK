//
// Created by Alberto Campi on 23/07/2019.
//

#include "FactoryMappings.h"
#include "../Symbols/AlgebraicOperatorSymbols.h"
#include "../../Common/Config/ConfigVariable.h"
#include "../../Common/Math/Relative/RelativeModel.h"
#include "../../Common/Math/Interpolation/Interpolator.h"
#include "../../Common/Utils/General/AlgebraicExpressionInterpreter.h"
#include "../../Common/Seasonality/SeasonalDecompose.h"
#include "../../Common/Auxiliary/FormulaVariable.h"

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

const Common::TransformationTypeFactory* TransformationTypeCodeFactoryMapping::getFactory(const std::string &transformationTypeCode) const
{
    if (m_mapping.find(transformationTypeCode) != m_mapping.end())
        return m_mapping.at(transformationTypeCode);
    else
        throw std::out_of_range("E: Global::TransformationTypeFactory::getFactory : unknown transformation code " +
                                transformationTypeCode);
}


RelativeModelFactoryMapping* RelativeModelFactoryMapping::m_instance = nullptr;

RelativeModelFactoryMapping::RelativeModelFactoryMapping()
{
    const std::vector<std::string> allowedRelativeSubTypeNames = {"growth", "volatility"};
    m_mapping.emplace(allowedRelativeSubTypeNames[0], new Math::RelativeGrowthModelFactory());
    m_mapping.emplace(allowedRelativeSubTypeNames[1], new Math::RelativeVolatilityModelFactory());
}

RelativeModelFactoryMapping* RelativeModelFactoryMapping::instance()
{
    if (!m_instance)
        m_instance = new RelativeModelFactoryMapping();

    return m_instance;
}

const Math::RelativeModelFactory* RelativeModelFactoryMapping::getFactory(const std::string &relativeModelSubTypeName) const
{
    if (m_mapping.find(relativeModelSubTypeName) != m_mapping.end())
        return m_mapping.at(relativeModelSubTypeName);
    else
        throw std::out_of_range("E: Global::RelativeModelFactoryMapping::getFactory : unknown model sub-type name " +
                                relativeModelSubTypeName);
}


InterpolatorFactoryMapping* InterpolatorFactoryMapping::m_instance = nullptr;

InterpolatorFactoryMapping::InterpolatorFactoryMapping()
{
    const std::vector<std::string> allowedInterpolationMethods = {"linear", "cubic"};
    m_mapping.emplace(allowedInterpolationMethods[0], new Math::LinearInterpolatorFactory());
    m_mapping.emplace(allowedInterpolationMethods[1], new Math::NaturalCubicSplineInterpolatorFactory());
}

InterpolatorFactoryMapping* InterpolatorFactoryMapping::instance()
{
    if (!m_instance)
        m_instance = new InterpolatorFactoryMapping();

    return m_instance;
}

const Math::InterpolatorFactory* InterpolatorFactoryMapping::getFactory(const std::string &interpolationMethodName) const
{
    if (m_mapping.find(interpolationMethodName) != m_mapping.end())
        return m_mapping.at(interpolationMethodName);
    else
        throw std::out_of_range("E: Global::InterpolatorFactoryMapping::getFactory : unknown interpolation method name " +
        interpolationMethodName);
}


AlgebraicExpressionFactoryMapping* AlgebraicExpressionFactoryMapping::m_instance = nullptr;

AlgebraicExpressionFactoryMapping::AlgebraicExpressionFactoryMapping()
{
    AlgebraicOperatorSymbols* ptr = AlgebraicOperatorSymbols::instance();
    m_mapping.emplace(ptr -> getAdditionSymbol(), new Common::AdditionExpressionFactory());
    m_mapping.emplace(ptr -> getSubstractionSymbol(), new Common::SubstractionExpressionFactory());
    m_mapping.emplace(ptr -> getMultiplicationSymbol(), new Common::MultiplicationExpressionFactory());
    m_mapping.emplace(ptr -> getDivisionSymbol(), new Common::DivisionExpressionFactory());
    m_mapping.emplace(ptr -> getExponentiationSymbol(), new Common::ExponentiationExpressionFactory());
}

AlgebraicExpressionFactoryMapping* AlgebraicExpressionFactoryMapping::instance()
{
    if (!m_instance)
        m_instance = new AlgebraicExpressionFactoryMapping();

    return m_instance;
}

const Common::BinaryOperatorExpressionFactory* AlgebraicExpressionFactoryMapping::getFactory(const std::string &symbol) const
{
    if (m_mapping.find(symbol) != m_mapping.end())
        return m_mapping.at(symbol);
    else
        throw std::out_of_range("E: Global::AlgebraicExpressionFactoryMapping::getFactory : unknown algebraic symbol " +
                                        symbol);
}

SeasonalDecomposeFactoryMapping* SeasonalDecomposeFactoryMapping::g_instance = nullptr;

SeasonalDecomposeFactoryMapping::SeasonalDecomposeFactoryMapping()
{
    const std::vector<std::string> allowedDecompositionTypes = {"additive", "multiplicative"};
    m_mapping.emplace(allowedDecompositionTypes[0], new Common::SeasonalDecomposeConvolutionAdditiveFactory());
    m_mapping.emplace(allowedDecompositionTypes[1], new Common::SeasonalDecomposeConvolutionMultiplicativeFactory());
}

SeasonalDecomposeFactoryMapping* SeasonalDecomposeFactoryMapping::instance()
{
    if (!g_instance)
        g_instance = new SeasonalDecomposeFactoryMapping();

    return g_instance;
}

const Common::SeasonalDecomposeFactory* SeasonalDecomposeFactoryMapping::getFactory(const std::string &decompositionType) const
{
    if (m_mapping.find(decompositionType) != m_mapping.end())
        return m_mapping.at(decompositionType);
    else
        throw std::out_of_range("Global::SeasonalDecomposeFactoryMapping::getFactory : unknown seasonal decomposition type " +
        decompositionType);
}

RestoreSeasonFactoryMapping* RestoreSeasonFactoryMapping::g_instance = nullptr;

RestoreSeasonFactoryMapping::RestoreSeasonFactoryMapping()
{
    const std::vector<std::string> allowedDecompositionTypes = {"additive", "multiplicative"};
    m_mapping.emplace(allowedDecompositionTypes[0], new Common::RestoreSeasonAdditiveFactory());
    m_mapping.emplace(allowedDecompositionTypes[1], new Common::RestoreSeasonMultiplicativeFactory());
}

RestoreSeasonFactoryMapping* RestoreSeasonFactoryMapping::instance()
{
    if (!g_instance)
        g_instance = new RestoreSeasonFactoryMapping();

    return g_instance;
}

const Common::RestoreSeasonFactory* RestoreSeasonFactoryMapping::getFactory(const std::string &decompositionType) const
{
    if (m_mapping.find(decompositionType) != m_mapping.end())
        return m_mapping.at(decompositionType);
    else
        throw std::out_of_range("Global::RestoreSeasonFactoryMapping::getFactory : unknown seasonal decomposition type " +
                                decompositionType);
}