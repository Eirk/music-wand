#ifndef _BEAT_DETECTOR_H_
#define _BEAT_DETECTOR_H_

#define BASE_THRESHOLD   1.0f   /* nominal multiplier */
#define VARIANCE_SCALE   2.0f   /* how strongly variance affects the threshold */
#define BEAT_DETECTOR_HISTORY_SIZE 43 /* 43 is the number of frames in 1 second at 1024 samples/frame and 44100 samples/second */

typedef struct {
    float history[BEAT_DETECTOR_HISTORY_SIZE];
    float *history_ptr;
    float energy;
    float variance;
    float average_energy;
    float threshold;
} BeatDetector;

BeatDetector * beat_detector_create(void);
void beat_detector_init(BeatDetector *detector);
void beat_detector_update(BeatDetector *detector, const float* samples, int num_samples);
int beat_detector_is_beat(const BeatDetector *detector);
void beat_detector_destroy(BeatDetector *detector);

#endif // _BEAT_DETECTOR_H_