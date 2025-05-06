#include <cstdint>
#include <string_view>
#include <vector>

class MaskedColumnCache {
    const std::vector<double> *fullData = nullptr;
    const std::vector<uint8_t> *mask = nullptr;

    std::vector<double> filteredData;
    size_t cachedMaskHash = 0;

  public:
    void setSource(const std::vector<double> &data,
                   const std::vector<uint8_t> &mask_) {
        fullData = &data;
        mask = &mask_;
        cachedMaskHash = 0;
    }

    void setSource(const std::vector<double> *data,
                   const std::vector<uint8_t> *mask_) {
        fullData = data;
        mask = mask_;
        cachedMaskHash = 0;
    }

    const std::vector<double> *getSource() const { return fullData; }

    const std::vector<double> &get() {
        static const std::vector<double> empty;

        if (!fullData || !mask)
            return empty;

        size_t currentHash = std::hash<std::string_view>{}(std::string_view(
            reinterpret_cast<const char *>(mask->data()), mask->size()));

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
            if ((*mask)[i]) {
                filteredData.push_back((*fullData)[i]);
            }
        }
    }
};
