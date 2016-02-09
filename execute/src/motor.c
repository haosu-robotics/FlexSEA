//****************************************************************************
// MIT Media Lab - Biomechatronics
// Jean-Francois (Jeff) Duval
// jfduval@mit.edu
// 02/2015
//****************************************************************************
// motor: motor control functions
//****************************************************************************

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "motor.h"
#include "analog.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//Main data structure for all the controllers:
volatile struct ctrl_s ctrl;

//Encoder:
struct enc_s encoder;

//Impedance loop
int debug_var = 0;

//****************************************************************************
// Function(s)
//****************************************************************************

//Controls motor PWM duty cycle
//Sign of 'pwm_duty' determines rotation direction
void motor_open_speed_1(int16 pwm_duty)
{
	int16 pdc = 0;
	uint16 tmp = 0;
	
	//Clip PWM to valid range
	if(pwm_duty >= MAX_PWM)
		pdc = MAX_PWM;
	else if(pwm_duty <= MIN_PWM)
		pdc = MIN_PWM;
	else
		pdc = pwm_duty;
	
	//Save value to structure:
	ctrl.pwm = pdc;
	
	//Change direction according to sign
	if(pdc < 0)
	{
		pdc = -pdc;	//Make it positive
		MotorDirection_Write(0);
	}
	else
	{
		MotorDirection_Write(1);
	}
	
	//Write duty cycle to PWM module
	tmp = PWM1DC((uint16)pdc);
	PWM_1_WriteCompare1(tmp);
	PWM_1_WriteCompare2(PWM2DC(tmp));	//Can't be 0 or the ADC won't trigger
}

//Controls motor PWM duty cycle
//Manual sign (-1 or 1, everything else will set the PWM at 0)
void motor_open_speed_2(int16 pwm_duty, int sign)
{
	int16 pdc = 0;
	
	//Clip PWM to valid range (0 - Maximum)
	if(pwm_duty >= MAX_PWM)
		pdc = MAX_PWM;
	else if(pwm_duty <= 0)
		pdc = 0;
	else
		pdc = pwm_duty;
	
	//Change direction according to sign
	if(sign == -1)
	{
		MotorDirection_Write(0);
	}
	else if(sign == 1)
	{
		MotorDirection_Write(1);
	}
	else
	{
		//Invalid sign, forces PWM to 0
		pdc = 0;
	}
	
	//Write duty cycle to PWM module
	pdc = PWM1DC(pdc);
	PWM_1_WriteCompare1(pdc);
	PWM_1_WriteCompare2(PWM2DC(pdc));	//Can't be 0 or the ADC won't trigger
}

//Second version of the serial interface, now with sign
//'x' clears the encoder count
int8 serial_motor_speed(int8 letter)
{
	static int8 step = 0;
	
	//'x' clears the encoder count
	if(letter == 'x')
	{
		QuadDec_1_SetCounter(0);
	}
	
	//Increase with s
	if(letter == 's')
		step++;
	if(letter == 'd')
		step--;

	//Step to do
	if(step >= 16)
		step = 0;		
	if(step <= -16)
		step = 0;

	//Set duty cycle
	motor_open_speed_1(200*step);
	
	//Return value to be displayed
	return(step + '0');
}

//Motor position controller - non blocking
int32 motor_position_pid(int32 wanted_pos, int32 actual_pos)
{
	int32 p = 0, i = 0;
	int32 pwm = 0;

	//Position values:
	ctrl.position.pos = actual_pos;
	ctrl.position.setp = wanted_pos;
	
	//Errors:
	ctrl.position.error = ctrl.position.pos - ctrl.position.setp;
	ctrl.position.error_sum = ctrl.position.error_sum + ctrl.position.error;
	//ctrl.position.error_dif ToDo
	
	//Saturate cumulative error
	if(ctrl.position.error_sum >= MAX_CUMULATIVE_ERROR)
		ctrl.position.error_sum = MAX_CUMULATIVE_ERROR;
	if(ctrl.position.error_sum <= -MAX_CUMULATIVE_ERROR)
		ctrl.position.error_sum = -MAX_CUMULATIVE_ERROR;
	
	//Proportional term
	p = (ctrl.position.gain.P_KP * ctrl.position.error) / 100;
	//Integral term
	i = (ctrl.position.gain.P_KI * ctrl.position.error_sum) / 100;
	
	//Output
	pwm = (p + i);		//
	
	//Saturates PWM to low values
	if(pwm >= POS_PWM_LIMIT)
		pwm = POS_PWM_LIMIT;
	if(pwm <= -POS_PWM_LIMIT)
		pwm = -POS_PWM_LIMIT;
	
	motor_open_speed_1(pwm);
	
	return ctrl.position.error;
}

