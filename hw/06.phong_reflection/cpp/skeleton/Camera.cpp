#include "Camera.h"

void Camera::set_rotation(const glm::quat& _q)
{
  front_dir_ = glm::normalize(_q * glm::vec3(0.0f, 0.0f, -1.0f));
  up_dir_ = glm::normalize(_q * glm::vec3(0.0f, 1.0f, 0.0f));
  right_dir_ = glm::normalize(_q * glm::vec3(1.0f, 0.0f, 0.0f));
}

const glm::quat Camera::get_rotation() const
{
    return glm::quatLookAt(front_dir_, up_dir_);
}

void Camera::set_pose(const glm::quat& _q, const glm::vec3& _t)
{
  set_rotation(_q);
  position_ = _t;
}

void Camera::get_pose(glm::quat& _q, glm::vec3& _t) const
{
  _q = get_rotation();
  _t = position_;
}

const glm::mat4 Camera::get_pose() const
{
  glm::mat4 pose = glm::mat4_cast(get_rotation());
  pose[3] = glm::vec4(position_, 1.0f);
  return pose;
}

void Camera::set_pose(const glm::mat4& _frame)
{
  glm::mat3 rotationMatrix = glm::mat3(_frame);
  glm::quat rotation = glm::quat_cast(rotationMatrix);
  glm::vec3 translation = glm::vec3(_frame[3]);

  set_rotation(rotation);
  position_ = translation;
}

void Camera::set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir)
{
  front_dir_ = glm::normalize(_at - _pos);
  right_dir_ = glm::normalize(glm::cross(_up_dir, front_dir_));
  up_dir_ = glm::normalize(glm::cross(front_dir_, right_dir_));
  position_ = _pos;
}

const glm::mat4 Camera::get_view_matrix() const
{
  glm::vec3 center = position() + front_direction();
  return glm::lookAt(position(), center, up_direction());
}

const glm::mat4 Camera::get_projection_matrix() const
{
  bool is_perspective = mode() == kPerspective ? true : false;
  if (is_perspective)
  {
    return glm::perspective(glm::radians(fovy()), aspect_, near_, far_);
  }
  else
  {
    float ortho_left = -aspect_ * ortho_scale_;
    float ortho_right = aspect_ * ortho_scale_;
    float ortho_bottom = -ortho_scale_;
    float ortho_top = ortho_scale_;
    return glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, near_, far_);
  }
}

void Camera::move_forward(float delta)
{
  position_ += delta * front_direction();
}

void Camera::move_backward(float delta)
{
  position_ -= delta * front_direction();
}

void Camera::move_left(float delta)
{
  position_ -= delta * right_direction();
}

void Camera::move_right(float delta)
{
  position_ += delta * right_direction();
}

void Camera::move_up(float delta)
{
  position_ += up_dir_ * delta;
}

void Camera::move_down(float delta)
{
  position_ -= up_dir_ * delta;
}
