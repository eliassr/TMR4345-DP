//Contains all the functions and subroutines we need for the program.

#include "Header.h"



/*
Function user_interface

A simple user interface to start off the program and let user decide
the input method.
*/

data user_interface() {
	data d;
	int choice;
	int ready;
	printf("--- Dynamic Positioning interface ---\n\n\n");
	do {
		printf("Press 1 to use input values from input-file.\n");
		printf("Press 2 to input values yourself.\n");
		printf("Press 3 to close the program.\n");
		scanf("%d", &choice);

		if (choice == 1) { read_input(&d); }
		else if (choice == 2) { user_input(&d); }
		else if (choice == 3) { exit(0);	}
	} while (choice != 1 && choice != 2 && choice != 3);
	do {
		printf("Make sure the boat is positioned correctly and type 1 to start\n");
		scanf("%d", &ready);
	} while (ready != 1);
	return d;
}




/*
Function: read_input

Reads key parameters from .txt file in the directory
*/

void read_input(data *d) {
	FILE *f;
	f = fopen("input.txt", "r");

	if (!f) {
		printf("Error: input.txt could not be opened.");
		exit(1);
	}

	fscanf(f, "%d \n %lf \n %d \n %d \n %d", &(d->max_time), &(d->setpoint), &(d->Kp), &(d->Kd), &(d->Ki));
	fclose(f);
}



/*
Function: user_input

Lets the user input parameters themselves
*/

void user_input(data* d) {
	printf("Select max runtime for system (seconds)");
	scanf("%d", &d->max_time);
	printf("Select target position");
	scanf("%lf", &d->setpoint);
	printf("Input PID gain constants\n");
	printf("Kp:");
	scanf("%d", &d->Kp);
	printf("Kd:");
	scanf("%d", &d->Kd);
	printf("Ki:");
	scanf("%d", &d->Ki);
}




/*
Function: activate_phidgets

Starts up the phidgets at the beginning of the program
*/

void activate_phidgets(PhidgetVoltageRatioInputHandle *ch_V, PhidgetRCServoHandle *ch_S) {

	PhidgetRCServo_create(ch_S);
	Phidget_setDeviceSerialNumber((PhidgetHandle)*ch_S, 42685);
	Phidget_setChannel((PhidgetHandle)*ch_S, 0);
	Phidget_openWaitForAttachment((PhidgetHandle)*ch_S, 5000);
	PhidgetRCServo_setTargetPosition(*ch_S, 0);
	PhidgetRCServo_setEngaged(*ch_S, 1);

	PhidgetVoltageRatioInput_create(ch_V);
	Phidget_setDeviceSerialNumber((PhidgetHandle)*ch_V, 85753);
	Phidget_setChannel((PhidgetHandle)*ch_V, 1);
	Phidget_openWaitForAttachment((PhidgetHandle)*ch_V, 5000);

	//Give the system some time to start up before moving on
	Sleep(1000);
	printf("Phidgets activated\n");
}



/*
Function: deactivate_phidgets

Shuts down the phidgets when the process is complete
*/

void deactivate_phidgets(PhidgetVoltageRatioInputHandle *ch_V, PhidgetRCServoHandle *ch_S) {
	PhidgetRCServo_setEngaged(*ch_S, 1);
	Phidget_close((PhidgetHandle)*ch_S);
	Phidget_delete((PhidgetHandle*)&ch_S);

	Phidget_close((PhidgetHandle)*ch_V);
	Phidget_delete((PhidgetHandle*)&ch_V);

	printf("Phidgets deactivated\n");
}



/*
Function: get_deviation

Finds the difference between chosen reference position and actual position
*/

double get_deviation(PhidgetVoltageRatioInputHandle *ch, double ref_pos) {
	double current_pos;
	PhidgetVoltageRatioInput_getVoltageRatio(*ch, &current_pos);
	double dev = ref_pos - current_pos;
	return dev;
}




/*
Function: clamp

A helper function to ensure that the applied input is within the
allowed boundaries.
*/

double clamp(double val, double max_val, double min_val) {
	if (val > max_val) { return max_val; }
	else if (val < min_val) { return min_val; }
	return val;
}




/*
Function: PID

Applies PID algorithm to find the necessary input to the motor
*/

double PID(double dev, double dt, int Kp, int Kd, int Ki) {
	static double prev_dev = 0;
	static double I = 0;
	double D = 0;
	double input = 0;

	I += (prev_dev + dev) * dt;
	if (prev_dev != 0) { D = (dev - prev_dev) / dt; }

	input = Kp * dev + Kd * D + Ki * I;
	prev_dev = dev;

	return clamp(input, max_input, min_input);
}



