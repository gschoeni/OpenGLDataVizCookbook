
#include "ObjLoader.h"

CObjLoader::CObjLoader()
{
    m_scale = 1.0f;
    m_scene = NULL;
    m_numVertices = 0;
}

CObjLoader::~CObjLoader()
{

}

size_t CObjLoader::GetNumVertices()
{
    return m_numVertices;
}

int CObjLoader::LoadAsset(const char* a_path)
{
    m_scene = aiImportFile(a_path, aiProcessPreset_TargetRealtime_MaxQuality);
    if (m_scene)
    {
        p_GetBoundingBox(&m_sceneMin, &m_sceneMax);
        m_sceneCenter.x = (m_sceneMin.x + m_sceneMax.x) / 2.0f;
        m_sceneCenter.y = (m_sceneMin.y + m_sceneMax.y) / 2.0f;
        m_sceneCenter.z = (m_sceneMin.z + m_sceneMax.z) / 2.0f;
        printf("Loaded file %s\n", a_path);
        m_scale = 4.0 / (m_sceneMax.x - m_sceneMin.x);

        printf("OBJ scale %s\n", a_path);
        m_numVertices = p_RecursiveGetNumVertices(m_scene->mRootNode);
        printf("This scene has %zu vertices\n", m_numVertices);
        return 0;
    }
    return 1;
}

void CObjLoader::LoadVertices(GLfloat *a_vertexBufferData)
{
    p_RecursiveVertexLoading(m_scene->mRootNode, a_vertexBufferData, 0);
}

size_t CObjLoader::p_RecursiveVertexLoading(
    const struct aiNode* a_node,
    GLfloat *a_vertexBufferData,
    size_t a_vertexCounter)
{
    // save all data to the vertex array, perform offset and scaling to reduce the computation
    for (size_t n = 0; n < a_node->mNumMeshes; ++n)
    {
        const struct aiMesh* l_mesh = m_scene->mMeshes[a_node->mMeshes[n]];
        for (size_t f = 0; f < l_mesh->mNumFaces; ++f)
        {
            const struct aiFace* l_face = &l_mesh->mFaces[f];
            for (size_t i = 0; i < l_face->mNumIndices; i++)
            {
                int l_index = l_face->mIndices[i];
                a_vertexBufferData[a_vertexCounter]   = (l_mesh->mVertices[l_index].x-m_sceneCenter.x) * m_scale;
                a_vertexBufferData[a_vertexCounter+1] = (l_mesh->mVertices[l_index].y-m_sceneCenter.y) * m_scale;
                a_vertexBufferData[a_vertexCounter+2] = (l_mesh->mVertices[l_index].z-m_sceneCenter.z) * m_scale;
                a_vertexCounter += 3;
            }
        }
    }

    // traverse all children nodes
    for (size_t n = 0; n < a_node->mNumChildren; ++n)
    {
        a_vertexCounter = p_RecursiveVertexLoading(a_node->mChildren[n], a_vertexBufferData, a_vertexCounter);
    }
    return a_vertexCounter;
}

void CObjLoader::Draw(const GLenum a_drawMode)
{
    p_RecursiveDrawing(m_scene->mRootNode, 0, a_drawMode);
}

size_t CObjLoader::p_RecursiveDrawing(
    const struct aiNode* a_node,
    size_t a_vertexCount,
    const GLenum a_drawMode)
{
    size_t l_totalCount = a_vertexCount;
    for (size_t n = 0; n < a_node->mNumMeshes; ++n)
    {
        size_t l_count = 0;
        const struct aiMesh* l_mesh = m_scene->mMeshes[a_node->mMeshes[n]];
        for (size_t f = 0; f < l_mesh->mNumFaces; ++f)
        {
            const struct aiFace* l_face = &l_mesh->mFaces[f];
            l_count += (3 * l_face->mNumIndices);
        }
        glDrawArrays(a_drawMode, l_totalCount, l_count);
        l_totalCount += l_count;
    }

    a_vertexCount = l_totalCount;
    for (size_t n = 0; n < a_node->mNumChildren; ++n)
    {
        a_vertexCount = p_RecursiveDrawing(a_node->mChildren[n], a_vertexCount, a_drawMode);
    }
    return a_vertexCount;
}

size_t CObjLoader::p_RecursiveGetNumVertices(const struct aiNode* a_node)
{
    size_t l_counter = 0;

    for (size_t n = 0; n < a_node->mNumMeshes; ++n)
    {
        const struct aiMesh* l_mesh = m_scene->mMeshes[a_node->mMeshes[n]];
        for (size_t f = 0; f < l_mesh->mNumFaces; ++f)
        {
            const struct aiFace* l_face = &l_mesh->mFaces[f];
            l_counter += (3 * l_face->mNumIndices);
        }
        printf("p_RecursiveGetNumVertices: mNumFaces 	%d\n", l_mesh->mNumFaces);
    }

    // traverse the children
    for (size_t n = 0; n < a_node->mNumChildren; ++n)
    {
        l_counter += p_RecursiveGetNumVertices(a_node->mChildren[n]);
    }
    printf("p_RecursiveGetNumVertices: l_counter 	%zu\n", l_counter);
    return l_counter;
}

void CObjLoader::p_GetBoundingBox(aiVector3D* a_min, aiVector3D* a_max)
{
    aiMatrix4x4 l_trafo;
    aiIdentityMatrix4(&l_trafo);
    a_min->x = a_min->y = a_min->z =  1e10f;
    a_max->x = a_max->y = a_max->z = -1e10f;
    p_GetBoundingBoxForNode(m_scene->mRootNode, a_min, a_max, &l_trafo);
}

void CObjLoader::p_GetBoundingBoxForNode(
    const struct aiNode* a_node,
    aiVector3D* a_min, aiVector3D* a_max,
    aiMatrix4x4* a_trafo)
{
    aiMatrix4x4 l_prev = *a_trafo;
    aiMultiplyMatrix4(a_trafo, &a_node->mTransformation);

    for (size_t n = 0; n < a_node->mNumMeshes; ++n)
    {
        const struct aiMesh* l_mesh = m_scene->mMeshes[a_node->mMeshes[n]];
        for (size_t v = 0; v < l_mesh->mNumVertices; ++v)
        {
            aiVector3D l_tmp = l_mesh->mVertices[v];
            aiTransformVecByMatrix4(&l_tmp, a_trafo);
            a_min->x = aisgl_min(a_min->x, l_tmp.x);
            a_min->y = aisgl_min(a_min->y, l_tmp.y);
            a_min->z = aisgl_min(a_min->z, l_tmp.z);
            a_max->x = aisgl_max(a_max->x, l_tmp.x);
            a_max->y = aisgl_max(a_max->y, l_tmp.y);
            a_max->z = aisgl_max(a_max->z, l_tmp.z);
        }
    }

    for (size_t n = 0; n < a_node->mNumChildren; ++n)
    {
        p_GetBoundingBoxForNode(a_node->mChildren[n], a_min, a_max, a_trafo);
    }
    *a_trafo = l_prev;
}
