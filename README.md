Fork of [FFNx](https://github.com/julianxhokaxhiu/FFNx) to add TTS support to FF8 from a TTS http server

https://github.com/user-attachments/assets/11dab1b9-e84f-420b-a4fc-0c52066ce825

If you want to build this then check the instructions at [FFNx](https://github.com/julianxhokaxhiu/FFNx).

## Prerequisites:
- FF8 Installed on steam with FFNx
- A locally hosted TTS server, for example: [LocalAI](https://github.com/mudler/LocalAI)
- Enough voice models for each character, all examples included in the release

## Instructions for use:
- Replace AF3DN.P1 and FFNx.toml
- Edit FFNx.toml (TTS-VOICEMOD section) to add you server and voice model paths
- Start game

## Limitations:
- SSL connections not implemented
- Speaking actor names are extracted from the dialogue & might not always be accurate

## Notes:
- If you have problems, enable trace_tts and check FFNx.log when dialogue occurs
- The TTS voice clips are generated in real-time and are saved locally (FINAL FANTASY VIII/voice)
