/**\file
 * \brief
 * Pose class declaration.
 * @version 02.07.2021
 * @author João Pedro Carvalho de Souza
 */

#ifndef MIMIC_GRASPING_SERVER_POSE_H
#define MIMIC_GRASPING_SERVER_POSE_H

#include <Eigen/Core>
#include <Eigen/Geometry>

class Pose{
public:

    Pose();
    Pose(std::string _name,std::string _parent_frame, Eigen::Translation3d _position, Eigen::Quaterniond _quaternion_orientation);
    Pose(std::string _name,std::string _parent_frame,Eigen::Translation3d _position, Eigen::Vector3d _rpy_zyx_orientation);

    ~Pose();

    Eigen::Vector3d getRPYOrientationZYXOrder();
    Eigen::Quaterniond getQuaternionOrientation();
    Eigen::Translation3d getPosition();
    std::string getName();
    std::string getParentName();

    void setRPYOrientationZYXOrder(Eigen::Vector3d _in);
    void setQuaternionOrientation(Eigen::Quaterniond _in);
    void setPosition(Eigen::Translation3d _in);
    void setName(std::string _in);
    void setParentName(std::string _in);

protected:
    std::string name_, parent_frame_;
    Eigen::Translation3d position_;
    Eigen::Vector3d rpy_zyx_orientation_;
    Eigen::Quaterniond quaternion_orientation_;

    Eigen::Quaterniond getQuaternionFromZYXEuler(Eigen::Vector3d _v);
    Eigen::Vector3d getTaitBryanZYXFromQuaternion(Eigen::Quaterniond _q);
    double sgnd(double _in);


};

#endif //MIMIC_GRASPING_SERVER_POSE_H