/*
Function: apply_input

Apply the input from the PID-controller to the servo
*/

apply_input(double input, PhidgetRCServoHandle *ch) {
	PhidgetRCServo_setTargetPosition(*ch, input);
}



/*
Function: get_pos

Collects the position of the boat
*/

double get_pos(PhidgetVoltageRatioInputHandle* ch) {
	double pos;
	PhidgetVoltageRatioInput_getVoltageRatio(*ch, &pos);
	return pos;
}



/*
Function: make_vtf

Uses the written result file to create a vtf file
The animation can be viewed in GLView
*/

void make_vtf(double ref) {

	//Open the result file and vtf file
	FILE* res = fopen("results.txt","r");
	FILE* anim = fopen("anim.vtf","w");
	if (!anim) {
		printf("Error: anim.vtf could not be opened");
		exit(1);
	}
	
	printf("Writing .vtf file\n");

	int n = 1;				//Number of steps in the result file
	double pos, dev, u, t;
	double L = 0.5;			//Dimensions of boat model
	double W = 0.2;
	double H = 0.1;

	fprintf(anim, "*VTF-1.00\n\n");

	//Pool
	fprintf(anim, "*NODES 1\n");
	fprintf(anim, "%f %f %d \n", 3 * L, 2 * W, 0);
	fprintf(anim, "%f %f %d \n", -3 * L, 2 * W, 0);
	fprintf(anim, "%f %f %d \n", -3 * L, -2 * W, 0);
	fprintf(anim, "%f %f %d \n", 3 * L, -2 * W, 0);
	fprintf(anim, "%f %f %f \n", 3 * L, 2 * W, -0.01);
	fprintf(anim, "%f %f %f \n", -3 * L, 2 * W, -0.01);
	fprintf(anim, "%f %f %f \n", -3 * L, -2 * W, -0.01);
	fprintf(anim, "%f %f %f \n", 3 * L, -2 * W, -0.01);
	fprintf(anim, "*ELEMENTS 1 \n%%NODES #1 \n%%HEXAHEDRONS\n");
	fprintf(anim, "1 2 3 4 5 6 7 8 \n\n");

	//Boat
	fprintf(anim, "*NODES 2\n");
	fprintf(anim, "%f %f %d \n", L, W, 0);
	fprintf(anim, "%f %f %d \n", L, -W, 0);
	fprintf(anim, "%f %f %d \n", -L, -W, 0);
	fprintf(anim, "%f %f %d \n", -L, W, 0);
	fprintf(anim, "%f %f %f \n", L, W, H);
	fprintf(anim, "%f %f %f \n", L, -W, H);
	fprintf(anim, "%f %f %f \n", -L, -W, H);
	fprintf(anim, "%f %f %f \n", -L, W, H);
	fprintf(anim, "*ELEMENTS 2 \n%%NODES #2 \n%%HEXAHEDRONS\n");
	fprintf(anim, "1 2 3 4 5 6 7 8 \n\n");

	//Setpoint, shadow of boat
	fprintf(anim, "*NODES 3\n");
	fprintf(anim, "%lf %f %d \n", ref - L, W, 0);
	fprintf(anim, "%lf %f %d \n", ref - L, -W, 0);
	fprintf(anim, "%lf %f %d \n", ref + L, -W, 0);
	fprintf(anim, "%lf %f %d \n", ref + L, W, 0);
	fprintf(anim, "%lf %f %f \n", ref - L, W, H);
	fprintf(anim, "%lf %f %f \n", ref - L, -W, H);
	fprintf(anim, "%lf %f %f \n", ref + L, -W, H);
	fprintf(anim, "%lf %f %f \n", ref + L, W, H);
	fprintf(anim, "*ELEMENTS 3 \n%%NODES #3 \n%%HEXAHEDRONS\n");
	fprintf(anim, "1 2 3 4 5 6 7 8 \n\n");

	fprintf(anim, "*GLVIEWGEOMETRY 1 \n%%ELEMENTS \n1 2 3 \n\n");

	int k;
	double err;
	while ((k = fgetc(res)) != EOF) {
		fscanf(res, "%lf %lf %lf %lf \n", &pos, &dev, &u, &t);
		err = pos - ref;
		fprintf(anim, "*RESULTS %i \n%%DIMENSION 1 \n%%PER_NODE #2 \n", n);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		fprintf(anim, "%lf %d %d\n", err, 0, 0);
		n++;
	}

	fprintf(anim, "*GLVIEWVECTOR 1\n%%NAME Displacement\n");
	for (int i = 1; i < n; i++) {
		fprintf(anim, "%%STEP %i \n%i\n", i, i);
	}

	fclose(res);
	fclose(anim);

	printf("Finished writing to vtf file.\n");


}
