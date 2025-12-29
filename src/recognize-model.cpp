#include "recognize-model.h"

#include <stdexcept>

RecognizeModel::RecognizeModel(const char* model)
	 : ctx_params_{/*.use_gpu              =*/false,
						/*.flash_attn           =*/false,
						/*.gpu_device           =*/0,

						/*.dtw_token_timestamps =*/false,
						/*.dtw_aheads_preset    =*/WHISPER_AHEADS_NONE,
						/*.dtw_n_top            =*/-1,
						/*.dtw_aheads           =*/
						{
							 /*.n_heads          =*/0,
							 /*.heads            =*/NULL,
						},
						/*.dtw_mem_size         =*/1024 * 1024 * 128},
		ctx_(whisper_init_from_file_with_params(model, ctx_params_)),
		params_(whisper_full_default_params(WHISPER_SAMPLING_GREEDY)) {
	if (!ctx_) {
		throw std::runtime_error("Failed to initialize whisper context");
	}
	params_.print_progress = false;
	params_.print_realtime = false;
	params_.print_timestamps = false;
	params_.print_special = false;
	// params_.translate = false;
	// params_.no_context = false;
	params_.language = "ru";
	params_.n_threads = 6;
}

RecognizeModel::~RecognizeModel() {
	whisper_free(ctx_);
}

std::string RecognizeModel::RecognizeAudio(std::vector<float> buf) {
	int ret = whisper_full(ctx_, params_, buf.data(), (int)buf.size());
	if (ret != 0) {
		return "Error";
	}

	int n_segments = whisper_full_n_segments(ctx_);
	for (int i = 0; i < n_segments; ++i) {
		const char* text = whisper_full_get_segment_text(ctx_, i);
		return text;
	}

	return "";
}
