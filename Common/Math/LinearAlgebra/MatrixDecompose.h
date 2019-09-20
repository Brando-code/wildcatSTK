//
// Created by Alberto Campi on 2019-09-08.
//

#ifndef WILDCATSTKCORE_MATRIXDECOMPOSE_H
#define WILDCATSTKCORE_MATRIXDECOMPOSE_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <map>

namespace Math
{
    //
    // Class hierarchy abstracting generic matrix decomposition
    //
    class MatrixDecompose
    {
    public:
        virtual void decompose(const boost::numeric::ublas::matrix<double> &M) = 0;
        virtual bool hasFailed() const = 0;

        virtual ~MatrixDecompose() = default;
    };

    class CholeskyDecompose : MatrixDecompose
    {
    public:
        CholeskyDecompose();
        void decompose(const boost::numeric::ublas::matrix<double> &M) override;
        boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> getCholeskyFactor() const;
        bool hasFailed() const final;

    protected:
        boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::lower> m_L;
        bool m_isPositiveDef;
    };

    class CholeskyDecomposeWithPivoting : public CholeskyDecompose
    {
    public:
        void decompose(const boost::numeric::ublas::matrix<double> &M) override;

    private:

    };

    //More decomposition classes to derive from MatrixDecompose..
}



#endif //WILDCATSTKCORE_MATRIXDECOMPOSE_H
