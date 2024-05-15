#include "Camera.h"

// FIXED
void Camera::set_rotation(const glm::quat& _q) {
    glm::mat4 rot = glm::mat4_cast(_q); // 회전 행렬로 변환

    // 쿼터니언을 바탕으로 새로운 'front' 방향 벡터 계산 (기본 front 방향은 -z)
    glm::vec3 new_front = glm::normalize(glm::vec3(rot * glm::vec4(0.0, 0.0, -1.0, 0.0)));
    // 'up' 벡터 계산 (기본 up 방향은 y)
    glm::vec3 new_up = glm::normalize(glm::vec3(rot * glm::vec4(0.0, 1.0, 0.0, 0.0)));

    // glm::lookAt를 사용하여 최종 view 행렬 계산
    glm::mat4 view = glm::lookAt(glm::vec3(0.0), new_front, new_up);

    // view 행렬에서 다시 방향 벡터 추출
    right_dir_ = glm::normalize(glm::vec3(view[0])); // view 행렬의 첫 번째 열은 right 벡터
    up_dir_ = glm::normalize(glm::vec3(view[1])); // view 행렬의 두 번째 열은 up 벡터
    front_dir_ = -glm::normalize(glm::vec3(view[2])); // view 행렬의 세 번째 열은 -front 벡터 (OpenGL은 right-handed 좌표계 사용)
}


// FIXED
const glm::quat Camera::get_rotation() const {
  return glm::quat_cast(glm::mat4(glm::vec4(right_dir_, 0.0f),
                                glm::vec4(up_dir_, 0.0f),
                                glm::vec4(-front_dir_, 0.0f), // -z for front direction
                                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
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