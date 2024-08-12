#include <iostream>
#include <fstream>
#include <filesystem>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

class HttpClient {
public:
  HttpClient() {

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      if (trace_tts) ffnx_trace("TTS: WSAStartup failed \n");
      initialized = false;
    }
    else {
      initialized = true;
    }
  }

  ~HttpClient() {
    if (initialized) {
      WSACleanup();
    }
  }

  bool post(const std::string& host, const int port, const std::string& path, const std::string& data, std::string& response) {
    if (!initialized) return false;

    SOCKET sock = createSocket();
    if (sock == INVALID_SOCKET)
    {
      if (trace_tts) ffnx_trace("TTS: failed to create socket \n");
      return false;
    }

    if (!connectToServer(sock, port, host)) {
      if (trace_tts) ffnx_trace("TTS: failed to connect to %s:%i \n", host.c_str(), port);

      closesocket(sock);
      return false;
    }

    std::string request = createPostRequest(host, path, data);
    if (send(sock, request.c_str(), request.length(), 0) < 0) {
      if (trace_tts) ffnx_trace("TTS: failed to send data to TTS server \n");
      closesocket(sock);
      return false;
    }

    response = receiveResponse(sock);
    closesocket(sock);
    return true;
  }

private:
  WSADATA wsaData;
  bool initialized;

  SOCKET createSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    return sock;
  }

  bool connectToServer(SOCKET& sock, const int port, const std::string& host) {
    struct hostent* he = gethostbyname(host.c_str());
    if (he == nullptr) {
      return false;
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = *(unsigned long*)he->h_addr;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
      return false;
    }

    return true;
  }

  std::string createPostRequest(const std::string& host, const std::string& path, const std::string& data) {
    return "POST " + path + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Content-Type: application/json\r\n" +
      "accept: audio/x-wav\r\n" +
      "Content-Length: " + std::to_string(data.length()) + "\r\n" +
      "Connection: close\r\n\r\n" +
      data;
  }

  std::string receiveResponse(SOCKET& sock) {
    std::string response;
    char buffer[4096];
    int bytesReceived;

    bool headerSkipped = false;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
      buffer[bytesReceived] = '\0';

      if (!headerSkipped) {
        std::string tempBuffer(buffer, bytesReceived);
        size_t headerEnd = tempBuffer.find("\r\n\r\n");

        if (headerEnd != std::string::npos) {
          headerSkipped = true;
          response.append(tempBuffer.substr(headerEnd + 4));
        }
      }
      else {
        response.append(buffer, bytesReceived);
      }
    }

    if (bytesReceived < 0) {
      if (trace_tts) ffnx_trace("TTS: error getting response from TTS server \n");
    }
    else if (trace_tts) ffnx_trace("TTS: response success, %i bytes \n", response.size());

    return response;
  }

} http_tts;

class cTTS
{
private:
  bool isVoiced(std::string character, std::string& voiceModel)
  {
    for (size_t i = 0; i < character.length(); ++i) {
      if (character[i] == ' ') {
        character.resize(i);
      }
    }

    if (ff8)
    {
      auto it = std::find(tts_ff8_voiced.begin(), tts_ff8_voiced.end(), character);
      if (it != tts_ff8_voiced.end()) {
        int index = std::distance(tts_ff8_voiced.begin(), it);
        if (index >= 0 && index < tts_ff8_voiced_models.size()) {
          if (trace_tts) ffnx_trace("TTS: loaded voice model: %s (%s)\n", tts_ff8_voiced_models[index].c_str(), character.c_str());
          voiceModel = tts_ff8_voiced_models[index];
          return true;
        }
      }
    }
    else
    {
      auto it = std::find(tts_ff7_voiced.begin(), tts_ff7_voiced.end(), character);
      if (it != tts_ff7_voiced.end()) {
        int index = std::distance(tts_ff7_voiced.begin(), it);
        if (index >= 0 && index < tts_ff7_voiced_models.size()) {
          if (trace_tts) ffnx_trace("TTS: loaded voice model: %s (%s)\n", tts_ff7_voiced_models[index].c_str(), character.c_str());
          voiceModel = tts_ff7_voiced_models[index];
          return true;
        }
      }
    }

    if (trace_tts) ffnx_trace("TTS: %s not voiced\n", character.c_str());

    voiceModel = tts_voice_other;
    return false;
  }

  size_t getUtf8CharLength(unsigned char firstByte) {
    if ((firstByte & 0x80) == 0) {
      return 1; // ASCII character (1 byte)
    }
    else if ((firstByte & 0xE0) == 0xC0) {
      return 2; // 2-byte UTF-8 character
    }
    else if ((firstByte & 0xF0) == 0xE0) {
      return 3; // 3-byte UTF-8 character
    }
    else if ((firstByte & 0xF8) == 0xF0) {
      return 4; // 4-byte UTF-8 character
    }
    return 0; // Invalid UTF-8 byte sequence
  }

