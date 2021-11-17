#include "Header.h"

void main(){

	PhidgetRCServoHandle ch_S;
	PhidgetVoltageRatioInputHandle ch_V;
	double motor_input;
	double dev;
	double elapsed_time = 0;
	double dt = 0.01;

	//Open results file
	FILE *res = fopen("results.txt", "w");
	if (!res) {
		printf("Error: results.txt could not be opened.\n");
		exit(1);
	}

	data d = user_interface();
	activate_phidgets(&ch_V, &ch_S);
	d.target_pos = (d.setpoint * voltage_diff) + get_pos(&ch_V);

	do {
		clock_t t = clock();

		dev = get_deviation(&ch_V, d.target_pos);
		motor_input = PID(dev, dt, d.Kp, d.Kd, d.Ki);
		apply_input(motor_input, &ch_S);

		//Limit refresh rate, 100 operations/sec maximum
		Sleep(10);

		dt = ((double) clock() - t) / CLOCKS_PER_SEC;
		elapsed_time += dt;

		//Write to file and console
		fprintf(res, "%lf %lf %lf %lf \n", d.target_pos - dev, dev, motor_input, elapsed_time);
		printf("Thrust: %f\t Deviation: %f\t Time: %f\n", motor_input, dev, elapsed_time);
	} while (elapsed_time < d.max_time);

	deactivate_phidgets(&ch_V, &ch_S);

	fclose(res);
	make_vtf(d.target_pos);

	printf("Results can be found in results.txt and anim.vtf\n");
	printf("Program complete\n");
}
