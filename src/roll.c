
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pulse/error.h>
#include <pulse/simple.h>

#include "log.h"

#define SAMPLES 5000
#define SAMPLE_RATE 44100
#define WAVE_AMPLITUDE 32000.0

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Converts a digit into its corresponding
 * integer value.
 */
int
note_ctoi(char c)
{
  if (c >= '0' && c <= '9') {
    return c - '0';
  }

  return 0;
}

/**
 * Read all note digits from a file.
 */
int*
read_notes(FILE* fstream, int* len)
{
  int cap = 16;
  int s = 0;

  int* notes = malloc(cap * sizeof(int));
  if (notes == NULL) {
    error("Failed to allocate memory for note digits.");
    return NULL;
  }

  char c;
  while ((c = fgetc(fstream)) != EOF) {
    int note = note_ctoi(c);
    if (note >= 0) {
      if (s >= cap) {
        cap *= 2;
        int* nm = realloc(notes, cap * sizeof(int));
        if (nm == NULL) {
          error("Failed to reallocate memory for note digits.");

          free(notes);
          return NULL;
        }
        notes = nm;
        memset(notes + s, 0, (cap - s) * sizeof(int));
      }
      notes[s++] = note;
    }
  }
  *len = s;

  return notes;
}

/**
 * Generate a frequency that's audible by humans, meant
 * to receive a number in range 0..9.
 *
 * https://en.wikipedia.org/wiki/A440_(pitch_standard)
 */
int
audible_freq(int n)
{
  return (n * 50) + 440;
}

int
main(int argc, char* argv[])
{
  pa_simple* s = NULL;
  int ret = 1;
  int err;

  if (argc < 2) {
    error("Usage: roll <file>");
    return 1;
  }

  char* fpath = argv[1];

  FILE* fstream = fopen(fpath, "r");
  if (!fstream) {
    errorf("Unable to open file '%s', check that the file exists and you have "
           "enough permissions to open it.",
           fpath);
    return 1;
  }

  int len = 0;
  int* notes = read_notes(fstream, &len);
  if (notes == NULL) {
    goto exit;
  }

  // sample format.
  pa_sample_spec ss;
  ss.format = PA_SAMPLE_S16LE;
  ss.rate = SAMPLE_RATE;
  ss.channels = 1;

  // connect to the pulseaudio daemon.
  s = pa_simple_new(NULL,
                    "roll",
                    PA_STREAM_PLAYBACK,
                    NULL,
                    "roll playback",
                    &ss,
                    NULL,
                    NULL,
                    &err);
  if (!s) {
    errorf("Failed to create a connection to the PulseAudio server: %s",
           pa_strerror(err));
    goto exit;
  }

  short buf[SAMPLES];

  // play each note.
  for (int i = 0; i < len; i++) {
    int freq = audible_freq(notes[i]);
    float c = 2 * M_PI * freq / SAMPLE_RATE;

    for (int j = 0; j < SAMPLES; j++) {
      buf[j] = WAVE_AMPLITUDE * sin(c * j);
    }

    ret = pa_simple_write(s, buf, sizeof(buf), &err);
    if (ret < 0) {
      errorf("Could not write data to the PulseAudio server: %s",
             pa_strerror(err));
      goto exit;
    }
    pa_simple_drain(s, &err);
  }

exit:
  if (s) {
    pa_simple_free(s);
  }
  return ret;
}
