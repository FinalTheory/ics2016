#include <SDL.h>

void init_monitor(int, char *[]);
void reg_test();
void restart(int, char *[]);
void ui_mainloop();
void cpu_exec(uint32_t);


int main(int argc, char *argv[]) {
  int i;
  int interactive = 0;

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the `CPU_state' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart(argc, argv);

  /* Check flags */
  for (i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0) {
      interactive = 1;
      break;
    }
  }

  if (interactive) {
    /* Receive commands from user. */
    ui_mainloop();
  } else {
    cpu_exec(-1);
  }

	return 0;
}
