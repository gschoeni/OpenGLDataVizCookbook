#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "common/common.h"

GLuint InitTexture(const unsigned char* a_imageData, int a_width, int a_height, GLenum a_format);
void UpdateTexture(const unsigned char* a_imageData, int a_width, int a_height, GLenum a_format);
GLuint LoadImageToTexture(const char* a_imagePath, int* a_width, int* a_height, int* a_channels);

#endif
