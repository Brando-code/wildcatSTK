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
#endif //WILDCATSTKCORE_CONCEPTS_H
