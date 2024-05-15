#include "Camera.h"
#include <iostream>
using namespace std;

// TODO: fill up the following function properly 
void Camera::set_rotation(const glm::quat& _q) {

}
// 현재 카메라의 회전 상태를 quaternion 형태로 반환 (어디를 바라보고 있는지)
const glm::quat Camera::get_rotation() const {
  return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

// quaternion을 사용하여 카메라의 회전을 설정 & 벡터를 사용하여 카메라의 위치를 설정 (특정 위치로 이동시키고 특정 방향을 보도록)
void Camera::set_pose(const glm::quat& _q, const glm::vec3& _t){
  set_rotation(_q); //회전
  position_ = _t; //위치
}

//현재 카메라의 회전(quaternion)과 위치(3D 벡터)를 반환 (어디에 위치해 있고, 어느 방향을 보고 있는지)
void Camera::get_pose(glm::quat& _q, glm::vec3& _t) const {

}

// 현재 카메라의 위치와 회전 정보를 4x4 행렬로 반환 (카메라의 전체적인 위치와 방향 정보를 행렬 형태로)
const glm::mat4 Camera::get_pose() const {
  return  glm::mat4(1.0f);
}

// 4x4 행렬로 카메라의 위치와 회전을 설정 (행렬을 사용해서 카메라의 위치와 방향을 한 번에 설정)
void Camera::set_pose(const glm::mat4& _frame) {
  
  // +x_cam: right direction of the camera    (it should be a unit vector whose length is 1)
  // right_dir_ = ..... ; // +x
  // +y_cam: up direction of the camera       (it should be a unit vector whose length is 1)   
  // up_dir_    = ..... ;    // +y
  // -z_cam: front direction of the camera    (it should be a unit vector whose length is 1)
  // front_dir_ = ..... ;    // -z
  // pos_cam: position of the camera
  // position_  = ..... ;    // pos
}

// 카메라의 위치, 목표 지점, 업 벡터를 사용하여 카메라의 위치와 방향을 설정 (카메라를 특정 위치로 이동시키고, 특정 지점을 바라보도록)
void Camera::set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir) {
  // up_dir_    = ..... ;
  // front_dir_ = ..... ;    // -z_cam direction
  // right_dir_ = ..... ;    // +x_cam direction
  // up_dir_    = ..... ;    // +y_cam direction

  // position_  = ..... ;      // pos
}

// 현재 카메라의 view 행렬을 반환
const glm::mat4 Camera::get_view_matrix() const {
  return glm::mat4(1.0f);
}

// 현재 카메라의 projection 행렬을 반환 (3D 장면을 2D 화면에 어떻게 투영할지)
const glm::mat4 Camera::get_projection_matrix() const {
  // TODO: Considering the followings,
  //       You must return a proper projection matrix
  //       i) camera mode: it can be either kOrtho or kPerspective
  //       ii) zoom-in/-out: if the camera mode is kOrtho, 
  //                         utilize ortho_scale_ for zoom-in/-out
  //                         if the camera mode is kPerspective,
  //                         utilize fovy_ for zoom-in/-out
  //       iii) aspect ratio: utilize aspect_ in the both camera modes
  //       iv) near/far clipping planes: utilize near_, far_

  return glm::mat4(1.0f);
}
// !
void Camera::move_forward(float delta) {
  std::cout << "FF Camera Position - x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
  position_ += front_dir_ * delta;
}
void Camera::move_backward(float delta) {
  position_ -= front_dir_ * delta;
}
void Camera::move_left(float delta) {
  position_ -= right_dir_ * delta;
}
void Camera::move_right(float delta) {
  std::cout << "RR Camera Position - x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
  position_ += right_dir_ * delta;
}
void Camera::move_up(float delta) {
  position_ += up_dir_ * delta;
}
void Camera::move_down(float delta) {
  position_ -= up_dir_ * delta;
}
