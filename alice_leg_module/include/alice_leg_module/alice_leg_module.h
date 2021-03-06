/*
 * alice_leg_module.h
 *
 *  Created on: May 27, 2018
 *      Author: robotemperor
 */

#ifndef ALICE_HEROEHS_ALICE_MOTION_ALICE_LEG_MODULE_INCLUDE_ALICE_LEG_MODULE_ALICE_LEG_MODULE_H_
#define ALICE_HEROEHS_ALICE_MOTION_ALICE_LEG_MODULE_INCLUDE_ALICE_LEG_MODULE_ALICE_LEG_MODULE_H_

#include <map>
#include <ros/ros.h>
#include <ros/callback_queue.h>
#include <ros/package.h>
#include <boost/thread.hpp>

#include <Eigen/Dense>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <stdio.h>

#include "robotis_framework_common/motion_module.h"
//library
#include "robotis_math/robotis_math.h"
#include "heroehs_math/fifth_order_trajectory_generate.h"
#include "heroehs_math/kinematics.h"
#include "heroehs_math/end_point_to_rad_cal.h"
#include "alice_balance_control/zmp_calculation_function.h"

//message
//m - standard
#include <std_msgs/Int16.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/PointStamped.h>
#include <sensor_msgs/Imu.h>
#include "alice_msgs/ForceTorque.h"
#include "alice_msgs/BalanceParam.h"

//m - personal


namespace alice_leg_module
{

class AliceLegModule: public robotis_framework::MotionModule, public robotis_framework::Singleton<AliceLegModule>
{
public:
	AliceLegModule();
	virtual ~AliceLegModule();

	/* ROS Framework Functions */
	void initialize(const int control_cycle_msec, robotis_framework::Robot *robot);
	void process(std::map<std::string, robotis_framework::Dynamixel *> dxls, std::map<std::string, double> sensors);
	void updateBalanceParameter();

	void stop();
	bool isRunning();

	bool gazebo_check;

	// paper messages
//	ros::Publisher current_leg_pose_pub;
/*	ros::Publisher l_leg_point_xyz_pub;
	ros::Publisher l_leg_point_rpy_pub;
	ros::Publisher r_leg_point_xyz_pub;
	ros::Publisher r_leg_point_rpy_pub;

	ros::Publisher l_compensation_xyz_pub;
	ros::Publisher l_compensation_rpy_pub;
	ros::Publisher r_compensation_xyz_pub;
	ros::Publisher r_compensation_rpy_pub;*/

	ros::Publisher zmp_fz_pub;
	ros::Subscriber get_ft_data_sub_;
	ros::Subscriber set_balance_param_sub_;
	//ros::Subscriber walking_path_sub_;

	void ftDataMsgCallback(const alice_msgs::ForceTorque::ConstPtr& msg);
	void setBalanceParameterCallback(const alice_msgs::BalanceParam::ConstPtr& msg);
	//void walkingPathMsgCAllBack(const alice_msgs::BalanceParam::ConstPtr& msg);

	// sensor data & balance on off
	//ros::Subscriber get_imu_data_sub_;
	//ros::Subscriber desired_pose_all_sub;

	/* ROS Topic Callback Functions */
	//void imuDataMsgCallback(const sensor_msgs::Imu::ConstPtr& msg);
	//void desiredPoseMsgCallback(const std_msgs::Float64MultiArray::ConstPtr& msg);
	//void desiredPoseAllMsgCallback(const diana_msgs::DesiredPoseCommand::ConstPtr& msg);

private:
	void queueThread();
	bool running_;
	int control_cycle_msec_;

	int new_count_;


	// leg state
/*	std_msgs::Float64MultiArray current_leg_pose_msg_;
	geometry_msgs::Vector3 l_leg_point_xyz_msg_;
	geometry_msgs::Vector3 l_leg_point_rpy_msg_;
	geometry_msgs::Vector3 r_leg_point_xyz_msg_;
	geometry_msgs::Vector3 r_leg_point_rpy_msg_;

	geometry_msgs::Vector3 l_compensation_xyz_msg_;
	geometry_msgs::Vector3 l_compensation_rpy_msg_;
	geometry_msgs::Vector3 r_compensation_xyz_msg_;
	geometry_msgs::Vector3 r_compensation_rpy_msg_;*/


	boost::thread queue_thread_;

