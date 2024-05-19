#include "Camera.h"

// FIXME
void Camera::set_rotation(const glm::quat& _q) {
    // input 쿼터니언을 이용해 회전 행렬을 만든다
    glm::mat4 rotation_matrix = glm::toMat4(_q);

    // 회전 행렬을 이용해 front, up, right 방향을 계산한다
    front_dir_ = glm::normalize(glm::vec3(rotation_matrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    up_dir_ = glm::normalize(glm::vec3(rotation_matrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    right_dir_ = glm::normalize(glm::vec3(rotation_matrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
}

// FIXED
const glm::quat Camera::get_rotation() const {
    // 방향 벡터를 이용해 회전행렬을 만든다
    glm::mat4 rotation_matrix(
        glm::vec4(right_dir_, 0.0f),
        glm::vec4(up_dir_, 0.0f),
        glm::vec4(-front_dir_, 0.0f), // front_dir은 -z 방향
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    return glm::quat_cast(rotation_matrix); //회전행렬을 쿼터니언으로 변환
}

// FIXED
void Camera::set_pose(const glm::quat& _q, const glm::vec3& _t) {
    set_rotation(_q);
    position_ = _t;
}

// FIXED
void Camera::get_pose(glm::quat& _q, glm::vec3& _t) const {
    _q = get_rotation();
    _t = position_;
}

// FIXED
const glm::mat4 Camera::get_pose() const {
  return glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(get_rotation());
}

// FIXED
void Camera::set_pose(const glm::mat4& _frame) {
  position_ = glm::vec3(_frame[3]);
  set_rotation(glm::quat_cast(_frame));
}

// FIXED
void Camera::set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir){
  position_ = _pos; 
  front_dir_ = glm::normalize(_at - _pos);
  up_dir_    = glm::normalize(_up_dir);
  right_dir_ = glm::normalize(glm::cross(front_dir_, up_dir_));
  up_dir_ = glm::cross(right_dir_, front_dir_); // Re-calculate to ensure orthogonality
}

// FIXED
const glm::mat4 Camera::get_view_matrix() const
{
  return glm::lookAt(position_, position_ + front_dir_, up_dir_);
}

// FIXED
const glm::mat4 Camera::get_projection_matrix() const
{
if (mode_ == kPerspective) {
        return glm::perspective(glm::radians(fovy_), aspect_, near_, far_);
    } else { // kOrtho
        float ortho_half_height = ortho_scale_ * aspect_;
        float ortho_half_width = ortho_scale_;
        return glm::ortho(-ortho_half_width, ortho_half_width, -ortho_half_height, ortho_half_height, near_, far_);
    }
}

// FIXED
void Camera::move_forward(float delta) {
  position_ += front_dir_ * delta;
}
void Camera::move_backward(float delta){
  position_ -= front_dir_ * delta;
}
void Camera::move_left(float delta) {
  position_ -= right_dir_ * delta;
}
void Camera::move_right(float delta){
  position_ += right_dir_ * delta;
}
void Camera::move_up(float delta){
  position_ += up_dir_ * delta;
}
void Camera::move_down(float delta){
  position_ -= up_dir_ * delta;
}