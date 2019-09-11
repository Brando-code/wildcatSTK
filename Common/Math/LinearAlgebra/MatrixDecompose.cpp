//
// Created by Alberto Campi on 2019-09-08.
//

#include "MatrixDecompose.h"

Math::CholeskyDecompose::CholeskyDecompose() : m_L(), m_isPositiveDef(true)
{

}

void Math::CholeskyDecompose::decompose(const boost::numeric::ublas::matrix<double> &M)
{
    const long dim = M.size1();
    boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> L(dim, dim);

    for (unsigned long i = 0; i < dim ; ++i)
    {
        for (unsigned long k = 0; k < i + 1; ++k)
        {
            double sum = 0;
            for (unsigned long j = 0; j < k; ++j)
            {
                sum += L(i, j) * L(k, j);
            }

            if (i == k and M(i, i) - sum <= 0)
            {
                std::cerr << "Math::CholeskyDecompose::decompose : "
                             "algorithm failed to complete due to matrix non-positive-definiteness" << std::endl;
                m_isPositiveDef = false;
                break;
            }
            else
                L(i, k) = (i == k) ? sqrt(M(i, i) - sum) :
                                    ((1. / L(k, k)) * (M(i, k) - sum));
        }
    }
    m_L = L;
}

boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> Math::CholeskyDecompose::getCholeskyFactor() const
{
    return m_L;
}

bool Math::CholeskyDecompose::hasFailed() const
{
    return !m_isPositiveDef;
}