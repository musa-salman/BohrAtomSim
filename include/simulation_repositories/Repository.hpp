#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <memory>
#include <vector>

template <typename T> class Repository {
  public:
    Repository() = default;
    virtual ~Repository() = default;

    virtual size_t add(const T &item) const = 0;
    virtual void remove(const size_t id) const = 0;
    virtual std::vector<std::unique_ptr<T>> getAll() const = 0;
};

#endif // REPOSITORY_HPP