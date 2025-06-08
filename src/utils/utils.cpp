#include "utils/utils.hpp"

#include <format>

namespace utils {
std::string formatOutputFilename(size_t id,
                                 const std::filesystem::path &basePath) {
    return std::format("{}/simulations/{}.bin", basePath.string(), id);
}

} // namespace utils