//PI Current controller
//'wanted_curr' is centered at zero and is in the ±CURRENT_SPAN range
//'measured_curr' is also centered at 0
//The sign of 'wanted_curr' will change the rotation direction, not the polarity of the current (I have no control on this)
int32 motor_current_pid(int32 wanted_curr, int32 measured_curr)
{
	int curr_p = 0, curr_i = 0;
	int curr_pwm = 0;
	int sign = 0;
	unsigned int uint_wanted_curr = 0;
	int motor_current = 0;
	int32 shifted_measured_curr = 0;
	
	//Clip out of range values
	if(wanted_curr >= CURRENT_POS_LIMIT)
		wanted_curr = CURRENT_POS_LIMIT;
	if(wanted_curr <= CURRENT_NEG_LIMIT)
		wanted_curr = CURRENT_NEG_LIMIT;
		
	ctrl.current.setpoint_val = wanted_curr;
	
	//Sign extracted from wanted_curr:
	if(wanted_curr < 0)
	{
		sign = -1;
		uint_wanted_curr = -wanted_curr;
	}
	else
	{
		sign = 1;
		uint_wanted_curr = wanted_curr;
	}
	
	//At this point 'uint_wanted_curr' is always a positive value.
	//This is our setpoint.
	
	//From ADC value to motor current:
	shifted_measured_curr = measured_curr + CURRENT_ZERO;
	if(shifted_measured_curr <= CURRENT_ZERO)
	{
		//We are driving the motor (Q1 or Q3)
		motor_current = CURRENT_ZERO - shifted_measured_curr;
	}
	else
	{
		motor_current =  shifted_measured_curr - CURRENT_ZERO;
	}
	//ToDo above code seems complex for no valid reason
	
	//At this point 'motor_current' is always a positive value.
	//This is our measured value.
	
	//Error and integral of errors:
	ctrl.current.error = uint_wanted_curr - motor_current;					//Actual error
	ctrl.current.error_sum = ctrl.current.error_sum + ctrl.current.error;	//Cumulative error
	
	//Saturate cumulative error
	if(ctrl.current.error_sum >= MAX_CUMULATIVE_ERROR)
		ctrl.current.error_sum = MAX_CUMULATIVE_ERROR;
	if(ctrl.current.error_sum <= -MAX_CUMULATIVE_ERROR)
		ctrl.current.error_sum = -MAX_CUMULATIVE_ERROR;
	
	//Proportional term
	curr_p = (int) ctrl.current.gain.I_KP * ctrl.current.error;
	//Integral term
	curr_i = (int)(ctrl.current.gain.I_KI * ctrl.current.error_sum)/100;
	//Add differential term here if needed
	
	//Output
	curr_pwm = curr_p + curr_i;
	
	//Saturates PWM
	if(curr_pwm >= POS_PWM_LIMIT)
		curr_pwm = POS_PWM_LIMIT;
	if(curr_pwm <= 0)	//Should not happen
		curr_pwm = 0;
	
	//Apply PWM
	motor_open_speed_2(curr_pwm, sign);
	
	return ctrl.current.error;
}

