#include "tonegen.h"
#include <AudioToolbox/AudioToolbox.h>
#include <math.h>
#include <unistd.h> // For usleep
#include <string.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024
#define MAX_POLYPHONY 8

static AudioQueueRef audioQueue;

typedef struct {
    float freq;
    float phase;
    int active;
} ToneContext;

static ToneContext tones[MAX_POLYPHONY];

static void AQCallback(void *custom_data, AudioQueueRef inQ, AudioQueueBufferRef outQB) {
    Float32* buffer = (Float32*)outQB->mAudioData;
    memset(buffer, 0, BUFFER_SIZE * sizeof(Float32));

    for (int i = 0; i < MAX_POLYPHONY; i++) {
        if (!tones[i].active) continue;
        for (int j = 0; j < BUFFER_SIZE; j++) {
            buffer[j] += sinf(2.0f * M_PI * tones[i].freq * tones[i].phase / SAMPLE_RATE);
            tones[i].phase += 1;
        }
    }

    // Normalize output if more than one tone
    for (int j = 0; j < BUFFER_SIZE; j++) {
        buffer[j] /= MAX_POLYPHONY;
    }

    outQB->mAudioDataByteSize = BUFFER_SIZE * sizeof(Float32);
    AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
}

void tonegen_init() {
    AudioStreamBasicDescription fmt = {0};
    fmt.mSampleRate = SAMPLE_RATE;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsFloat;
    fmt.mChannelsPerFrame = 1;
    fmt.mBitsPerChannel = 32;
    fmt.mFramesPerPacket = 1;
    fmt.mBytesPerFrame = 4;
    fmt.mBytesPerPacket = 4;

    AudioQueueNewOutput(&fmt, AQCallback, NULL, NULL, NULL, 0, &audioQueue);

    for (int i = 0; i < 3; i++) {
        AudioQueueBufferRef buffer;
        AudioQueueAllocateBuffer(audioQueue, BUFFER_SIZE * sizeof(Float32), &buffer);
        AQCallback(NULL, audioQueue, buffer);
    }

    AudioQueueStart(audioQueue, NULL);
}

void start_note(float freq) {
    for (int i = 0; i < MAX_POLYPHONY; i++) {
        if (!tones[i].active) {
            tones[i].freq = freq;
            tones[i].phase = 0;
            tones[i].active = 1;
            printf("🎵 start_note: %.2f Hz\n", freq);
            return;
        }
    }
}

void stop_note(float freq) {
    for (int i = 0; i < MAX_POLYPHONY; i++) {
        if (tones[i].active && fabs(tones[i].freq - freq) < 0.01) {
            tones[i].active = 0;
            printf("🔇 stop_note: %.2f Hz\n", freq);
            return;
        }
    }
}