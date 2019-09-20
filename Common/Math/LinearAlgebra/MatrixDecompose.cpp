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

            // Make sure that no zero or complex element is on cholesky factor diagonal. If so, matrix is singular.
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

void Math::CholeskyDecomposeWithPivoting::decompose(const boost::numeric::ublas::matrix<double> &M)
{
    const unsigned long dim = M.size1();
    boost::numeric::ublas::matrix<double> M_(M);
    const boost::numeric::ublas::triangular_adaptor<boost::numeric::ublas::matrix<double>, boost::numeric::ublas::lower> ad(M_);
    boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> L(ad);

    for (unsigned long j = 0; j < dim; ++j)
    {
        const boost::numeric::ublas::vector<double> row_ = subrange(row(L, j), 0, j);
        double maxInnerProd = inner_prod(row_, trans(row_));

        unsigned long maxIndex = j;
        for (unsigned long k = j; k < dim; ++k)
        {
            const boost::numeric::ublas::vector<double> thisRow_ = subrange(row(L, j), 0, j);
            const double thisInnerProd = inner_prod(thisRow_, trans(thisRow_));
            if (thisInnerProd > maxInnerProd)
            {
                maxInnerProd = thisInnerProd;
                maxIndex = k;
            }
        }
        L(j, j) = L(maxIndex, maxIndex) - maxInnerProd;
        //L(j, j) = L(j, j) - inner_prod(row_, trans(row_));

        if (L(j, j) <= 0)
        {
            subrange(L, j, dim, j, dim) = boost::numeric::ublas::zero_matrix<double>(dim - j, dim - j);

            std::cerr << "Math::CholeskyDecompose::decompose : "
                         "algorithm quit due to matrix non-positive-definiteness." << std::endl;
            //m_isPositiveDef = false;
            break;
        }
        else
        {
            L(j, j) = sqrt(L(j, j));
            if (j < dim - 1)
            {
                const boost::numeric::ublas::matrix<double> Mj_ = subrange(L, j + 1, dim, j, j + 1);
                const boost::numeric::ublas::matrix<double> P_ =
                        prod(subrange(L, j + 1, dim, 0, j), trans(subrange(L, j, j + 1, 0, j)));

                subrange(L, j + 1, dim, j, j + 1) = (Mj_ - P_) / L(j, j) ;
            }
        }
    }
    m_L = L;
}