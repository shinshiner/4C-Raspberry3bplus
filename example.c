#include <assert.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <cstring>

#include "speech_sdk.h"

pthread_mutex_t mutex;
pthread_cond_t cond;

static const int kBufferSize = 640;
static const int kLineWrapLength = 100;

static const int kPlaySampleRate = 16000;
static const int kPlayChannelNum = 2;

// Please refer to the following URL to obtain a valid application key:
//   http://ai.chumenwenwen.com/pages/document/get-started
//static const std::string kAppKey = "072232D5CBEFD8CF6F5DF538C3C3A096";
static const std::string kAppKey = "F518D4D34EC6528A1F186C2C3A700FF2";
static const std::string kAsr = "ASR";
static const std::string kSemantic = "SEMANTIC";
static const std::string kOnebox = "ONEBOX";

volatile bool in_the_session = true;
volatile mobvoi_recognizer_type type = MOBVOI_RECOGNIZER_ONLINE_ONEBOX;

void show_usage() {
  std::cout << "Usage: recognizer_with_file ASR/SEMANTIC/ONEBOX audio_file_path"
            << std::endl;
}

void on_remote_silence_detected() {
  std::cout << "--------> dummy on_remote_silence_detected" << std::endl;
}

void on_partial_transcription(const char* result) {
  std::cout << "--------> dummy on_partial_transcription: " << result
            << std::endl;
}

void on_final_transcription(const char* result) {
  std::cout << "--------> dummy on_final_transcription: " << result
            << std::endl;
  if (type == MOBVOI_RECOGNIZER_ONLINE_ASR) {
    pthread_mutex_lock(&mutex);
    in_the_session = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
  }
}

std::string GetTTS(const std::string& final_transcription) {
  static const std::string kPromptWords[] = {
    std::string("\"displayText\":\""),
    std::string("\"query\":\""),
  };

  size_t pos;
  size_t i;

  for (i = 0; i < sizeof(kPromptWords) / sizeof(kPromptWords[0]); ++i) {
    pos = final_transcription.find(kPromptWords[i], 0);
    if (pos != std::string::npos) {
      break;
    }
  }

  if (pos != std::string::npos) {
    size_t start = pos + kPromptWords[i].size();
    size_t end = final_transcription.find('"', start);
    if (end != std::string::npos) {
      std::string promt_content =
          final_transcription.substr(start, end - start);
      return promt_content;
    }
  }

  return "问问没有听清,您能再说一次吗";
}

void on_result(const char* result) {
  std::cout << "--------> dummy on_result: " << result << std::endl;
  std::string s(result);
  for (int i = 0; i < s.size(); i += kLineWrapLength) {
    int len = std::min((int)(s.size() - i), kLineWrapLength);
    std::string temp = s.substr(i, len);
    std::cout << temp << std::endl;
  }
  std::string tts = GetTTS(result);
  std::cout << tts << std::endl;

  pthread_mutex_lock(&mutex);
  in_the_session = false;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

void on_error(int error_code) {
  std::cout << "--------> dummy on_error with error code: " << error_code
            << std::endl;
  pthread_mutex_lock(&mutex);
  in_the_session = false;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

void on_local_silence_detected() {
  std::cout << "--------> dummy on_local_silence_detected" << std::endl;
  mobvoi_recognizer_stop();
}

void on_volume(float spl) {
  // The sound press level is here, do whatever you want
  // std::cout << "--------> dummy on_speech_spl_generated: spl = "
  //           << std::fixed << std::setprecision(6) << spl
  //           << std::endl;
}

void* send_audio_thread(void* arg) {
  std::ifstream& file = *(std::ifstream*) arg;
  const int kBatchSize = 320;
  int pos = 0;
  file.seekg(0, file.end);
  int length = file.tellg() / 2;
  file.seekg(0, file.beg);
  short in_shorts[kBatchSize];

  usleep(200 * 1000);
  if (file.is_open()) {
    while (pos < length) {
      int stride =
          (pos + kBatchSize < length) ? kBatchSize : (length - pos);
      file.read((char*) &in_shorts, stride * 2);
      mobvoi_send_speech_frame((char*) &in_shorts, stride * 2);
      pos += stride;
    }
  } else {
    std::cout << "File could not be opened!" << std::endl;
  }
  file.close();
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    show_usage();
    return 1;
  }
  // type = MOBVOI_RECOGNIZER_ONLINE_SEMANTIC;

  std::string online_type(argv[1]);
  if (online_type == "ASR") {
    type = MOBVOI_RECOGNIZER_ONLINE_ASR;
  } else if (online_type == "SEMANTIC") {
    type = MOBVOI_RECOGNIZER_ONLINE_SEMANTIC;
  } else {
    type = MOBVOI_RECOGNIZER_ONLINE_ONEBOX;
  }

  std::ifstream test_file;
  test_file.open(argv[2]);
  // Read the audio file specified by the command line argument
  if (!test_file.is_open()) {
    std::cout << "Failed to open file " << argv[1] << std::endl;
    return 2;
  }

  // SDK initilalization, including callback functions
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  mobvoi_sdk_init(kAppKey.c_str());
  mobvoi_recognizer_set_params("mobvoi_folder", "/home/pi/speechsdk-1.2.0/");
  mobvoi_recognizer_set_params(
      "location", "中国,北京市,北京市,海淀区,苏州街,3号,39.989602,116.316568");


  mobvoi_recognizer_handler_vtable* speech_handlers =
      new mobvoi_recognizer_handler_vtable;
  assert(speech_handlers != NULL);
  memset(speech_handlers, 0, sizeof(mobvoi_recognizer_handler_vtable));
  speech_handlers->on_error = &on_error;
  speech_handlers->on_partial_transcription = &on_partial_transcription;
  speech_handlers->on_final_transcription = &on_final_transcription;
  speech_handlers->on_local_silence_detected = &on_local_silence_detected;
  speech_handlers->on_remote_silence_detected = &on_remote_silence_detected;
  speech_handlers->on_result = &on_result;
  speech_handlers->on_volume = &on_volume;
  mobvoi_recognizer_set_handler(speech_handlers);

  mobvoi_recognizer_start(type);

  // Send the audio data in a separated thread
  pthread_t tid;
  pthread_create(&tid, NULL, send_audio_thread, &test_file);
  pthread_mutex_lock(&mutex);
  in_the_session = true;
  while (in_the_session) {
    pthread_cond_wait(&cond, &mutex);
  }
  pthread_mutex_unlock(&mutex);

  // SDK clean up
  std::cout << "start sdk cleanup" << std::endl;
  mobvoi_sdk_cleanup();
  std::cout << "end sdk cleanup" << std::endl;
  delete speech_handlers;
  std::cout << "end dummy sender" << std::endl;
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  pthread_join(tid, NULL);

  return 0;
}