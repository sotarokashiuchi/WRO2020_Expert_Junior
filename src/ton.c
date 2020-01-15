#include "ev3api.h"
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

void run_task(void) {

	ev3_speaker_set_volume(10);

	while (1) {		
		ev3_speaker_play_tone(NOTE_G4, 1000);
		tslp_tsk(TAIME);

		ev3_speaker_play_tone(NOTE_C5, 1000);
		tslp_tsk(TAIME);
	}

}