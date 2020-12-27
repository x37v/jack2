#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <jack/transport.h>

jack_client_t *client;

void printPos(char * prefix, jack_transport_state_t state, jack_position_t *pos) {
  if (pos->valid & JackPositionBBT)
    printf("%s\n\tstate: %d\n\tbpm: %f, bar: %d beat: %d tick %d\n", prefix, state, pos->beats_per_minute, pos->bar, pos->beat, pos->tick);
  else
    printf("%s\n\tstate: %d\n\tbbt not valid\n", prefix, state);
}

double bpm = 120.0;

void timeBaseCallback(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos, void *arg) {
  if (new_pos) {
    bpm = pos->beats_per_minute;
    printPos("timeBaseCallback", state, pos);
  } else {
    pos->valid = JackPositionBBT;
    pos->bar = 1;
    pos->beat = 1;
    pos->tick = 0;
    pos->bar_start_tick = 0;
    pos->beats_per_bar = 4.0;
    pos->beat_type = 4.0;
    pos->ticks_per_beat = 1920.0;
    pos->beats_per_minute = bpm;
  }
}

int syncCallback(jack_transport_state_t state, jack_position_t *pos, void *arg) {
  printPos("syncCallback", state, pos);
  return 1;
}

int main (int argc, char *argv[]) {
	if ((client = jack_client_open ("transhow", JackNullOption, NULL)) == 0) {
		fprintf (stderr, "JACK server not running?\n");
		exit (1);
	}

  jack_set_timebase_callback(client, 0, timeBaseCallback, NULL);
  jack_set_sync_callback(client, syncCallback, NULL);
  jack_activate(client);

  while (1) {
    usleep(10);
  }
	exit (0);
}
