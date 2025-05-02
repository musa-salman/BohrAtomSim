#ifndef SERVICE_LOCATOR_HPP
#define SERVICE_LOCATOR_HPP

#include <memory>
#include <sys/stat.h>
#include <typeindex>
#include <unordered_map>

class ServiceLocator {
    std::unordered_map<std::type_index, std::shared_ptr<void>> services;

  public:
    template <typename T> void registerService(std::shared_ptr<T> service) {
        services[typeid(T)] = service;
    }

    template <typename T> std::shared_ptr<T> get() {
        auto it = services.find(typeid(T));
        if (it != services.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    static ServiceLocator &getInstance() {
        static ServiceLocator instance;
        return instance;
    }

  private:
    ServiceLocator() = default;
};

#endif // SERVICE_LOCATOR_HPP
