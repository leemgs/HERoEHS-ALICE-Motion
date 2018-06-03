/*
 * alice_upper_body_module.cpp
 *
 *  Created on: May 30, 2018
 *      Author: robotemperor
 */
#include "alice_upper_body_module/alice_upper_body_module.h"

using namespace alice_upper_body_module;

void UpperBodyModule::initialize(const int control_cycle_msec, robotis_framework::Robot *robot)
{
	control_cycle_msec_ = control_cycle_msec;
	queue_thread_ = boost::thread(boost::bind(&UpperBodyModule::queueThread, this));

	for (std::map<std::string, robotis_framework::Dynamixel*>::iterator it = robot->dxls_.begin();
			it != robot->dxls_.end(); it++)
	{
		std::string joint_name = it->first;
		robotis_framework::Dynamixel* dxl_info = it->second;

		joint_name_to_id_[joint_name] = dxl_info->id_;
		joint_id_to_name_[dxl_info->id_] = joint_name;
	}
	// waist yaw roll
	waist_end_point_.resize(6,8);
	waist_end_point_.fill(0);
	result_rad_waist_.resize(6,1);
	result_rad_waist_.fill(0);

	// head
	head_end_point_.resize(6,8);
	head_end_point_.fill(0);
	result_rad_head_.resize(6,1);
	result_rad_head_.fill(0);

	head_end_point_(4,0) = 20*DEGREE2RADIAN; // pitch 초기값
	head_end_point_(4,1) = 20*DEGREE2RADIAN; //
	end_to_rad_head_->cal_end_point_tra_betta->current_pose = 20*DEGREE2RADIAN;
	end_to_rad_head_->current_pose_change(4,0) = 20*DEGREE2RADIAN;
	temp_pre_pitch = 20*DEGREE2RADIAN; // low pass filter initialize


	for(int joint_num_= 3; joint_num_< 6 ; joint_num_ ++)  // waist 3, 5번 // head 345 초기화
	{
		waist_end_point_(joint_num_, 7) = traj_time_test;
		head_end_point_ (joint_num_, 7) = traj_time_test;
	}
	waist_end_point_(4, 7) = 0;

	ROS_INFO("< -------  Initialize Module : Upper Body Module  [HEAD  && WAIST] !!  ------->");
}
double UpperBodyModule::limitCheck(double calculated_value, double max, double min)
{
	if(calculated_value > (max*DEGREE2RADIAN))
		return (max*DEGREE2RADIAN);
	else if (calculated_value < (min*DEGREE2RADIAN))
		return (min*DEGREE2RADIAN);
	else
		return calculated_value;
}
bool UpperBodyModule::isRunning()
{
	return running_;
}
void UpperBodyModule::process(std::map<std::string, robotis_framework::Dynamixel *> dxls,
		std::map<std::string, double> sensors)
{

	//algorithm_process(command);
	tracking_function();

	if (enable_ == false)
	{
		return;
	}
	head_end_point_(3,1) = limitCheck(head_end_point_(3,1),135,-135);
	head_end_point_(4,1) = limitCheck(head_end_point_(4,1),85,-25);

	result_rad_head_  = end_to_rad_head_  -> cal_end_point_to_rad(head_end_point_);
	//is_moving_head_  = end_to_rad_head_  -> is_moving_check;

	temp_head_yaw   = limitCheck(result_rad_head_(3,0),135,-135);
	temp_head_pitch = limitCheck(result_rad_head_(4,0),85,-25);


	waist_end_point_(3,1)   = limitCheck(waist_end_point_(3,1),60,-60);
	waist_end_point_(4,1)   = limitCheck(waist_end_point_(4,1),85,-15);

	result_rad_waist_ = end_to_rad_waist_ -> cal_end_point_to_rad(waist_end_point_);
	//is_moving_waist_ = end_to_rad_waist_ -> is_moving_check;

	result_[joint_id_to_name_[7]]-> goal_position_  =  filter_head->lowPassFilter(temp_head_pitch, temp_pre_pitch, 0.02, 0.008);
	result_[joint_id_to_name_[8]]-> goal_position_  =  filter_head->lowPassFilter(temp_head_yaw, temp_pre_yaw, 0.02, 0.008);

	//printf("pitch ::  %f \n", result_[joint_id_to_name_[7]]-> goal_position_);

	result_[joint_id_to_name_[9]] -> goal_position_  = result_rad_waist_ (3,0); // waist pitch
	result_[joint_id_to_name_[10]]-> goal_position_  = result_rad_waist_ (4,0); // waist yaw

	temp_pre_roll  = temp_head_roll;
	temp_pre_pitch = temp_head_pitch;
	temp_pre_yaw   = temp_head_yaw;
}
void UpperBodyModule::stop()
{
	return;
}

