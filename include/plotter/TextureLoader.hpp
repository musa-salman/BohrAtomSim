#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <GL/glew.h>

class TextureLoader {
  public:
    static GLuint loadTexture(const char *filename);
};

#endif // TEXTURE_LOADER_HPP
