#include <AudioToolbox/AudioToolbox.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024
#define NUM_NOTES 24

static AudioQueueRef audioQueue;

typedef struct {
    float freq;
    float phase;
} ToneContext;

static ToneContext ctx = {440.0, 0};

static void AQCallback(void *custom_data, AudioQueueRef inQ, AudioQueueBufferRef outQB) {
    ToneContext* ctx = (ToneContext*)custom_data;
    Float32* buffer = (Float32*)outQB->mAudioData;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = sinf(2.0f * M_PI * ctx->freq * ctx->phase / SAMPLE_RATE);
        ctx->phase += 1;
    }

    outQB->mAudioDataByteSize = BUFFER_SIZE * sizeof(Float32);
    AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
}

static float midi_to_freq(uint8_t note) {
    return 440.0f * powf(2.0f, ((int)note - 69) / 12.0f);
}

int main() {
    // 24 notes: C4 (60) to B5 (83)
    uint8_t midi_notes[NUM_NOTES];
    for (int i = 0; i < NUM_NOTES; i++) {
        midi_notes[i] = 60 + i;
    }

    AudioStreamBasicDescription fmt = {0};
    fmt.mSampleRate = SAMPLE_RATE;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsFloat;
    fmt.mChannelsPerFrame = 1;
    fmt.mBitsPerChannel = 32;
    fmt.mFramesPerPacket = 1;
    fmt.mBytesPerFrame = 4;
    fmt.mBytesPerPacket = 4;

    AudioQueueNewOutput(&fmt, AQCallback, &ctx, NULL, NULL, 0, &audioQueue);

    for (int i = 0; i < 3; i++) {
        AudioQueueBufferRef buffer;
        AudioQueueAllocateBuffer(audioQueue, BUFFER_SIZE * sizeof(Float32), &buffer);
        AQCallback(&ctx, audioQueue, buffer);
    }

    AudioQueueStart(audioQueue, NULL); // 🔥 Start only once before the loop

for (int i = 0; i < NUM_NOTES; i++) {
    float freq = midi_to_freq(midi_notes[i]);
    ctx.freq = freq;
    ctx.phase = 0;

    printf("🔊 [%2d] MIDI %d → %.2f Hz\n", i, midi_notes[i], freq);
    usleep(300000); // wait 300 ms for this note to ring out
}

    AudioQueueStop(audioQueue, true);      // 🛑 Stop after all tones
    AudioQueueDispose(audioQueue, true);   // 🧼 Clean up

    AudioQueueDispose(audioQueue, true);
    return 0;
}