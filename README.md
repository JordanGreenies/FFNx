Fork of [FFNx](https://github.com/julianxhokaxhiu/FFNx) to add realtime TTS support to FF8 from a TTS http server

https://github.com/user-attachments/assets/11dab1b9-e84f-420b-a4fc-0c52066ce825

https://github.com/user-attachments/assets/dea5d7a6-7d67-46dc-abae-fa227e5c2242

If you want to build this then check the instructions at [FFNx](https://github.com/julianxhokaxhiu/FFNx).

## Prerequisites:
- FF8 Installed on steam with FFNx
- A locally hosted TTS server, for example: [LocalAI](https://github.com/mudler/LocalAI)
- Enough voice models for each character, all examples included in the release

## Instructions for use:
- Replace AF3DN.P1 and FFNx.toml
- Edit FFNx.toml (TTS-VOICEMOD section) to add your server and voice model settings
- Start game

New FFNx.toml config options:
```
tts_enabled = true

tts_backend_ip = "192.168.1.227"
tts_backend_port = 8693
tts_path = "/tts"
tts_backend = "piper"
tts_language = "en-us"

#First names and lowercase only
tts_voiced = ["squall", "zell", "irvine", "quistis", "rinoa", "selphie", "seifer", "edea", "laguna", "kiros", "ward", "headmaster", "gsoldier", "wedge", "biggs"]
#In the same order as tts_voiced
tts_voiced_models = ["ff8/Squall.onnx", "ff8/Zell.onnx", "ff8/Irvine.onnx", "ff8/Quistis.onnx", "ff8/Rinoa.onnx", "ff8/Selphie.onnx", "ff8/Seifer.onnx", "ff8/Edea.onnx","ff8/Laguna.onnx", "ff8/Kiros.onnx", "ff8/Ward.onnx", "ff8/Headmaster.onnx", "ff8/Ward.onnx", "ff8/Kiros.onnx", "ff8/Ward.onnx" ]

tts_enable_unknown_voices = true
tts_voice_other = "ff8/Unknown.onnx"

trace_tts = false
trace_unknown_voices = true
```

## Limitations:
- SSL connections not implemented
- Speaking actor names are extracted from the dialogue & might not always be accurate, in which case it selects the Unknown voice model

## Notes:
- If you have problems, enable trace_tts and check FFNx.log when dialogue occurs
- The TTS voice clips are generated in real-time and are saved locally (FINAL FANTASY VIII/voice)
- The TTS results are not always perfect, but they are funny and it beats reading right?
- I have only tested TTS with piper, clips are generated in around ~0.15s from a local server, other backends or AI generation might take longer which would result in a delay before the dialogue
- If a sound clip already exists, a TTS clip will not be created, so it can work with other voice mods such as the [echo-s-8](https://www.tsunamods.com/echo-s-8/) demo, without effecting them
- Standard voice models for [piper](https://github.com/rhasspy/piper) can be found [here](https://github.com/rhasspy/piper/blob/master/VOICES.md) or [here](https://huggingface.co/rhasspy/piper-voices/tree/main)
- [Here](https://ssamjh.nz/create-custom-piper-tts-voice/) is a tutorial on how to create custom voice models

## TTS server tutorial (docker)
- You can quickly start a docker tts server with the following commands (with GPU, check [LocalAI](https://github.com/mudler/LocalAI) for cpu instructions):
```
mkdir localai
cd localai
docker run -ti -v $PWD/models:/build/models --name local-ai -p 8693:8080 --gpus all localai/localai:latest-aio-gpu-nvidia-cuda-12
```
- After the server is running, copy all the voice models from the release to localai/models
