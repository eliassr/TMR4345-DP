//Shared header file for entire program

//Suppress some unwanted compiler warnings
#define _CRT_SECURE_NO_WARNINGS

//Include the necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <io.h>
#include <phidget22.h>

//Define key parameters

//Voltage difference between each end of pool
#define voltage_diff 0.220

//Input limits for servo
#define max_input 160
#define min_input 110

//A struct containing the data from input-file/user-input
typedef struct {
	int max_time;
	double target_pos;
	double setpoint;
	int Kp;
	int Kd;
	int Ki;
} data;

//Subroutines

void activate_phidgets(PhidgetVoltageRatioInputHandle* ch_V, PhidgetRCServoHandle* ch_S);
void deactivate_phidgets(PhidgetVoltageRatioInputHandle* ch_V, PhidgetRCServoHandle* ch_S);
data user_interface();
void read_input(data* d);
void user_input(data* d);
double get_deviation(PhidgetVoltageRatioInputHandle* ch, double ref_pos);
double clamp(double val, double max_val, double min_val);
double PID(double dev, double dt, int Kp, int Kd, int Ki);
apply_input(double input, PhidgetRCServoHandle* ch);
double get_pos(PhidgetVoltageRatioInputHandle* ch);
void make_vtf(double ref);
