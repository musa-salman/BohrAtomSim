#include "plotter/TextureLoader.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

GLuint TextureLoader::loadTexture(const char *filename) {
    // int width;
    // int height;
    // int channels;
    // // unsigned char *data = stbi_load(filename, &width, &height, &channels,
    // 4);

    // if (!data) {
    //     std::cerr << "Failed to load image: " << filename << std::endl;
    //     // data =
    //         // stbi_load(std::format("{}/plots/placholder.png",
    //         DB_PATH).c_str(),
    //         //           &width, &height, &channels, 4);
    // }

    // GLuint textureID;
    // glGenTextures(1, &textureID);
    // glBindTexture(GL_TEXTURE_2D, textureID);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
    //              GL_UNSIGNED_BYTE, data);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // stbi_image_free(data);
    return 9;
}
