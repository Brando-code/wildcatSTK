//
// Created by Alberto Campi on 2019-09-08.
//

#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include "MatrixDecompose.h"

//
// MatrixDecompose concrete classes implementation
//
Math::CholeskyDecompose::CholeskyDecompose() : m_L(), m_isPositiveDef(true)
{

}

//
// Naive Cholesky-Crout decomposition for positive-definite and semi-positive-definite matrix
//
void Math::CholeskyDecompose::decompose(const boost::numeric::ublas::matrix<double> &M)
{
    const long dim = M.size1();
    boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> L(dim, dim);

    for (unsigned long i = 0; i < dim ; ++i)
    {
        for (unsigned long k = i; k < dim; ++k)
        {
            double sum = 0;
            for (unsigned long j = 0; j < i; ++j)
            {
                sum += L(i, j) * L(k, j);
            }

            // Make sure that no zero or complex element is on cholesky factor diagonal. If so, matrix is singular.
            if (i == k and M(i, i) - sum <= 0)
            {
                subrange(L, i, dim, k, k + 1) = boost::numeric::ublas::zero_matrix<double>(dim - i, 1);
                std::cerr << "Math::CholeskyDecompose::decompose : "
                             " matrix non-positive-definiteness detected. Cholesky decomposition may not be unique." << std::endl;
                m_isPositiveDef = false;
                k = dim - 1;
            }
            else
                L(k, i) = (i == k) ? sqrt(M(i, i) - sum) :
                          ((1. / L(i, i)) * (M(k, i) - sum));
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