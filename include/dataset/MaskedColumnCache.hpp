#include <string_view>
#include <vector>

#include "dataset/BitVector.hpp"

class MaskedColumnCache {
    const std::vector<double> *fullData = nullptr;
    const BitVector *mask = nullptr;

    std::vector<double> filteredData;
    size_t cachedMaskHash = 0;

  public:
    void setSource(const std::vector<double> &data, const BitVector &mask_) {
        fullData = &data;
        mask = &mask_;
        cachedMaskHash = 0;
    }

    void setSource(const std::vector<double> *data, const BitVector *mask_) {
        fullData = data;
        mask = mask_;
        cachedMaskHash = 0;
    }

    const std::vector<double> *getSource() const { return fullData; }

    const std::vector<double> &get() {
        static const std::vector<double> empty;

        if (!fullData || !mask)
            return empty;

        size_t currentHash = std::hash<std::string_view>{}(
            std::string_view(reinterpret_cast<const char *>(mask->raw().data()),
                             mask->raw().size()));

        if (currentHash != cachedMaskHash) {
            rebuild();
            cachedMaskHash = currentHash;
        }

        return filteredData;
    }

  private:
    void rebuild() {
        filteredData.clear();
        filteredData.reserve(fullData->size());
        for (size_t i = 0; i < fullData->size(); ++i) {
            if (mask->get(i)) {
                filteredData.push_back((*fullData)[i]);
            }
        }
    }
};