	std::map<std::string, int> joint_name_to_id_;
	std::map<int, std::string> joint_id_to_name_;

	double traj_time_;

	bool is_moving_l_;
	bool is_moving_r_;
	bool is_moving_one_joint_;

	Eigen::MatrixXd leg_end_point_l_;
	Eigen::MatrixXd leg_end_point_r_;

	heroehs_math::Kinematics *l_kinematics_;
	heroehs_math::Kinematics *r_kinematics_;
	heroehs_math::CalRad *end_to_rad_l_;
	heroehs_math::CalRad *end_to_rad_r_;

	Eigen::MatrixXd result_end_l_;
	Eigen::MatrixXd result_end_r_;
	Eigen::Matrix4d result_mat_cob_, result_mat_cob_modified_;
	Eigen::Matrix4d result_mat_l_, result_mat_l_modified_;
	Eigen::Matrix4d result_mat_r_, result_mat_r_modified_;
	robotis_framework::Pose3D result_pose_l_modified_;
	robotis_framework::Pose3D result_pose_r_modified_;

	/*// balance gyro
	Eigen::MatrixXd result_end_l_;
	Eigen::MatrixXd result_end_r_;
	Eigen::Matrix4d result_mat_cob_, result_mat_cob_modified_;
	Eigen::Matrix4d result_mat_l_, result_mat_l_modified_;
	Eigen::Matrix4d result_mat_r_, result_mat_r_modified_;
	robotis_framework::Pose3D result_pose_l_modified_;
	robotis_framework::Pose3D result_pose_r_modified_;

	diana::BalanceControlUsingPDController balance_ctrl_;
	void updateBalanceParameter();
	void gyroRotationTransformation(double gyro_z, double gyro_y, double gyro_x);
	double currentGyroX,currentGyroY,currentGyroZ;
	double tf_current_gyro_x, tf_current_gyro_y, tf_current_gyro_z;
	diana_msgs::BalanceParam previous_balance_param_, desired_balance_param_;
	robotis_framework::FifthOrderPolynomialTrajectory balance_param_update_coeff_;
	double balance_updating_duration_sec_;
	double balance_updating_sys_time_sec_;
	bool balance_update_;

	// balance gyro joint space pitch
	heroehs_math::FifthOrderTrajectory *gain_pitch_p_adjustment;
	heroehs_math::FifthOrderTrajectory *gain_pitch_d_adjustment;
	control_function::PID_function *gyro_pitch_function;
	double updating_duration;
	double gyro_pitch_p_gain;
	double gyro_pitch_d_gain;*/

	/*// cop
	diana::CopCalculationFunc *cop_cal;
	double currentFX_l,currentFY_l,currentFZ_l,currentTX_l,currentTY_l,currentTZ_l;
	double currentFX_r,currentFY_r,currentFZ_r,currentTX_r,currentTY_r,currentTZ_r;
	std_msgs::Float64MultiArray cop_fz_msg_;

	//cop compensation
	diana::CopCompensationFunc *cop_compensation;
	heroehs_math::FifthOrderTrajectory *gain_copFz_p_adjustment;
	heroehs_math::FifthOrderTrajectory *gain_copFz_d_adjustment;
	double updating_duration_cop;
	double copFz_p_gain;
	double copFz_d_gain;*/

    robotis_framework::FifthOrderPolynomialTrajectory balance_param_update_coeff_;
	double balance_updating_duration_sec_;
	double balance_updating_sys_time_sec_;
	bool balance_update_;

	alice::ZmpCalculationFunc *zmp_cal;
	double currentFX_l,currentFY_l,currentFZ_l,currentTX_l,currentTY_l,currentTZ_l;
	double currentFX_r,currentFY_r,currentFZ_r,currentTX_r,currentTY_r,currentTZ_r;
	std_msgs::Float64MultiArray zmp_fz_msg_;

	//zmp compensation
	alice::ZmpCompensationFunc *zmp_compensation;
	heroehs_math::FifthOrderTrajectory *gain_zmpFz_p_adjustment;
	heroehs_math::FifthOrderTrajectory *gain_zmpFz_d_adjustment;
	double updating_duration_cop;
	double zmpFz_p_gain;
	double zmpFz_d_gain;
};
}







#endif /* ALICE_HEROEHS_ALICE_MOTION_ALICE_LEG_MODULE_INCLUDE_ALICE_LEG_MODULE_ALICE_LEG_MODULE_H_ */