// algorithm
void UpperBodyModule::finding_motion()
{
	double motion_time_ = 3.0;
	static double current_time_ = 0.0;
	static int motion_num_ = 1;
	waist_end_point_(3, 7)  = motion_time_;
	waist_end_point_(4, 7)  = motion_time_;
	head_end_point_(3, 7)   = motion_time_;
	head_end_point_(4, 7)   = motion_time_;

	if(current_time_ >= 0 && current_time_ < motion_time_&& motion_num_ == 1)
	{
		waist_end_point_(3, 1) = 0;
		waist_end_point_(4, 1) = 0;
		head_end_point_(3, 1)  = 0;
		head_end_point_(4, 1)  = 70*DEGREE2RADIAN;

	}
	else if(current_time_ >= motion_time_ && current_time_ < motion_time_*2 && motion_num_ == 2)
	{
		waist_end_point_(3, 1) = 55*DEGREE2RADIAN;
		//waist_end_point_(4, 1) = 0;
		head_end_point_(3, 1)  = 90*DEGREE2RADIAN;
		//head_end_point_(4, 1)  = 85*DEGREE2RADIAN;

	}
	else if(current_time_ >= motion_time_ && current_time_ < motion_time_*3 && motion_num_ == 3)
	{
		waist_end_point_(3, 1) = 0*DEGREE2RADIAN;
		//waist_end_point_(4, 1) = 0;
		head_end_point_(3, 1)  = 0*DEGREE2RADIAN;
		//head_end_point_(4, 1)  = 85*DEGREE2RADIAN;

	}
	else if(current_time_ >= motion_time_*3 && current_time_ < motion_time_*4 && motion_num_ == 4)
	{
		waist_end_point_(3, 1) = -55*DEGREE2RADIAN;
		//waist_end_point_(3, 7)  = 6;
		//waist_end_point_(4, 1) = 0;
		head_end_point_(3, 1)  = -90*DEGREE2RADIAN;
		//head_end_point_(3, 7)  = 6;
		//head_end_point_(4, 1)  = 0;

	}
	else if(current_time_ >= motion_time_*4 && current_time_ < motion_time_*5 && motion_num_ == 5)
	{
		waist_end_point_(3, 1) = 0;
		waist_end_point_(4, 1) = 0;
		head_end_point_(3, 1)  = 0;
		head_end_point_(4, 1)  = 10*DEGREE2RADIAN;
	}
	/*	else if(current_time_ >= motion_time_*4 && current_time_ < motion_time_*5 && motion_num_ == 5)
	{
		waist_end_point_(3, 1) = 55*DEGREE2RADIAN;
		waist_end_point_(4, 1) = 0;
	    head_end_point_(3, 1)  = 0;
		head_end_point_(4, 1)  = 0;
	}
	else if(current_time_ >= motion_time_*5 && current_time_ < motion_time_*6 && motion_num_ == 6)
	{
		waist_end_point_(3, 1) = 0;
		waist_end_point_(4, 1) = 0;
		head_end_point_(3, 1)  = 0;
		head_end_point_(4, 1)  = 20*DEGREE2RADIAN;
	}*/
	else
	{
		motion_num_++;
		if(motion_num_ == 6)
		{
			motion_num_ = 0;
			current_time_ = 0;
		}
	}
	current_time_ = current_time_+ 0.008;
}
void UpperBodyModule::updateBalanceParameter()
{
	if(balance_update_ == false)
		return;
	balance_updating_sys_time_sec_ += 0.008;

	if(balance_updating_sys_time_sec_ > balance_updating_duration_sec_)
	{
		balance_updating_sys_time_sec_ = 0;


		balance_update_ = false;
	}
	else
	{
		Eigen::MatrixXd value;
		value.resize(1,8);
		value.fill(0);
		value(0,7) = balance_updating_duration_sec_;
		value(0,1) = x_p_gain;
		pidController_x->kp_ = gain_x_p_adjustment -> fifth_order_traj_gen_one_value(value);
		value(0,1) = y_p_gain;
		pidController_y->kp_ = gain_y_p_adjustment -> fifth_order_traj_gen_one_value(value);
		value(0,1) = x_d_gain;
		pidController_x->kd_ = gain_x_d_adjustment -> fifth_order_traj_gen_one_value(value);
		value(0,1) = y_d_gain;
		pidController_y->kd_ = gain_y_d_adjustment -> fifth_order_traj_gen_one_value(value);

	}


}
void UpperBodyModule::tracking_function()
{

	updateBalanceParameter();

	control_angle_yaw   = pidController_x->PID_calculate(desired_x, current_x);
	control_angle_pitch = pidController_y->PID_calculate(desired_y, current_y);

	control_angle_yaw   += control_angle_yaw;
	control_angle_pitch += control_angle_pitch;

	head_end_point_(3, 1)  = control_angle_yaw;
	head_end_point_(4, 1)  = control_angle_pitch + 20*DEGREE2RADIAN;
	head_end_point_(3, 7)  = 0.5;
	head_end_point_(4, 7)  = 0.5;

	//printf("yaw   control value ::  %f \n",control_angle_yaw);
	//printf("pitch control value ::  %f \n",control_angle_pitch);


}
void UpperBodyModule::algorithm_process(std::string command_)
{
	if(!command_.compare("finding"))
	{
		finding_motion();
	}
	else if(!command_.compare("initial"))
	{
		waist_end_point_(3, 1) = 0; // yaw  트레젝토리 6 * 8 은 xyz yaw(z) pitch(y) roll(x) 이며 8은 처음 위치 나중 위치 / 속도 속도 / 가속도 가속도 / 시간 시간 / 임
		waist_end_point_(4, 1) = 0; // pitch

		head_end_point_(3, 1)  = 0; // yaw  트레젝토리 6 * 8 은 xyz yaw(z) pitch(y) roll(x) 이며 8은 처음 위치 나중 위치 / 속도 속도 / 가속도 가속도 / 시간 시간 / 임
		head_end_point_(4, 1)  = 0; // pitch
	}
	else // tracking algorithm
	{

	}

}





