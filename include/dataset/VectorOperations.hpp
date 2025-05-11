#ifndef VECTOR_OPERATIONS_HPP
#define VECTOR_OPERATIONS_HPP

#include "dataset/BitVector.hpp"
#include <cmath>
#include <stdexcept>
#include <sys/stat.h>
#include <vector>

class VectorOperations {
  public:
    static std::vector<double> add(const std::vector<double> &a,
                                   const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in add()");

        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = a[i] + b[i];
        return result;
    }

    static std::vector<double> subtract(const std::vector<double> &a,
                                        const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in subtract()");

        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = a[i] - b[i];
        return result;
    }

    static std::vector<double> multiply(const std::vector<double> &a,
                                        const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in multiply()");

        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = a[i] * b[i];
        return result;
    }

    static std::vector<double> multiply(const std::vector<double> &a,
                                        double b) {
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = a[i] * b;
        return result;
    }

    static std::vector<double> divide(const std::vector<double> &a,
                                      const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in divide()");

        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = a[i] / b[i];
        return result;
    }

    static std::vector<double> power(const std::vector<double> &a,
                                     const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in power()");

        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = std::pow(a[i], b[i]);
        return result;
    }

    static std::vector<double> sin(const std::vector<double> &a) {
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = std::sin(a[i]);
        return result;
    }

    static std::vector<double> cos(const std::vector<double> &a) {
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = std::cos(a[i]);
        return result;
    }

    static std::vector<double> tan(const std::vector<double> &a) {
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = std::tan(a[i]);
        return result;
    }

    static std::vector<double> sqrt(const std::vector<double> &a) {
        std::vector<double> result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result[i] = std::sqrt(a[i]);
        return result;
    }

    static BitVector approx(const std::vector<double> &a,
                            const std::vector<double> &b,
                            const std::vector<double> &c) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in approx()");

        BitVector result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result.set(i, std::abs(a[i] - b[i]) < c[i]);
        return result;
    }

    static BitVector greater(const std::vector<double> &a,
                             const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in greater()");

        BitVector result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result.set(i, a[i] > b[i]);
        return result;
    }

    static BitVector less(const std::vector<double> &a,
                          const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in less()");

        BitVector result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result.set(i, a[i] < b[i]);
        return result;
    }

    static BitVector equal(const std::vector<double> &a,
                           const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in equal()");

        BitVector result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result.set(i, a[i] == b[i]);
        return result;
    }

    static BitVector notEqual(const std::vector<double> &a,
                              const std::vector<double> &b) {
        if (a.size() != b.size())
            throw std::runtime_error("Mismatched vector sizes in notEqual()");

        BitVector result(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            result.set(i, a[i] != b[i]);
        return result;
    }

    static BitVector andOp(const BitVector &a, const BitVector &b) {
        if (a.bitSize() != b.bitSize())
            throw std::runtime_error("Mismatched bit vector sizes in andOp()");

        BitVector result(a.bitSize());
        for (size_t i = 0; i < a.bitSize(); ++i)
            result.set(i, a.get(i) && b.get(i));
        return result;
    }

    static BitVector orOp(const BitVector &a, const BitVector &b) {
        if (a.bitSize() != b.bitSize())
            throw std::runtime_error("Mismatched bit vector sizes in orOp()");

        BitVector result(a.bitSize());
        for (size_t i = 0; i < a.bitSize(); ++i)
            result.set(i, a.get(i) || b.get(i));
        return result;
    }

    static BitVector notOp(const BitVector &a) {
        BitVector result(a.bitSize());
        for (size_t i = 0; i < a.bitSize(); ++i)
            result.set(i, !a.get(i));
        return result;
    }

    static std::vector<double> pi(size_t size) {
        return std::vector<double>(size, 3.14159265358979323846);
    }

    static std::vector<double> e(size_t size) {
        return std::vector<double>(size, 2.71828182845904523536);
    }
};

#endif // VECTOR_OPERATIONS_HPP