//PI Current controller #2: speed optimized
//'wanted_curr' & 'measured_curr' are centered at zero and are in the ±CURRENT_SPAN range
//The sign of 'wanted_curr' will change the rotation direction, not the polarity of the current (I have no control on this)
inline int32 motor_current_pid_2(int32 wanted_curr, int32 measured_curr)
{
	volatile int32 curr_p = 0, curr_i = 0;
	volatile int32 curr_pwm = 0;
	int32 sign = 0;
	int32 uint_wanted_curr = 0;
	int32 motor_current = 0;
	int32 shifted_measured_curr = 0;
	
	//Clip out of range values
	if(wanted_curr >= CURRENT_POS_LIMIT)
		wanted_curr = CURRENT_POS_LIMIT;
	if(wanted_curr <= CURRENT_NEG_LIMIT)
		wanted_curr = CURRENT_NEG_LIMIT;		
	ctrl.current.setpoint_val = wanted_curr;
	
	//Sign extracted from wanted_curr:
	if(wanted_curr < 0)
	{
		sign = -1;
		MotorDirection_Control = 0;		//MotorDirection_Write(0);
		uint_wanted_curr = -wanted_curr;
	}
	else
	{
		sign = 1;
		MotorDirection_Control = 1;		//MotorDirection_Write(1);
		uint_wanted_curr = wanted_curr;
	}
	
	//At this point 'uint_wanted_curr' is always a positive value.
	//This is our setpoint.
	
	//From ADC value to motor current:
	shifted_measured_curr = measured_curr + CURRENT_ZERO;
	if(shifted_measured_curr <= CURRENT_ZERO)
	{
		//We are driving the motor (Q1 or Q3)
		motor_current = CURRENT_ZERO - shifted_measured_curr;
	}
	else
	{
		motor_current =  shifted_measured_curr - CURRENT_ZERO;
	}
	//ToDo above code seems complex for no valid reason
	
	//At this point 'motor_current' is always a positive value.
	//This is our measured value.
	
	//Error and integral of errors:
	ctrl.current.error = uint_wanted_curr - motor_current;					//Actual error
	ctrl.current.error_sum = ctrl.current.error_sum + ctrl.current.error;	//Cumulative error
	
	//Saturate cumulative error
	if(ctrl.current.error_sum >= MAX_CUMULATIVE_ERROR)
		ctrl.current.error_sum = MAX_CUMULATIVE_ERROR;
	if(ctrl.current.error_sum <= -MAX_CUMULATIVE_ERROR)
		ctrl.current.error_sum = -MAX_CUMULATIVE_ERROR;
	
	//Proportional term
	curr_p = (int) (ctrl.current.gain.I_KP * ctrl.current.error) / 100;
	//Integral term
	curr_i = (int)(ctrl.current.gain.I_KI * ctrl.current.error_sum) / 100;
	//Add differential term here if needed
	//In both cases we divide by 100 to get a finer gain adjustement w/ integer values.
	
	//Output
	curr_pwm = curr_p + curr_i;
	
	//Saturates PWM
	if(curr_pwm >= POS_PWM_LIMIT)
		curr_pwm = POS_PWM_LIMIT;
	if(curr_pwm <= 0)	//Should not happen
		curr_pwm = 0;
	
	//Apply PWM
	//motor_open_speed_2(curr_pwm, sign);
	//Integrated to avoid a function call and a double saturation:
	
	//Write duty cycle to PWM module (avoiding double function calls)
	curr_pwm = PWM1DC(curr_pwm);
	CY_SET_REG16(PWM_1_COMPARE1_LSB_PTR, (uint16)curr_pwm);					//PWM_1_WriteCompare1((uint16)curr_pwm);
	CY_SET_REG16(PWM_1_COMPARE2_LSB_PTR, (uint16)(PWM2DC(curr_pwm)));	//PWM_1_WriteCompare2((uint16)((curr_pwm >> 1) + 1));	
	//Compare 2 can't be 0 or the ADC won't trigger => that's why I'm adding 1
	
	//ToDo: where's the sign applied???
	
	return ctrl.current.error;
}

//Use this function to change the control strategy
void control_strategy(unsigned char strat)
{
	//By default we place the gains to 0 before we change the strategy:
					
	//Position controller
	ctrl.position.gain.P_KP = 0;
	ctrl.position.gain.P_KI = 0;
	ctrl.position.gain.P_KD = 0;
	
	//Impedance controller
	ctrl.impedance.gain.Z_K = 0;
	ctrl.impedance.gain.Z_B = 0;
	ctrl.impedance.gain.Z_I = 0;
	
	//Current controller
	ctrl.current.gain.I_KP = 0;
	ctrl.current.gain.I_KI = 0;
	ctrl.current.gain.I_KD = 0;
	ctrl.current.setpoint_val = 0;
	
	//To avoid a huge startup error on the Position-based controllers:
	if(strat == CTRL_POSITION)
	{
		ctrl.position.setp = QuadDec_1_GetCounter(); //adc1_res_filtered[0];	//ToDo make this flexible
		steps = trapez_gen_motion_1(ctrl.position.setp, ctrl.position.setp, 1, 1);
	}
	else if(strat == CTRL_IMPEDANCE)
	{
		ctrl.impedance.setpoint_val = QuadDec_1_GetCounter();
		steps = trapez_gen_motion_1(ctrl.impedance.setpoint_val, ctrl.impedance.setpoint_val, 1, 1);
	}
	
	ctrl.active_ctrl = strat;	//controller = strat;
	
	//The user should call a set gain function at this point.
}

