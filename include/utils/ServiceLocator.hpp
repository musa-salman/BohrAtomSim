#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace utils {

class ServiceLocator {
    std::unordered_map<std::type_index, std::shared_ptr<void>> services;

  public:
    template <typename T> void registerService(std::shared_ptr<T> service) {
        services[typeid(T)] = service;
    }

    template <typename T> constexpr inline T &get() noexcept {
        return *static_cast<T *>(services.at(typeid(T)).get());
    }

    static ServiceLocator &getInstance() {
        static ServiceLocator instance;
        return instance;
    }

  private:
    ServiceLocator() = default;
};
} // namespace utils