  std::string get_field_dialog_char_name(byte window_id) {
    byte id = *(byte*)ff7_externals.current_dialog_string_pointer[window_id];

    switch (id) {
    case 0xEA: // CLOUD
      return "cloud";
    case 0xEB: // BARRET
      return "barret";
    case 0xEC: // TIFA
      return "tifa";
    case 0xED: // AERITH
      return "aerith";
    case 0xEE: // RED XIII
      return "red";
    case 0xEF: // YUFFIE
      return "yuffie";
    case 0xF0: // CAIT SITH
      return "cait";
    case 0xF1: // VINCENT
      return "vincent";
    case 0xF2: // CID
      return "cid";
    default:
      return "";
    }
  }

  void split_dialogue(std::string decoded_text, std::string& name, std::string& message, std::string& voiceModel, std::string& unvoiced, byte window_id = 0)
  {
    transform(decoded_text.begin(), decoded_text.end(), decoded_text.begin(), tolower);

    bool gotName = false;
    if (!ff8)
    {
      name = get_field_dialog_char_name(window_id);
      if (!name.empty() && isVoiced(name, voiceModel)) gotName = true;
    }

    unsigned char last_char = 0;
    for (auto it = decoded_text.begin(); it != decoded_text.end(); )
    {
      unsigned char current_char = static_cast<unsigned char>(*it);
      size_t charLength = getUtf8CharLength(current_char);

      if (charLength == 0 || std::distance(it, decoded_text.end()) < charLength) {
        break;
      }

      std::string utf8Char(it, it + charLength);

      unsigned char new_char = current_char;
      if (current_char < ' ' || current_char > '}') new_char = ' ';
      if (current_char == '!' || current_char == '-' || current_char == '.') new_char = ' ';

      if (!gotName && (current_char == 0xe2 || current_char == 0xf2)) //quote start, meaning the name as finished, or its unnamed
      {
        if (name.empty())
        {
          voiceModel = tts_voice_other;
          name = "Unknown";
        }
        else
        {
          if(ff8) name.pop_back();
          if (!isVoiced(name, voiceModel))
          {
            unvoiced = name;
            name = "Unknown";
          }
        }

        gotName = true;
      }
      else
      {
        if (current_char == '"' && charLength == 1)
        {
          last_char = current_char;
          if(ff8) std::advance(it, charLength);
          else  std::advance(it, 1);
          continue; //make sure we cant escape the json
        }

        if (last_char == '.' && current_char == '.') //replace ... with space (makes tts sounds better)
        {
          if (gotName) message += " ";
        }
        else if (last_char == '.' && current_char != ' ' && charLength == 1) //ensure a space after .
        {
          if (gotName)
          {
            message += " ";
            message += ",";
            message += new_char;
          }
        }
        else
        {
          if (!gotName) name += new_char;
          else message += new_char;
        }
      }
      

      last_char = current_char;
      if (ff8) std::advance(it, charLength);
      else  std::advance(it, 1);
    }

    if (message.empty()) //there was likely no name in the dialogue
    {
      message = name;
      voiceModel = tts_voice_other;
      name = "Unknown";
    }
  }

  std::string tokenize_text(std::string decoded_text)
  {
    transform(decoded_text.begin(), decoded_text.end(), decoded_text.begin(), tolower);

    std::string filename{};
    for (auto current_char : decoded_text)
    {
      if (current_char >= 'a' && current_char <= 'z')
        filename += current_char;
      else if (current_char >= '0' && current_char <= '9')
        filename += current_char;
      else if (current_char == ' ')
        filename += '_';
      else if (current_char == '{' || current_char == '}')
        filename += current_char;
    }
    return filename;
  }

  bool request_tts(std::string &tts_dialogue, std::string &voice_model, char* folderfilename)
  {
    std::string response;
    std::string payload = "{\"backend\": \"" + tts_backend + "\", \"input\" : \"" + tts_dialogue + "\", \"language\" : \"" + tts_language + "\", \"model\" : \"" + voice_model + "\"}";
    if (trace_tts) ffnx_trace("TTS: request JSON, %s \n", payload.c_str());
    http_tts.post(tts_backend_ip.c_str(), tts_backend_port, tts_path.c_str(), payload.c_str(), response);
    std::filesystem::create_directories(std::filesystem::path(folderfilename).parent_path());
    std::fstream file;
    file.open(folderfilename, std::ios::app | std::ios::binary);
    if (file.is_open())
    {
      file.write(response.c_str(), response.length());
      file.close();
      if (trace_tts) ffnx_trace("TTS: sound file created: %s (%ibytes) \n", folderfilename, response.length());
      return true;
    }
    else if (trace_tts) ffnx_trace("TTS: failed to create file: %s \n", folderfilename);
    return false;
  }
public:

