#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <memory>
#include <vector>

template <typename T> class Repository {
  public:
    Repository() = default;
    virtual ~Repository() = default;

    virtual size_t add(const T &item) = 0;
    virtual void remove(const size_t id) = 0;
    virtual std::vector<std::shared_ptr<T>> getAll() = 0;
};

#endif // REPOSITORY_HPP