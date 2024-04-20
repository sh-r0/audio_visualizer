#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <optional>
#include <set>
#include <filesystem>
#include <thread>
#define _USE_MATH_DEFINES
#include <cmath>
#include <complex.h>
#include <cstdlib>
#include <numeric>
#include "renderer.hpp"
#define MINIAUDIO_IMPLEMENTATION
#include "external/miniaudio.h"

typedef std::complex<float> cFloat;
const size_t arrSize = 1<<14;
float* rawSamples = nullptr, * inSamples = nullptr;
cFloat* outSamples = nullptr;
renderer_t* _renderer = nullptr;

struct audioVars {
    ma_decoder decoder;
    ma_device device;
};

inline float getAmp(cFloat _a) {
    float a = _a.real();
    float b = _a.imag();
    return std::log(a*a + b*b);
}

bool arrInUse = false;
inline void pushSamples(float* _samples, size_t _count) {
    if (!arrInUse) {
        memmove(rawSamples, rawSamples + _count, (arrSize - _count)*sizeof(float));
        memcpy(rawSamples + arrSize - _count, _samples, _count * sizeof(float));
    }
    return;
}

static void fft(float _in[], size_t _stride, cFloat _out[], size_t _n) {
    assert(_n > 0);
    if (_n == 1) {
        _out[0] = cFloat(_in[0]);
        return;
    }

    fft(_in, _stride * 2, _out, _n / 2);
    fft(_in + _stride, _stride * 2, _out + _n / 2, _n / 2);

    for (size_t k = 0; k < _n / 2; ++k) {
        float t = (float)k / _n;
        cFloat v = std::exp(cFloat(0, -2 * M_PI * t)) * _out[k + _n / 2];
        cFloat e = _out[k];
        _out[k] = e + v;
        _out[k + _n / 2] = e - v;
    }
    return;
}

void dataCallback(ma_device* _pDevice, void* _pOutput, const void* _pInput, ma_uint32 _frameCount) {
	ma_decoder* pDecoder = (ma_decoder*)_pDevice->pUserData;
	if (pDecoder == NULL) 
		return;
    
	ma_decoder_read_pcm_frames(pDecoder, _pOutput, _frameCount, NULL);
    float(*fs)[2] = reinterpret_cast<float(*)[2]>(_pOutput);
    float* tmp = reinterpret_cast<float*>(malloc(_frameCount * sizeof(float)));
    for (uint32_t i = 0; i < _frameCount; i++)
        tmp[i] = fs[i][0];
    pushSamples(tmp, _frameCount);
    free(tmp);

	(void)_pInput;
    return;
}

float hannVals[arrSize] = {};
float outSmooth[barsCount] = {};
void fftAudioSamples() {
    while (arrInUse) __noop;
    arrInUse = true;
    //apply hann window
    for (size_t i = 0; i < arrSize; ++i) 
        inSamples[i] = rawSamples[i] * hannVals[i];
    arrInUse = false;

    fft(inSamples, 1, outSamples, arrSize);
    //var set experimentally such that screen can be filled with bars
    const float step = 1.023f;
    float lowFreq = 1.0f;
    size_t m = 0;
    float max_amp = 1.0f;
    float outLog[barsCount] = {};
    for (float freq = lowFreq; (size_t)freq < arrSize / 2; freq = ceilf(freq * step)) {
        float f1 = ceilf(freq * step);
        float currAmp = 0.0f;
        for (size_t i = (size_t)freq; i < arrSize / 2 && i < (size_t)f1; ++i) {
            float _amp = getAmp(outSamples[i]);
            if (_amp > currAmp) currAmp = _amp;
        }
        if (max_amp < currAmp) max_amp = currAmp;
        assert(m <= barsCount);
        outLog[m++] = currAmp;
    }

    for (size_t i = 0; i < m; ++i) {
        const float smoothness = 0.25;
        outSmooth[i] += (outLog[i]/max_amp - outSmooth[i]) * smoothness;
    }

    for (size_t i = 0; i < 2 && i < m; i++) {
        _renderer->heights[i] = outSmooth[i];
        _renderer->heights[barsCount - i - 1] = outSmooth[barsCount - i - 1];
    }
    for (size_t i = 2; i < m; i++) {
        const float vals[3] = { 0.075, 0.175, 0.25 };
        float res = 0;
        for (size_t j = 0; j < 3; j++)
            res += vals[j] * (outSmooth[i - 2 + j] + outSmooth[i + 2 - j]);
        _renderer->heights[i] = res;
    }
    return;
}

bool initAudio(audioVars& _audio, const char* _audioPath) {
    ma_result result;
    ma_decoder_config decoderConfig{};
    decoderConfig.channels = 2;
    decoderConfig.format = ma_format_f32;

    result = ma_decoder_init_file(_audioPath, &decoderConfig, &_audio.decoder);
    if (result != MA_SUCCESS) {
        return false;
    }

    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = _audio.decoder.outputFormat;
    deviceConfig.playback.channels = _audio.decoder.outputChannels;
    deviceConfig.sampleRate = _audio.decoder.outputSampleRate;
    deviceConfig.dataCallback = dataCallback;
    deviceConfig.pUserData = &_audio.decoder;

    if (ma_device_init(NULL, &deviceConfig, &_audio.device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&_audio.decoder);
        return false;
    }
    float vol; ma_device_get_master_volume(&_audio.device, &vol);
    ma_device_set_master_volume(&_audio.device, vol / 20);
    if (ma_device_start(&_audio.device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&_audio.device);
        ma_decoder_uninit(&_audio.decoder);
        return false;
    }

    return true;
}

inline void allocArrs(void) {
    rawSamples = (float*)malloc(arrSize * sizeof(float));
    inSamples = (float*)malloc(arrSize * sizeof(float));
    outSamples = (cFloat*)malloc(arrSize * sizeof(cFloat));
    for (size_t i = 0; i < arrSize; ++i) {
        float t = (float)i / (arrSize - 1);
        hannVals[i] = 0.5 - 0.5 * std::cos(2 * M_PI * t);
    }

    return;
}

int32_t main(int argc, char** argv) {
    if (argc < 2) {
        printf("Incorrect calling convention!\nShould be ./audio_visualizer.exe <filepath>\n");
        return EXIT_FAILURE;
    }
    if (!std::filesystem::exists(std::filesystem::path(argv[1]))) {
        printf("Could not find requested audio file!\n");
        return EXIT_FAILURE;
    }

	renderer_t renderer;
	renderer.initRenderer();
    _renderer = &renderer;
    allocArrs();

    audioVars audio{};

    if (!initAudio(audio, argv[1])) {
        renderer.cleanup(); 
        std::fprintf(stderr, "Could not initialize audio device!\n");
        return EXIT_FAILURE; 
    };
    
    std::chrono::time_point<std::chrono::steady_clock> startFrame = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(renderer.window)) {
        fftAudioSamples();
		renderer.drawFrame();
		glfwPollEvents();
	}
    
    ma_device_uninit(&audio.device);
    ma_decoder_uninit(&audio.decoder);
	renderer.cleanup();
	return EXIT_SUCCESS;
}