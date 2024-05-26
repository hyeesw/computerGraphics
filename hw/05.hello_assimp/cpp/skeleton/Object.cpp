#include "Object.h"

void Object::init_buffer_objects() 
{
    // Create buffers
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, pmesh_->mNumVertices * 3 * sizeof(float), pmesh_->mVertices, GL_STATIC_DRAW);

    if (pmesh_->HasVertexColors(0))
    {
        is_color = true;
        std::vector<GLfloat> colors;
        colors.reserve(pmesh_->mNumVertices * 3); // assuming RGB colors

        for (unsigned int i = 0; i < pmesh_->mNumVertices; ++i)
        {
            aiColor4D color = pmesh_->mColors[0][i];
            colors.push_back(color.r);
            colors.push_back(color.g);
            colors.push_back(color.b);
            // You can also include alpha channel if needed
        }

        glGenBuffers(1, &color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
    }

    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i)
    {
        Face face;
        const aiFace& ai_face = pmesh_->mFaces[i];

        glGenBuffers(1, &face.index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face.index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ai_face.mNumIndices * sizeof(unsigned int), ai_face.mIndices, GL_STATIC_DRAW);

        face.num_indices = ai_face.mNumIndices;
        faces.push_back(face);
    }
}

glm::mat4 Object::get_model_matrix() const
{
    glm::mat4 mat_model = glm::mat4(1.0f);

    mat_model = glm::translate(mat_model, vec_translate_);
    mat_model *= glm::mat4_cast(quat_rotate_);
    mat_model = glm::scale(mat_model, vec_scale_);

    return mat_model;
}

void Object::draw(int loc_a_position, int loc_a_color)
{
    // Bind position buffer
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(loc_a_position);

    // Bind color buffer if available
    if (is_color)
    {
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glVertexAttribPointer(loc_a_color, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(loc_a_color);
    }

    // Draw all faces
    for (const Face& face : faces)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face.index_buffer);
        glDrawElements(GL_TRIANGLES, face.num_indices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(loc_a_position);
    if (is_color)
    {
        glDisableVertexAttribArray(loc_a_color);
    }
}
