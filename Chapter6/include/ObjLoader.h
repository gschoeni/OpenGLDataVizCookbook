
#pragma once

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "common.h"

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

class CObjLoader
{
public:
    CObjLoader();
    virtual ~CObjLoader ();

    int LoadAsset(const char* a_path);
    void SetScale(float a_scale);
    size_t GetNumVertices();
    void Draw(const GLenum a_drawMode);
    void LoadVertices(GLfloat *a_vertexBufferData);

private:
    const struct aiScene* m_scene;
    GLuint m_sceneList;
    aiVector3D m_sceneMin, m_sceneMax, m_sceneCenter;
    float m_scale;
    size_t m_numVertices;
    size_t p_RecursiveDrawing(const struct aiNode* a_node, size_t a_vertexCount, const GLenum);
    size_t p_RecursiveVertexLoading(const struct aiNode* a_node, GLfloat *a_vertexBufferData, size_t a_vertexCounter);
    size_t p_RecursiveGetNumVertices(const struct aiNode* a_node);
    void p_GetBoundingBox(aiVector3D* a_min, aiVector3D* a_max);
    void p_GetBoundingBoxForNode(const struct aiNode* a_node, aiVector3D* a_min, aiVector3D* a_max, aiMatrix4x4* a_trafo);

};
