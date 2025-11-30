#!/bin/bash

# Проверяем наличие ffmpeg
if ! command -v ffmpeg &> /dev/null; then
    echo "Ошибка: ffmpeg не найден. Установите ffmpeg и попробуйте снова."
    exit 1
fi

# Переименовываем файлы 1.wav -> 31.wav в *_old.wav
for i in {1..31}; do
    if [ -f "${i}.wav" ]; then
        mv "${i}.wav" "${i}_old.wav"
        echo "Переименован: ${i}.wav -> ${i}_old.wav"
    else
        echo "Предупреждение: файл ${i}.wav не найден"
    fi
done

# Конвертируем обратно с помощью ffmpeg
for i in {1..31}; do
    old_file="${i}_old.wav"
    new_file="${i}.wav"
    
    if [ -f "$old_file" ]; then
        ffmpeg -i "$old_file" -ar 16000 -ac 1 -c:a pcm_s16le "$new_file" -y
        if [ $? -eq 0 ]; then
            echo "Конвертирован: $old_file -> $new_file"
            # Удаляем старый файл после успешной конвертации
            rm "$old_file"
        else
            echo "Ошибка конвертации: $old_file"
        fi
    fi
done

echo "Обработка завершена!"
