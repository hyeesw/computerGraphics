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


// Mesh.cpp에서 메쉬 데이터를 GPU 버퍼로 설정
void Mesh::set_gl_position_buffer_() {
    std::vector<glm::vec3> tv_positions(tv_indices_.size()); // 벡터 사이즈 초기화
    for (size_t i = 0; i < tv_indices_.size(); i++) {
        const aiVector3D& vertex = pmesh_->mVertices[tv_indices_[i]];
        tv_positions[i] = glm::vec3(vertex.x, vertex.y, vertex.z); // aiVector3D에서 glm::vec3로 변환
    }

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_positions.size(), tv_positions.data(), GL_STATIC_DRAW);
}

void Mesh::set_gl_color_buffer_(unsigned int cs_idx) {
    std::vector<glm::vec3> tv_colors(tv_indices_.size());
    for (size_t i = 0; i < tv_indices_.size(); i++) {
        aiColor4D color = pmesh_->mColors[cs_idx][tv_indices_[i]];
        tv_colors[i] = glm::vec3(color.r, color.g, color.b);
    }
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_colors.size(), tv_colors.data(), GL_STATIC_DRAW);
}


void Mesh::set_gl_normal_buffer_(ShadingType shading_type)
{
    std::vector<glm::vec3> tv_flat_normals(tv_indices_.size());
    std::vector<glm::vec3> tv_smooth_normals(tv_indices_.size());
    std::vector<glm::vec3> v_smooth_normals(pmesh_->mNumVertices, glm::vec3(0.0f));

    // Compute normals for each triangle
    for (size_t i = 0; i < tv_indices_.size(); i += 3) {
        aiVector3D aiv0 = pmesh_->mVertices[tv_indices_[i]];
        aiVector3D aiv1 = pmesh_->mVertices[tv_indices_[i + 1]];
        aiVector3D aiv2 = pmesh_->mVertices[tv_indices_[i + 2]];

        glm::vec3 v0(aiv0.x, aiv0.y, aiv0.z);
        glm::vec3 v1(aiv1.x, aiv1.y, aiv1.z);
        glm::vec3 v2(aiv2.x, aiv2.y, aiv2.z);

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Assign flat normal for each vertex of the triangle
        tv_flat_normals[i] = tv_flat_normals[i + 1] = tv_flat_normals[i + 2] = normal;

        // Add to smooth normals
        v_smooth_normals[tv_indices_[i]] += normal;
        v_smooth_normals[tv_indices_[i + 1]] += normal;
        v_smooth_normals[tv_indices_[i + 2]] += normal;
    }

    // Normalize the smooth normals
    for (auto& n : v_smooth_normals) {
        n = glm::normalize(n);
    }

    // Assign smooth normals to tv_smooth_normals
    for (size_t i = 0; i < tv_indices_.size(); ++i) {
        tv_smooth_normals[i] = v_smooth_normals[tv_indices_[i]];
    }

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    if (shading_type == kSmooth) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_smooth_normals.size(), tv_smooth_normals.data(), GL_STATIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_flat_normals.size(), tv_flat_normals.data(), GL_STATIC_DRAW);
    }
}

void Mesh::set_gl_buffers(ShadingType shading_type)
{
    set_gl_position_buffer_();
    if (pmesh_->HasVertexColors(0))
        set_gl_color_buffer_(0);
    set_gl_normal_buffer_(shading_type);
}

void Mesh::draw(int loc_a_position, int loc_a_normal) {
    // 버텍스 배열 객체 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 노멀 배열 객체 바인딩
    if (normal_buffer_ != 0) {  // 정상적으로 버퍼가 생성되었는지 확인
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
        glEnableVertexAttribArray(loc_a_normal);
        glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // 삼각형 그리기
    glDrawArrays(GL_TRIANGLES, 0, tv_indices_.size());

    // 정점 배열 비활성화
    glDisableVertexAttribArray(loc_a_position);
    if (normal_buffer_ != 0) {
        glDisableVertexAttribArray(loc_a_normal);
    }
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