//
// Created by Alberto Campi on 2019-08-06.
//

#ifndef WILDCATSTKCORE_CONCEPTS_H
#define WILDCATSTKCORE_CONCEPTS_H

namespace Common
{
    class Uncopyable
    {
    protected:
        Uncopyable() = default;
        ~Uncopyable() = default;

    private:
        Uncopyable(const Common::Uncopyable &other);
        Uncopyable &operator=(const Common::Uncopyable &other);
    };
}

namespace Math
{
    template <typename T, typename D>
    class Functor
    {
    public:
        virtual T operator()(const D& arg) const = 0;

        virtual ~Functor() = default;
    };
}
#endif //WILDCATSTKCORE_CONCEPTS_H
