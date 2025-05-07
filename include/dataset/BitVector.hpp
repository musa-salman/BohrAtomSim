#ifndef BITVECTOR_HPP
#define BITVECTOR_HPP

#include <cstdint>
#include <vector>

class BitVector {
  public:
    explicit BitVector(size_t bitCount);

    [[nodiscard]] bool get(size_t pos) const;

    void set(size_t pos, bool value);

    void clear();

    [[nodiscard]] size_t bitSize() const;

    [[nodiscard]] const std::vector<uint64_t> &raw() const;

  private:
    size_t size;
    std::vector<uint64_t> words;
};

#endif // BITVECTOR_HPP