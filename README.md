Fork of [FFNx](https://github.com/julianxhokaxhiu/FFNx) to add TTS support to FF8 from a TTS host/server

If you want to build this then check the instructions at [FFNx](https://github.com/julianxhokaxhiu/FFNx).

Prerequisites
- A locally hosted TTS server, for example: [LocalAI](https://github.com/mudler/LocalAI)
- Enough voice models for each character, all examples included in the release

Instructions:
- Install the original FFNx
- Replace AF3DN.P1 and FFNx.toml from this repo
- Edit FFNx.toml (TTS-VOICEMOD section) to add you server and voice model paths

Limitations:
- SSL connections not implemented
- Speaking actor names are extracted from the dialogue & might not always be accurate

Notes:
- If you have problems, enable trace_tts and check FFNx.log when dialogue occurs
