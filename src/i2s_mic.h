#ifndef I2S_MIC_H
#define I2S_MIC_H

extern struct k_msgq beat_msgq;

struct beat_msg {
    uint32_t event_code;  // e.g., 1 for beat
    float energy;         // From beat_detector
};

void i2s_mic_thread(void);

#endif /* I2S_MIC_H */