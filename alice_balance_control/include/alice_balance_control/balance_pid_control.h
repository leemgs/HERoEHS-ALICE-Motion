/*
 * balance_pid_control.h
 *
 *  Created on: May 29, 2018
 *      Author: robotemperor
 */

#ifndef ALICE_HEROEHS_ALICE_MOTION_ALICE_BALANCE_CONTROL_INCLUDE_ALICE_BALANCE_CONTROL_BALANCE_PID_CONTROL_H_
#define ALICE_HEROEHS_ALICE_MOTION_ALICE_BALANCE_CONTROL_INCLUDE_ALICE_BALANCE_CONTROL_BALANCE_PID_CONTROL_H_

namespace alice
{

class BalancePIDController
{
public:
  BalancePIDController();
  ~BalancePIDController();

  double control_cycle_sec_;

  double desired_;

  double p_gain_;
  double i_gain_;
  double d_gain_;

  double getFeedBack(double present_sensor_output);

private:
  double curr_err_;
  double prev_err_;
  double sum_err_;
};


}



#endif /* ALICE_HEROEHS_ALICE_MOTION_ALICE_BALANCE_CONTROL_INCLUDE_ALICE_BALANCE_CONTROL_BALANCE_PID_CONTROL_H_ */
