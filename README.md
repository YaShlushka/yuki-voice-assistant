# yuki-voice-assistant
This project depends by whisper.cpp and miniaudio. Before compiling, copy the whisper.cpp repository here
## Recognizer variants
For speech recognition you can use whisper.cpp models. To change the model, change path to thar in main.cpp 13 line `RecognizeModel recognizer("/home/paderinee/Documents/Code/yuki-voice-assistant/models/ggml-base.bin");`
## Default applications
Voice assistant can't know your default browser, terminal for open, to change default apps, change variables in `user_default_apps.h`
