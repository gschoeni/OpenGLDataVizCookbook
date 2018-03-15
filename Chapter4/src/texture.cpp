
#include "common/texture.hpp"
#include <SOIL.h>

GLuint InitTexture(const unsigned char* a_imageData, int a_width, int a_height, GLenum a_format)
{
    GLuint l_textureId = 0;
    // create and bind one texture element
    glGenTextures(1, &l_textureId);
    glBindTexture(GL_TEXTURE_2D, l_textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Specify the target texture. The params describe the texture format and type of image data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_width, a_height, 0, a_format, GL_UNSIGNED_BYTE, a_imageData);
    // Set the wrap parameter for texture coordinate s & t to GL_CLAMP, which clamps the coordinates within [0, 1]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set the magnification method to linear and return weighted average of four texture elements closest to the center of the pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Choose the mipmap that most closely matches the size of the pixel being textured and use the GL_NEAREST criterion (the texture element nearest to the center of the pixel) to produce a texture value.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    return l_textureId;
}

void UpdateTexture(const unsigned char* a_imageData, int a_width, int a_height, GLenum a_format)
{
    // Update Texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, a_width, a_height, a_format, GL_UNSIGNED_BYTE, a_imageData);
    // Sets the wrap parameter for texture coordinate s & t to GL_CLAMP, which clamps the coordinates within [0, 1].
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set the magnification method to linear and return weighted average of four texture elements closest to the center of the pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Choose the mipmap that most closely matches the size of the pixel being textured and use the GL_NEAREST criterion (the texture element nearest to the center of the pixel) to produce a texture value.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

GLuint LoadImageToTexture(const char* a_imagePath, int* a_width, int* a_height, int* a_channels)
{
    GLuint l_textureID=0;
    // Load the images and convert them to RGBA format
    unsigned char* l_image = SOIL_load_image(a_imagePath, a_width, a_height, a_channels, SOIL_LOAD_RGBA);
    if (!l_image)
    {
        printf("Failed to load image %s\n", a_imagePath);
        return l_textureID;
    }

    printf("Loaded Image: %d x %d - %d channels\n", *a_width, *a_height, *a_channels);
    l_textureID = InitTexture(l_image, *a_width, *a_height, GL_RGBA);
    SOIL_free_image_data(l_image);
    return l_textureID;
}
