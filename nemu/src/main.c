void init_monitor(int, char *[]);
void reg_test();
void restart(int, char *[]);
void ui_mainloop();

int main(int argc, char *argv[]) {

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the `CPU_state' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart(argc, argv);

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}
