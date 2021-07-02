/**\file
 * \brief
 * Pose class definition.
 * @version 02.07.2021
 * @author João Pedro Carvalho de Souza
 */

#include "pose.h"

Pose::Pose(){
    name_ = "NULL";
    parent_frame_ = "NULL";
    position_ = Eigen::Translation3d(0,0,0);
    rpy_zyx_orientation_ = Eigen::Vector3d(0,0,0);
    Eigen::Quaterniond quaternion_orientation_= Eigen::Quaterniond (1,0,0,0);
};

Pose::Pose(std::string _name, std::string _parent_frame, Eigen::Translation3d _position,
           Eigen::Quaterniond _quaternion_orientation) {
    name_ = _name;
    parent_frame_ = _parent_frame;
    position_ = _position;
    quaternion_orientation_ = _quaternion_orientation;
    //rpy_zyx_orientation_ = quaternion_orientation_.matrix().eulerAngles(2,0,2); //ZYZ
    rpy_zyx_orientation_ = getTaitBryanZYXFromQuaternion(quaternion_orientation_);

}

Pose::Pose(std::string _name, std::string _parent_frame, Eigen::Translation3d _position,
           Eigen::Vector3d _rpy_zyx_orientation) {
    name_ = _name;
    parent_frame_ = _parent_frame;
    position_ = _position;
    rpy_zyx_orientation_ = _rpy_zyx_orientation;
    quaternion_orientation_ = getQuaternionFromZYXEuler(rpy_zyx_orientation_);

}

Pose::~Pose() {};

Eigen::Vector3d Pose::getRPYOrientationZYXOrder() {
    return rpy_zyx_orientation_;
}

Eigen::Quaterniond Pose::getQuaternionOrientation() {
    return quaternion_orientation_;
}

Eigen::Translation3d Pose::getPosition(){
    return position_;
}


std::string Pose::getName(){
    return name_;
}

std::string Pose::getParentName(){
    return parent_frame_;
}

void Pose::setRPYOrientationZYXOrder(Eigen::Vector3d _in){
    rpy_zyx_orientation_ = _in;
    quaternion_orientation_ = getQuaternionFromZYXEuler(rpy_zyx_orientation_);
}
void Pose::setQuaternionOrientation(Eigen::Quaterniond _in){
    quaternion_orientation_ = _in;
    rpy_zyx_orientation_ = getTaitBryanZYXFromQuaternion(quaternion_orientation_);
}
void Pose::setPosition(Eigen::Translation3d _in){
    position_ = _in;
}
void Pose::setName(std::string _in){
    name_ = _in;

}
void Pose::setParentName(std::string _in){
    parent_frame_ = _in;
}

/// <summary>
/// Transform a Euler angle representation (cumulative or not) to quaternion angle representation. The construction sequence is ZYX
/// </summary>
/// <param name="_v"> the Vector3 with Roll(x), Pitch(y) and Yaw(z) angles.</param>
/// <returns> The quaternion representation angles.</returns>
Eigen::Quaterniond Pose::getQuaternionFromZYXEuler(Eigen::Vector3d _v){
    Eigen::Quaterniond output;
    output = Eigen::AngleAxisd(rpy_zyx_orientation_.z(), Eigen::Vector3d::UnitZ())
                          * Eigen::AngleAxisd(rpy_zyx_orientation_.y(), Eigen::Vector3d::UnitY())
                          * Eigen::AngleAxisd(rpy_zyx_orientation_.x(), Eigen::Vector3d::UnitX());
    return output;
}

/// <summary>
/// Transform a quaternion angle representation to Tait-Bryan (or cumulative Euler) angle. The construction sequence is ZYX
/// </summary>
/// <param name="_q"> Quaternion.</param>
/// <returns> The Vector3 with Roll(x), Pitch(y) and Yaw(z) angles.</returns>
Eigen::Vector3d Pose::getTaitBryanZYXFromQuaternion(Eigen::Quaterniond _q) {
    //https://marc-b-reynolds.github.io/math/2017/04/18/TaitEuler.html

    double x = _q.x(),
            y = _q.y(),
            z = _q.z(),
            w = _q.w();

    Eigen::Vector3d output;

    double t0 = x * x - z * z,
           t1 = w * w - y * y,
           xx = 0.5 * (t0 + t1),            // 1/2 x of x'
           xy = x * y + w * z,              // 1/2 y of x'
           xz = w * y - x * z,              // 1/2 z of x'
           t = xx * xx + xy * xy,           // cos(theta)^2
           yz = 2.0 * (y * z + w * x);      // z of y'

    output.z() = (float) atan2(xy, xx);         // yaw   (psi)
    output.y() = (float) atan(xz / sqrt(t)); // pitch (theta)

    if (t != 0)
        output.x() = (float) atan2(yz, t1 - t0);
    else
        output.x() = (float) (2.0 * atan2(x, w) - sgnd(xz) * output.z());

    return output;
}

double Pose::sgnd(double _in) {
    return (_in > 0.0) ? 1.0 : ((_in < 0.0) ? -1.0 : 0.0);
}




