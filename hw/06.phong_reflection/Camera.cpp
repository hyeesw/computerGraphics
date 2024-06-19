#include "Camera.h"

// TODO: fill up the following function properly
void Camera::set_rotation(const glm::quat &_q)
{
  front_dir_ = glm::normalize(_q * glm::vec3(0.0f, 0.0f, -1.0f));
  up_dir_ = glm::normalize(_q * glm::vec3(0.0f, 1.0f, 0.0f));
  right_dir_ = glm::normalize(_q * glm::vec3(1.0f, 0.0f, 0.0f));
}

// TODO: re-write the following function properly
const glm::quat Camera::get_rotation() const
{
  return glm::quatLookAt(front_dir_, up_dir_);
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::quat &_q, const glm::vec3 &_t)
{
  set_rotation(_q);
  position_ = _t;
}

// TODO: fill up the following function properly
void Camera::get_pose(glm::quat &_q, glm::vec3 &_t) const
{
  _q = get_rotation();
  _t = position_;
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_pose() const
{
  glm::mat4 pose = glm::mat4_cast(get_rotation());
  pose[3] = glm::vec4(position_, 1.0f);
  return pose;
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::mat4 &_frame)
{
  // +x_cam: right direction of the camera    (it should be a unit vector whose length is 1)
  // right_dir_ = ..... ; // +x
  // +y_cam: up direction of the camera       (it should be a unit vector whose length is 1)
  // up_dir_    = ..... ;    // +y
  // -z_cam: front direction of the camera    (it should be a unit vector whose length is 1)
  // front_dir_ = ..... ;    // -z
  // pos_cam: position of the camera
  // position_  = ..... ;    // pos

  glm::mat3 rotationMatrix = glm::mat3(_frame);
  glm::quat rotation = glm::quat_cast(rotationMatrix);
  glm::vec3 translation = glm::vec3(_frame[3]);

  set_rotation(rotation);
  position_ = translation;
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::vec3 &_pos, const glm::vec3 &_at, const glm::vec3 &_up_dir)
{
  front_dir_ = glm::normalize(_at - _pos);
  right_dir_ = glm::normalize(glm::cross(_up_dir, front_dir_));
  up_dir_ = glm::normalize(glm::cross(front_dir_, right_dir_));
  position_ = _pos;
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_view_matrix() const
{
  glm::vec3 center = position() + front_direction();
  return glm::lookAt(position(), center, up_direction());
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_projection_matrix() const
{
  // TODO: Considering the followings,
  //       You must return a proper projection matrix
  //       i) camera mode: it can be either kOrtho or kPerspective
  //       ii) zoom-in/-out: if the camera mode is kOrtho,
  //                         utilize ortho_scale_ for zoom-in/-out
  //                         if the camera mode is kPerspective,
  //                         utilize fovy_ for zoom-in/-out
  //       iii) aspect ratio: utilize aspect_ in the both camera modes
  //       iv) near/far clipping planes: utilize near_, far_

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

// TODO: fill up the following functions properly
void Camera::move_forward(float delta)
{
  position_ += delta * front_direction();
}

// TODO: fill up the following functions properly
void Camera::move_backward(float delta)
{
  position_ -= delta * front_direction();
}

// TODO: fill up the following functions properly
void Camera::move_left(float delta)
{
  position_ -= delta * right_direction();
}

// TODO: fill up the following functions properly
void Camera::move_right(float delta)
{
  position_ += delta * right_direction();
}

// TODO: fill up the following functions properly
void Camera::move_up(float delta)
{
  position_ += up_dir_ * delta;
}

// TODO: fill up the following functions properly
void Camera::move_down(float delta)
{
  position_ -= up_dir_ * delta;
}
