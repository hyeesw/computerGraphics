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
        aiFace& ai_face = pmesh_->mFaces[i];
        assert(ai_face.mNumIndices >= 3);

        // convert a polygon to a triangle fan
        for (unsigned int idx = 0; idx < ai_face.mNumIndices - 2; ++idx)
        {
            tv_indices_.push_back(ai_face.mIndices[0]);
            tv_indices_.push_back(ai_face.mIndices[idx+1]);
            tv_indices_.push_back(ai_face.mIndices[idx+2]);
        }
    }
}


void Mesh::set_gl_position_buffer_()
{
    assert(pmesh_->HasPositions());

    std::vector<glm::vec3>  tv_positions;       // per triangle-vertex 3D position (size = 3 x #triangles)
    
    // FIXED : for each triangle, set tv_positions
    tv_positions.reserve(tv_indices_.size());
    for (auto idx : tv_indices_)
    {
        aiVector3D& vertex = pmesh_->mVertices[idx];
        tv_positions.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tv_positions.size(), &tv_positions[0], GL_STATIC_DRAW);
}

void Mesh::set_gl_color_buffer_(unsigned int cs_idx)
{
    assert(pmesh_->HasVertexColors(cs_idx));

    std::vector<glm::vec3>  tv_colors;       // per triangle-vertex 3D position (size = 3 x #triangles)

    // FIXED: for each triangle, set tv_colors
    tv_colors.reserve(tv_indices_.size());
    for (auto idx : tv_indices_)
    {
         aiColor4D color = pmesh_->mColors[cs_idx][idx];
        tv_colors.push_back(glm::vec3(color.r, color.g, color.b));
    }
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tv_colors.size(), &tv_colors[0], GL_STATIC_DRAW);

    is_color_ = true;
}


void Mesh::set_gl_normal_buffer_(ShadingType shading_type)
{
    std::vector<glm::vec3> tv_flat_normals(tv_indices_.size(), glm::vec3(0.0f)); // Initialize with zeros
    std::vector<glm::vec3> tv_smooth_normals(tv_indices_.size(), glm::vec3(0.0f)); // Initialize with zeros
    std::vector<glm::vec3> v_smooth_normals(pmesh_->mNumVertices, glm::vec3(0.0f));

    // Calculate normals for smooth shading and accumulate them
    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i) {
        aiFace& face = pmesh_->mFaces[i];
        glm::vec3 v0(pmesh_->mVertices[face.mIndices[0]].x, pmesh_->mVertices[face.mIndices[0]].y, pmesh_->mVertices[face.mIndices[0]].z);
        glm::vec3 v1(pmesh_->mVertices[face.mIndices[1]].x, pmesh_->mVertices[face.mIndices[1]].y, pmesh_->mVertices[face.mIndices[1]].z);
        glm::vec3 v2(pmesh_->mVertices[face.mIndices[2]].x, pmesh_->mVertices[face.mIndices[2]].y, pmesh_->mVertices[face.mIndices[2]].z);

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Apply flat normal for each vertex of the triangle
        tv_flat_normals[3 * i] = normal;
        tv_flat_normals[3 * i + 1] = normal;
        tv_flat_normals[3 * i + 2] = normal;

        // Accumulate normals for smooth shading
        v_smooth_normals[face.mIndices[0]] += normal;
        v_smooth_normals[face.mIndices[1]] += normal;
        v_smooth_normals[face.mIndices[2]] += normal;
    }

    // Normalize the accumulated vertex normals for smooth shading
    for (auto& n : v_smooth_normals) {
        n = glm::normalize(n);
    }

    // Set triangle vertex normals for smooth shading
    for (unsigned int i = 0; i < tv_indices_.size(); ++i) {
        tv_smooth_normals[i] = v_smooth_normals[tv_indices_[i]];
    }

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    if (shading_type == kSmooth) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tv_smooth_normals.size(), tv_smooth_normals.data(), GL_STATIC_DRAW);
    } else { // For flat shading
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tv_flat_normals.size(), tv_flat_normals.data(), GL_STATIC_DRAW);
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
    // FIXED
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glEnableVertexAttribArray(loc_a_normal);
    glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(tv_indices_.size()));

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