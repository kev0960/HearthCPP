#include <fstream>
#include <iostream>
#include <vector>
#define LOG(X) Log::LogMessage(__PRETTY_FUNCTION__, __LINE__, X);
using namespace std;
class Log {
  void LogMsg(const char* file_name, int line_number, const char* msg) {
    if (log_file_.good()) {
      log_file_ << "[ " << file_name << " ] at " << line_number << " :: " << msg
                << endl;
    } else {
      cout << "Logging Error! Unable to open Logfile" << endl;
    }
    log_file_.flush();
  }

  ofstream log_file_;

  Log(const string& file_name) : log_file_(file_name) {
    if (!log_file_.good()) {
      cout << "Log File Open Error!" << endl;
    }
  }

 public:
  static void LogMessage(const char* name, int line_number, const char* msg) {
    static Log logger("default_log.log");
    logger.LogMsg(name, line_number, msg);
  }
};