// Impedance controller -- EJ Rouse, 8/11/14
// There will be filter transients for first few iterations -- maybe turn loops off for 100 ms?
// Variables created: stiffness, damping, prev_enc_count, 
int motor_impedance_encoder(int wanted_pos, int new_enc_count)
{
	// Initialize vars
	int i_k = 0, i_b = 0;	
	static long long prev_enc_count = 0, prev_vel1 = 0, prev_vel2 = 0, prev_vel3 = 0;
	static long long prev_filt1 = 0, prev_filt2 = 0, prev_filt3 = 0; 
	long long current_vel = 0, filt_vel = 0, current_val = 0;
	static int enc_t0 = 0, enc_tm1 = 0, enc_tm2 = 0, enc_tm3 = 0, enc_tm4 = 0, enc_tm5 = 0, enc_tm6 = 0, enc_tm7 = 0, enc_tm8 = 0, enc_tm9 = 0;

	ctrl.impedance.error = new_enc_count - wanted_pos;		//Actual error
	
	//Current for stiffness term
	i_k = ctrl.impedance.gain.Z_K * (ctrl.impedance.error >> 12);	//The /50 places the k gain in a good integer range
	
	//Velocity measured on n cycles:
	enc_tm9 = enc_tm8; enc_tm8 = enc_tm7; enc_tm7 = enc_tm6; enc_tm6 = enc_tm5; enc_tm5 = enc_tm4; enc_tm4 = enc_tm3; enc_tm3 = enc_tm2; enc_tm2 = enc_tm1; enc_tm1 = enc_t0;
	enc_t0 = new_enc_count;
	
	//Damping term
	//Difference velocity
	//current_vel = (enc_t0 - enc_tm4);
	//debug_var = current_vel;

 	filt_vel = (57*enc_t0)/220 + (73*enc_tm1)/660 - enc_tm2/264 - (37*enc_tm3)/440 - (43*enc_tm4)/330 - (47*enc_tm5)/330 - (53*enc_tm6)/440 - (17*enc_tm7)/264 + (17*enc_tm8)/660 + (3*enc_tm9)/20; // Derivative method that estimates best fit second order polynomial and calcualtes the derivative numerically for t = 0
 	debug_var = filt_vel;
	
 	i_b = ctrl.impedance.gain.Z_B * (filt_vel >> 5);

	//Output
	current_val = (i_k + i_b);		// Impedance command, motor current in terms of effort (A)
	
	// Changes variables for derivative filtering
	prev_enc_count = new_enc_count;
	prev_vel3 = prev_vel2;
	prev_vel2 = prev_vel1;
	prev_vel1 = current_vel;
	prev_filt3 = prev_filt2;
	prev_filt2 = prev_filt1;
	prev_filt1 = filt_vel;

	ctrl.current.setpoint_val = (int32)current_val;

	return ctrl.impedance.error;
}

//Initializes all the variable
void init_motor(void)
{
	//Variables:
	//=-=-=-=-=-=
	
	init_motor_data_structure();
	
	//Peripherals:
	//=-=-=-=-=-=
	
	//PWM1: BLDC
	PWM_1_Start();
	PWM_1_WriteCompare1(0);	//Start at 0%
	PWM_1_WriteCompare2(PWM2DC(0));	
	
	//Default is Brake mode:
	Coast_Brake_Write(1);
	
	//ADC2: Motor current
	ADC_SAR_2_Start();
	ADC_SAR_2_IRQ_Enable();
	adc_dma_config();
	isr_dma_Start();
	
	//VDAC8: OpAmp VREF
	VDAC8_1_Start();
	
	//Analog amplifiers & multiplexer(s):
	Opamp_1_Start();
	
	//Quadrature 1: Motor shaft encoder
	#ifdef USE_QEI1
	QuadDec_1_Start();
	QuadDec_1_Enable();
	QuadDec_1_SetCounter(QUAD1_INIT);
	#endif	//USE_QEI1	
}