  bool tts_create_ff7(const char* text_data1, char* field_name, byte window_id, byte dialog_id, byte page_count)
  {
    if (page_count) return false;
    if (ff7_externals.field_entity_id_list[window_id] != *ff7_externals.current_entity_id) return false;
    if (ff7_externals.field_entity_id_list[window_id] == 0x0) return false;

    std::string decoded_text = decode_ff7_text(text_data1);
    std::string tts_dialogue, tts_name, voice_model, unvoiced;
    split_dialogue(decoded_text, tts_name, tts_dialogue, voice_model, unvoiced, window_id);

    char folderfilename[MAX_PATH] = {};

    std::string file_name = get_voice_filename((char*)field_name, window_id, dialog_id, page_count);
    bool soundFileExists = nxAudioEngine.getFilenameFullPath(folderfilename, file_name.c_str(), NxAudioEngine::NxAudioEngineLayer::NXAUDIOENGINE_VOICE);

    if (!tts_enable_unknown_voices && tts_name == "Unknown")
    {
      if (trace_unknown_voices) ffnx_trace("TTS: not voicing unknown character %s \n", unvoiced.c_str());
      return false;
    }

    if (trace_tts)
    {
      ffnx_trace("TTS: playing/creating %s \n", folderfilename);
      ffnx_trace("TTS: tts_name %s \n", tts_name.c_str());
      ffnx_trace("TTS: tts_dialogue %s \n", tts_dialogue.c_str());
      ffnx_trace("TTS: decoded_text %s \n", decoded_text.c_str());
      ffnx_trace("TTS: soundFileExists %i \n", soundFileExists);
    }

    if (!soundFileExists && !tts_dialogue.empty())
    {
      return request_tts(tts_dialogue, voice_model, folderfilename);
    }

    return soundFileExists;
  }


  bool tts_create_ff8(char* text_data1, int window_id, uint32_t driver_mode)
  {
    std::string decoded_text = ff8_decode_text(text_data1);
    std::string tokenized_dialogue = tokenize_text(decoded_text);

    std::string tts_dialogue, tts_name, voice_model, unvoiced;
    split_dialogue(decoded_text, tts_name, tts_dialogue, voice_model, unvoiced); //I couldnt find any solid way to get the speaker id
    int dialog_id = current_opcode_message_status[window_id].message_dialog_id;

    bool soundFileExists = false;
    char folderfilename[MAX_PATH] = {};
    std::string file_name;

    switch (driver_mode)
    {
    case MODE_FIELD:
      file_name = get_voice_filename((char*)current_opcode_message_status[window_id].field_name.c_str(), window_id, current_opcode_message_status[window_id].message_dialog_id, current_opcode_message_status[window_id].message_page_count);
      soundFileExists = nxAudioEngine.getFilenameFullPath(folderfilename, file_name.c_str(), NxAudioEngine::NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
      break;
    case MODE_BATTLE:
    {
      std::string actor_name = ff8_decode_text(ff8_battle_actor_name[LOBYTE(*ff8_externals.battle_current_actor_talking)]);
      std::string tokenized_actor = tokenize_text(actor_name);
      isVoiced(tokenized_actor, voice_model);

      char voice_file[MAX_PATH] = {};
      sprintf(voice_file, "_battle/%s/%s", tokenized_actor.c_str(), tokenized_dialogue.c_str());
      soundFileExists = nxAudioEngine.getFilenameFullPath(folderfilename, voice_file, NxAudioEngine::NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
      break;
    }
    case MODE_WORLDMAP:

      if (dialog_id < 0)
      {
        char voice_file[MAX_PATH] = {};
        sprintf(voice_file, "_world/text/%s", tokenized_dialogue.c_str());
        soundFileExists = nxAudioEngine.getFilenameFullPath(folderfilename, voice_file, NxAudioEngine::NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
      }
      else
      {
        file_name = get_voice_filename((char*)"_world", window_id, current_opcode_message_status[window_id].message_dialog_id, current_opcode_message_status[window_id].message_page_count);
        soundFileExists = nxAudioEngine.getFilenameFullPath(folderfilename, file_name.c_str(), NxAudioEngine::NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
      }

      break;
    case FF8_MODE_TUTO:
    {
      char voice_file[MAX_PATH] = {};;
      sprintf(voice_file, "_tuto/%04u/%s", *ff8_externals.current_tutorial_id, tokenized_dialogue.c_str());
      soundFileExists = nxAudioEngine.getFilenameFullPath(folderfilename, voice_file, NxAudioEngine::NxAudioEngineLayer::NXAUDIOENGINE_VOICE);

      break;
    }
    default:
      return false;
    }

    if (!tts_enable_unknown_voices && tts_name == "Unknown")
    {
      if (trace_unknown_voices) ffnx_trace("TTS: not voicing unknown character %s \n", unvoiced.c_str());
      return false;
    }

    if (trace_tts)
    {
      ffnx_trace("TTS: playing/creating %s \n", folderfilename);
      ffnx_trace("TTS: tts_name %s \n", tts_name.c_str());
      ffnx_trace("TTS: tts_dialogue %s \n", tts_dialogue.c_str());
      ffnx_trace("TTS: decoded_text %s \n", decoded_text.c_str());
      ffnx_trace("TTS: soundFileExists %i \n", soundFileExists);
    }

    if (!soundFileExists && !tts_dialogue.empty())
    {
      return request_tts(tts_dialogue, voice_model, folderfilename);
    }

    return soundFileExists;
  }
} tts;
