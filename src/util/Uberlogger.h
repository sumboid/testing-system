#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <mutex>
#include "../system/util/Listener.h"
#include <queue>

// TODO:
//  1. Disable/enable threadsafety
//  2. Disabled log levels
//  3. Enable/disable separated thread

#define UBERDEFAULT "default"

class Uberstyle {
private:
  std::string escape = "\033[";
  std::string reset = escape + "0m";
  std::string delimiter = ";";

  int ccolor = 29;
  int cbold = 1;
  int citalic = 3;
  int cunderlined = 4;
  int cflashing = 6;

  bool vcolor = false;
  bool vbold = false;
  bool vitalic = false;
  bool vunderlined = false;
  bool vflashing = false;

public:
  Uberstyle& color(int c) { ccolor = c; vcolor = true; return *this; }
  Uberstyle& bold() { vbold = true; return *this; }
  Uberstyle& italic() { vitalic = true; return *this; }
  Uberstyle& underlined() { vunderlined = true; return *this; }
  Uberstyle& flashing() { vflashing = true; return *this; }

  std::string translate(std::string str) {
    std::stringstream stream;
    if(ccolor == 29 && !vbold && !vitalic && !vunderlined && !vflashing)
      stream << str;
    else {
	    stream << escape;
	    if(vbold) stream << cbold << delimiter;
	    else stream << 0 << delimiter;
	    if(vitalic) stream << citalic << delimiter;
	    if(vunderlined) stream << cunderlined << delimiter;
	    if(vflashing) stream << cflashing << delimiter;
	    stream << ccolor << "m" << str << reset;
    }

    return stream.str();
  }

  Uberstyle& operator | (const Uberstyle& style) {
    if(style.ccolor != 29) ccolor = style.ccolor;
    if(style.vcolor != false) vcolor = true;
    if(style.vbold != false) vbold = true;
    if(style.vitalic != false) vitalic = true;
    if(style.vunderlined != false) vunderlined = true;
    if(style.vflashing != false) vflashing = true;
    return *this;
  }
};

// Style setters
#define USTYLE(x, y) (Uberlogger::instance(#x).style(Uberstyle() | y))
#define UBOLD (Uberstyle().bold())
#define UITALIC (Uberstyle().italic())
#define UUNDERLINED (Uberstyle().underlined())
#define UFLASHING (Uberstyle().flashing())
#define UCOLOR(x) (Uberstyle().color(x))

// Colors
#define BLACK (30)
#define RED (31)
#define GREEN (32)
#define YELLOW (33)
#define BLUE (34)
#define MAGENTA (35)
#define CYAN (36)
#define WHITE (37)

class Uberlogger {
public:
  class Logger {
  private:
    std::string  _logname;
    std::string  _template;
    std::map<std::string, std::function<std::string(void)>> replace;
    Uberstyle _style;
    std::mutex omutex;

  public:
    Logger(std::string logname = UBERDEFAULT):
      _logname(logname),
      _style(Uberstyle()) {
      templ("[%name] %msg");
      repl("%name", [=]() { return _logname; });
    }

    Logger(const Logger& logger): _logname(logger._logname) {}

    void add (std::string msg) {
      //Need to fix
      if(msg.empty())
      {
        std::cout << "It's empty!" << std::endl;
        return;
      }
      omutex.lock();
      std::string result = _template;
      omutex.unlock();

      for(auto r : replace) {
        auto begin = result.find(r.first);
        if(begin != std::string::npos)
          result.replace(begin, r.first.size(), r.second());
      }

      auto begin = result.find("%msg");
      if(begin != std::string::npos)
        result.replace(begin, 4, msg);

      begin = result.find("%name");
      if(begin != std::string::npos)
        result.replace(begin, 5, _logname);

      omutex.lock();
      result = _style.translate(result);
      omutex.unlock();

      Uberlogger::print(result);
    }

    void templ(const std::string& _t) {
      omutex.lock();
      _template = _t;
      omutex.unlock();
    }

    void repl(const std::string& str, std::function<std::string(void)> func) {
      omutex.lock();
      replace.emplace(str, func);
      omutex.unlock();
    }

    void style(const Uberstyle& __style) {
      omutex.lock();
      _style = __style;
      omutex.unlock();
    }
  };

  static Logger& instance(const std::string& logname = UBERDEFAULT) {
    auto logger = loggers.find(logname);
    if(logger == loggers.end()) {
      loggers.emplace(logname, Logger(logname));
    }
    return loggers[logname];
  }

  static void init() {
    messageLoopThread = std::thread(&Uberlogger::messageLoop);
  }

  static void messageLoop() {
    while(true) {
      queueMutex.lock();
      size_t queueSize = messageQueue.size();
      queueMutex.unlock();

      if(queueSize == 0) {
        messageQueueListener.wait();
        continue;
      }

      for(size_t i = 0; i < queueSize; ++i) {
        queueMutex.lock();
        auto message = messageQueue.front();
        messageQueue.pop();
        queueMutex.unlock();

        std::cout << message << std::endl;
      }
    }
  }

  static void print(const std::string& message) {
    queueMutex.lock();
    messageQueue.push(message);
    queueMutex.unlock();
    messageQueueListener.notifyAll();
  }

private:
  static std::map<std::string, Logger> loggers;
  static std::thread messageLoopThread;
  static std::mutex queueMutex;
  static std::queue<std::string> messageQueue;
  static ts::system::Listener messageQueueListener;
};

class Message {
private:
  std::stringstream msg;
  std::string logname;
  bool end;

public:
  class End {};

  Message(const Message& m):
    logname(m.logname),
    end(m.end) {
    msg << m.msg.str();
  }

  Message(const End&) {
    end = true;
  }

  template<class T>
  Message(const T& some):
    logname(UBERDEFAULT),
    end(false) {
    msg << some;
  }

  Message():
    logname(UBERDEFAULT),
    end(false) {}

  Message& operator<< (const Message& some) {
    if(some.end) {
      Uberlogger::instance(logname).add(this->get());
    } else {
      msg << some.msg.str();
    }
    return *this;
  }

  std::string get() const {
    return msg.str();
  }

  Message& withLogname(std::string _ln) {
    logname = _ln;
    return *this;
  }
};

#define UBERINIT std::map<std::string, Uberlogger::Logger> Uberlogger::loggers; \
                   std::thread Uberlogger::messageLoopThread; \
                   std::mutex Uberlogger::queueMutex; \
                   std::queue<std::string> Uberlogger::messageQueue; \
                   ts::system::Listener Uberlogger::messageQueueListener

#define UBERRUN Uberlogger::init()

static inline Message UBERLOG() {
  return Message();
}

static inline Message UBERLOG(const std::string& s) {
  return Message().withLogname(s);
}

static inline Message UBEREND() {
  return Message::End();
}

static inline void UBERTEMPLATE(const std::string& logname, const std::string& tmpl) {
  Uberlogger::instance(logname).templ(tmpl);
}

static inline void UBERTEMPLATE(const std::string& tmpl) {
  Uberlogger::instance().templ(tmpl);
}

static inline void UBERREPLACE(const std::string& x, std::function<std::string(void)> y) {
  Uberlogger::instance().repl(x, y);
}

static inline void UBERREPLACE(const std::string& logname, const std::string& x, std::function<std::string(void)> y) {
  Uberlogger::instance(logname).repl(x, y);
}

#define UTEMPLATE(x, tmpl) (UBERTEMPLATE(#x, tmpl))
#define UREPLACE(x, a, b) (UBERREPLACE(#x, a, b))
#define ULOG(x) (UBERLOG(#x))
#define UEND (UBEREND())
