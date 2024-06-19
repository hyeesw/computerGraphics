#include "Mesh.h"

void Mesh::gen_gl_buffers()
{
    glGenBuffers(1, &position_buffer_);
    glGenBuffers(1, &color_buffer_);
    glGenBuffers(1, &normal_buffer_);
}

void Mesh::update_tv_indices()
{
    // triangle-vertex indices
    tv_indices_.clear();
    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i)
    {
        aiFace &ai_face = pmesh_->mFaces[i];
        assert(ai_face.mNumIndices >= 3);

        // convert a polygon to a triangle fan
        for (unsigned int idx = 0; idx < ai_face.mNumIndices - 2; ++idx)
        {
            tv_indices_.push_back(ai_face.mIndices[0]);
            tv_indices_.push_back(ai_face.mIndices[idx + 1]);
            tv_indices_.push_back(ai_face.mIndices[idx + 2]);
        }
    }
}

void Mesh::set_gl_position_buffer_()
{
    assert(pmesh_->HasPositions());

    std::vector<glm::vec3> tv_positions; // per triangle-vertex 3D position (size = 3 x #triangles)

    // TODO: for each triangle, set tv_positions
    for (const auto &index : tv_indices_)
    {
        const aiVector3D &vertex = pmesh_->mVertices[index];
        tv_positions.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
    }

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_positions.size(), &tv_positions[0], GL_STATIC_DRAW);
}

void Mesh::set_gl_color_buffer_(unsigned int cs_idx)
{
    assert(pmesh_->HasVertexColors(cs_idx));

    std::vector<glm::vec3> tv_colors; // per triangle-vertex 3D position (size = 3 x #triangles)

    // TODO: for each triangle, set tv_colors
    for (const auto &index : tv_indices_)
    {
        const aiColor4D &color = pmesh_->mColors[cs_idx][index];
        tv_colors.push_back(glm::vec3(color.r, color.g, color.b));
    }

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_colors.size(), &tv_colors[0], GL_STATIC_DRAW);

    is_color_ = true;
}

void Mesh::set_gl_normal_buffer_(ShadingType shading_type)
{
    std::vector<glm::vec3> tv_flat_normals;   // per triangle-vertex flat normal (size = 3 x #triangles)
    std::vector<glm::vec3> tv_smooth_normals; // per triangle-vertex smooth normal (size = 3 x #triangles)
    std::vector<glm::vec3> v_smooth_normals;  // per-vertex 3D normal (size = #vertices)

    // Initialize per-vertex smooth normals to zero
    v_smooth_normals.resize(pmesh_->mNumVertices, glm::vec3(0.0f));

    // Compute per-triangle normals and add to per-vertex normals
    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i)
    {
        aiFace &ai_face = pmesh_->mFaces[i];
        assert(ai_face.mNumIndices >= 3);

        const aiVector3D &v0 = pmesh_->mVertices[ai_face.mIndices[0]];
        const aiVector3D &v1 = pmesh_->mVertices[ai_face.mIndices[1]];
        const aiVector3D &v2 = pmesh_->mVertices[ai_face.mIndices[2]];

        glm::vec3 normal = glm::normalize(glm::cross(
            glm::vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z),
            glm::vec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z)));

        for (unsigned int idx = 0; idx < ai_face.mNumIndices; ++idx)
        {
            v_smooth_normals[ai_face.mIndices[idx]] += normal;
        }

        // Store flat normals for each vertex in the face
        tv_flat_normals.push_back(normal);
        tv_flat_normals.push_back(normal);
        tv_flat_normals.push_back(normal);
    }

    // Normalize the per-vertex normals
    for (auto &normal : v_smooth_normals)
    {
        normal = glm::normalize(normal);
    }

    // Use Assimp's normals if available
    if (pmesh_->HasNormals())
    {
        memcpy(&v_smooth_normals[0], &pmesh_->mNormals[0], sizeof(pmesh_->mNormals[0]) * pmesh_->mNumVertices);
    }

    // Set up smooth normals for each vertex in the triangles
    for (const auto &index : tv_indices_)
    {
        tv_smooth_normals.push_back(v_smooth_normals[index]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    if (shading_type == kSmooth)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_smooth_normals.size(), &tv_smooth_normals[0], GL_STATIC_DRAW);
    }
    else // if (shading_type == kFlat)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_flat_normals.size(), &tv_flat_normals[0], GL_STATIC_DRAW);
    }
}

void Mesh::set_gl_buffers(ShadingType shading_type)
{
    set_gl_position_buffer_();
    if (pmesh_->HasVertexColors(0))
        set_gl_color_buffer_(0);
    set_gl_normal_buffer_(shading_type);
}

void Mesh::draw(int loc_a_position, int loc_a_normal)
{
    // TODO : draw a triangular mesh
    //          glBindBuffer() with position_buffer_
    //          glEnableVertexAttribArray() for loc_a_position
    //          glVertexAttribPointer() by reusing GPU data in loc_a_position
    //
    //          glBindBuffer() with normal_buffer_
    //          glEnableVertexAttribArray() for loc_a_normal
    //          glVertexAttribPointer() by reusing GPU data in loc_a_normal
    //
    //          glDrawArrays(GL_TRIANGLES, ...)
    //
    //          glDisableVertexAttribArray() for loc_a_position & loc_a_normal

    // Bind position buffer
    glEnableVertexAttribArray(loc_a_position);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind normal buffer
    glEnableVertexAttribArray(loc_a_normal);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Draw the mesh
    glDrawArrays(GL_TRIANGLES, 0, tv_indices_.size());

    // Disable attribute arrays
    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_normal);
}

void Mesh::print_info()
{
    std::cout << "print mesh info" << std::endl;

    std::cout << "num vertices " << pmesh_->mNumVertices << std::endl;
    for (unsigned int i = 0; i < pmesh_->mNumVertices; ++i)
    {
        aiVector3D vertex = pmesh_->mVertices[i];
        std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

        if (pmesh_->mColors[0] != NULL)
        {
            aiColor4D color = pmesh_->mColors[0][i];
            std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
        }
    }
}