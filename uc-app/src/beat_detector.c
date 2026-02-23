#include "beat_detector.h"
#include <string.h>

void beat_detector_init(BeatDetector* detector)
{
    memset(detector->history, 0, sizeof(detector->history));
    detector->history_ptr = detector->history;
    detector->energy = 0.0f;
    detector->variance = 0.0f;
    detector->average_energy = 0.0f;
    detector->threshold = BASE_THRESHOLD; // Threshold multiplier for beat detection
}

void beat_detector_update(BeatDetector* detector, const float* samples, size_t num_samples)
{
    // Calculate energy of the current frame
    float energy = 0.0f;
    for (size_t i = 0; i < num_samples; ++i) {
        energy += samples[i] * samples[i];
    }
    energy /= num_samples;

    // Update history with the new energy value and check circular buffer
    *detector->history_ptr = energy;
    detector->history_ptr++;
    if (detector->history_ptr >= detector->history + BEAT_DETECTOR_HISTORY_SIZE) {
        detector->history_ptr = detector->history;
    }

    // Update average energy and variance
    float sum_energy = 0.0f;
    for (size_t i = 0; i < BEAT_DETECTOR_HISTORY_SIZE; ++i) {
        sum_energy += detector->history[i];
    }
    detector->average_energy = sum_energy / BEAT_DETECTOR_HISTORY_SIZE;

    float variance_sum = 0.0f;
    for (size_t i = 0; i < BEAT_DETECTOR_HISTORY_SIZE; ++i) {
        variance_sum += (detector->history[i] - detector->average_energy) * (detector->history[i] - detector->average_energy);
    }
    detector->variance = variance_sum / BEAT_DETECTOR_HISTORY_SIZE;

    // Update current energy
    detector->energy = energy;

    /* dynamically adjust the multiplier */
    /* threshold = base + scale * normalized‑variance; clamp to sane range */
    float norm_var = detector->variance / (detector->average_energy + 1e-6f);
    detector->threshold = BASE_THRESHOLD + VARIANCE_SCALE * norm_var;
    if (detector->threshold < 1.0f) detector->threshold = 1.0f;
    if (detector->threshold > 3.0f) detector->threshold = 3.0f;
}

int beat_detector_is_beat(const BeatDetector* detector)
{
    // A beat is detected if the current energy is significantly higher than the average energy
    return (detector->energy > detector->threshold * detector->average_energy);
}