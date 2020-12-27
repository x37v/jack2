#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <jack/transport.h>

jack_client_t *client;

static int repeats = 1;
static double bpm = 120.0;

void setBPM(jack_nframes_t frame) {
  jack_position_t pos;
  jack_transport_query(client, &pos);
  pos.frame += frame;
  if (!(pos.valid & JackPositionBBT)) {
    printf("bbt not valid\n");
    pos.valid = JackPositionBBT;
    pos.bar = 1;
    pos.beat = 1;
    pos.tick = 0;
    pos.bar_start_tick = 0;
    pos.beats_per_bar = 4.0;
    pos.beat_type = 4.0;
    pos.ticks_per_beat = 1920.0;
  }
  pos.beats_per_minute = bpm;
  jack_transport_reposition(client, &pos);
  printf("set bpm %f\n", bpm);
}

static int processCallback(jack_nframes_t nframes, void *arg) {
  setBPM(nframes);
  repeats -= 1;
  return repeats > 0 ? 0 : 1;
}

int main (int argc, char *argv[]) {
  int version = atoi(argv[1]);
  bpm = atoi(argv[2]);

	if ((client = jack_client_open ("transset", JackNullOption, NULL)) == 0) {
		fprintf (stderr, "JACK server not running?\n");
		exit (1);
	}

  printf("starting transport\n");
  jack_transport_start(client);
  sleep(1);

  if (version == 0) {
    printf("main thread\n");
    setBPM(0);
    jack_client_close(client);
    sleep(1);
    return 0;
  }
  if (version == 1)
    repeats = 1;
  else
    repeats = 2;
  printf("audio thread %d repeats\n", repeats);

  jack_set_process_callback(client, processCallback, NULL);
  jack_activate(client);

  while (repeats > 0) {
    usleep(10);
  }
  usleep(100);
  jack_deactivate(client);
  return 0;
}
