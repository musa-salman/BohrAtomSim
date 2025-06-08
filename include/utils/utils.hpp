#pragma once

#include <filesystem>
#include <string>

namespace utils {

std::string
formatOutputFilename(size_t id,
                     const std::filesystem::path &basePath = DB_PATH);

} // namespace utils
