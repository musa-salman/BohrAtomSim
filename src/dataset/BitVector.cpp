
#include <stdexcept>

#include "dataset/BitVector.hpp"

BitVector::BitVector(size_t bitCount)
    : size(bitCount), words((bitCount + 63) / 64, 0) {}

bool BitVector::get(size_t pos) const {
    if (pos >= size)
        throw std::out_of_range("BitVector::get");
    return (words[pos / 64] >> (pos % 64)) & 1ULL;
}

void BitVector::set(size_t pos, bool value) {
    if (pos >= size)
        throw std::out_of_range("BitVector::set");
    uint64_t mask = 1ULL << (pos % 64);
    if (value)
        words[pos / 64] |= mask;
    else
        words[pos / 64] &= ~mask;
}

void BitVector::setAll(bool value) {
    uint64_t word = value ? ~0ULL : 0;
    std::fill(words.begin(), words.end(), word);
}

void BitVector::clear() { std::fill(words.begin(), words.end(), 0); }

size_t BitVector::bitSize() const { return size; }

const std::vector<uint64_t> &BitVector::raw() const { return words; }