//Starts all the parameters at 0 or Default values
void init_motor_data_structure(void)
{
	//No active controller:
	ctrl.active_ctrl = CTRL_NONE;
	
	//Generic controller
	ctrl.generic.gain.g0 = 0;
	ctrl.generic.gain.g1 = 0;
	ctrl.generic.gain.g2 = 0;
	ctrl.generic.gain.g3 = 0;
	ctrl.generic.gain.g4 = 0;
	ctrl.generic.gain.g5 = 0;
	ctrl.generic.actual_val = 0;
	ctrl.generic.setpoint_val = 0;
	ctrl.generic.error = 0;
	ctrl.generic.error_sum = 0;
	ctrl.generic.error_dif = 0;
	
	//Position controller
	ctrl.position.gain.g0 = 0;
	ctrl.position.gain.g1 = 0;
	ctrl.position.gain.g2 = 0;
	ctrl.position.gain.g3 = 0;
	ctrl.position.gain.g4 = 0;
	ctrl.position.gain.g5 = 0;
	ctrl.position.pos = 0;
	ctrl.position.setp = 0;
	ctrl.position.error = 0;
	ctrl.position.error_sum = 0;
	ctrl.position.error_dif = 0;
	
	//Impedance controller
	ctrl.impedance.gain.g0 = 0;
	ctrl.impedance.gain.g1 = 0;
	ctrl.impedance.gain.g2 = 0;
	ctrl.impedance.gain.g3 = 0;
	ctrl.impedance.gain.g4 = 0;
	ctrl.impedance.gain.g5 = 0;
	ctrl.impedance.actual_val = 0;
	ctrl.impedance.setpoint_val = 0;
	ctrl.impedance.error = 0;
	ctrl.impedance.error_sum = 0;
	ctrl.impedance.error_dif = 0;
	
	//Current controller
	ctrl.current.gain.g0 = 0;
	ctrl.current.gain.g1 = 0;
	ctrl.current.gain.g2 = 0;
	ctrl.current.gain.g3 = 0;
	ctrl.current.gain.g4 = 0;
	ctrl.current.gain.g5 = 0;
	ctrl.current.actual_val = 0;
	ctrl.current.setpoint_val = 0;
	ctrl.current.error = 0;
	ctrl.current.error_sum = 0;
	ctrl.current.error_dif = 0;
}

//Copy of the test code used in main.c to test the hardware:
void motor_fixed_pwm_test_code_blocking(int spd)
{
	ctrl.active_ctrl = CTRL_OPEN;	
	Coast_Brake_Write(1);	//Brake
	motor_open_speed_1(spd);
	
	while(1)
	{	
		LED_R_Write(H1_Read());
		LED_G_Write(H2_Read());
		LED_B_Write(H3_Read());
		
		encoder_read();
	}
}

//Updates the structure with the latest encoder value
int32 encoder_read(void)
{
	//Count: actual, last, difference
	encoder.count_last = encoder.count;
	encoder.count = QuadDec_1_GetCounter();
	encoder.count_dif = encoder.count - encoder.count_last;
	
	//For the position & impedance controllers we use the last count
	ctrl.position.pos = encoder.count;
	ctrl.impedance.actual_val = encoder.count;
	
	return encoder.count;
}
//Warning: encoder.count seems to be interpreted as a uint... casting (int32) before using it works.

//Updates the structure with the desired value and write it to the encoder
void encoder_write(int32 enc)
{
	encoder.count = enc;
	QuadDec_1_SetCounter(enc);
}

//Can we cancel the damping by changing the Hall direction based on the encoder? Yes.
void motor_cancel_damping_test_code_blocking(void)
{
	ctrl.active_ctrl = CTRL_OPEN;	
	Coast_Brake_Write(1);	//Brake
	motor_open_speed_1(0);
	
	while(1)
	{	
		//RGB LED = Hall code:
		LED_R_Write(H1_Read());
		LED_G_Write(H2_Read());
		LED_B_Write(H3_Read());
		
		//Refresh encoder data:
		encoder.count_last = encoder.count;	
		encoder.count = QuadDec_1_GetCounter();
		encoder.count_dif = encoder.count - encoder.count_last;
		
		//Act based on the sign:
		if(encoder.count_dif >= 0)
		{
			MotorDirection_Write(0);
		}
		else
		{
			MotorDirection_Write(1);
		}		
		
		//Loop delay (otherwise we don't get a good difference)
		CyDelay(10);
	}
}
