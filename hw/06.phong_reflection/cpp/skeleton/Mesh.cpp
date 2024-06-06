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

    std::vector<glm::vec3>  tv_positions;

    for (auto idx : tv_indices_) {
        tv_positions.push_back(glm::vec3(pmesh_->mVertices[idx].x, pmesh_->mVertices[idx].y, pmesh_->mVertices[idx].z));
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
    std::vector<glm::vec3>      tv_flat_normals(tv_indices_.size());
    std::vector<glm::vec3>      v_smooth_normals(pmesh_->mNumVertices, glm::vec3(0.0f, 0.0f, 0.0f));

    // 평균 법선 계산
    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i)
    {
        aiFace& face = pmesh_->mFaces[i];
        glm::vec3 a = glm::vec3(pmesh_->mVertices[face.mIndices[0]].x, pmesh_->mVertices[face.mIndices[0]].y, pmesh_->mVertices[face.mIndices[0]].z);
        glm::vec3 b = glm::vec3(pmesh_->mVertices[face.mIndices[1]].x, pmesh_->mVertices[face.mIndices[1]].y, pmesh_->mVertices[face.mIndices[1]].z);
        glm::vec3 c = glm::vec3(pmesh_->mVertices[face.mIndices[2]].x, pmesh_->mVertices[face.mIndices[2]].y, pmesh_->mVertices[face.mIndices[2]].z);
        
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            v_smooth_normals[face.mIndices[j]] += normal;
        }
    }

    // 법선 벡터 정규화
    for (auto& normal : v_smooth_normals) {
        normal = glm::normalize(normal);
    }

    // tv_smooth_normals 설정
    for (auto idx : tv_indices_) {
        tv_flat_normals.push_back(v_smooth_normals[idx]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    if (shading_type == kSmooth) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v_smooth_normals.size(), &v_smooth_normals[0], GL_STATIC_DRAW);
    } else  {// if (shading_type == kFlat) 
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tv_flat_normals.size(), &tv_flat_normals[0], GL_STATIC_DRAW);
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