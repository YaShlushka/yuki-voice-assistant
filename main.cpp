#include "miniaudio.h"
#include <iostream>
#include <vector>

std::vector<int16_t> audio_buffer;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	const int16_t* input_samples = (const int16_t*)pInput;
	for (ma_uint32 i = 0; i < frameCount; i++) {
		audio_buffer.push_back(input_samples[i]);
	}
}

int main() {
	ma_device_config config = ma_device_config_init(ma_device_type_capture);
	ma_device device;

	config.capture.format = ma_format_s16; // 16 bit
	config.capture.channels = 1;				// mono
	config.sampleRate = 16000;					// 16 kHz
	config.dataCallback = data_callback;	// callback function

	if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
		return -1; // Failed to initialize the device.
	}

	ma_device_start(&device); // by default device is sleeping
	while(true) {

	}

	ma_device_uninit(&device);
	return 0;
}
