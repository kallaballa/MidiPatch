/*
s * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include "plog/Util.h"

#include <thread>
using std::string;

class ThreadNameLookup {
  std::mutex mutex_;
  std::map<unsigned int, string> idLookup_;
  std::map<string, unsigned int> nameLookup_;
  std::map<string, size_t> nameCount_;

  static ThreadNameLookup* instance_;

  ThreadNameLookup() {}
public:
  static ThreadNameLookup* getInstance() {
    if(instance_ == NULL) {
      instance_ = new ThreadNameLookup();
    }

    return instance_;
  }

  static void set(const unsigned int id, const string& threadName) {
    getInstance()->mutex_.lock();
    auto itname = getInstance()->nameCount_.find(threadName);
    string name = threadName;
    size_t cnt = 0;
    if(itname != getInstance()->nameCount_.end()) {
      cnt = getInstance()->nameCount_[threadName];
    }

    name += ("-" + std::to_string(cnt));
    getInstance()->nameCount_[threadName] = cnt + 1;
    getInstance()->nameLookup_[name] = id;
    getInstance()->idLookup_[id] = name;
    getInstance()->mutex_.unlock();
  }

  static string get(const unsigned int& id) {
    getInstance()->mutex_.lock();
    auto it = getInstance()->idLookup_.find(id);
    string name;
    if(it != getInstance()->idLookup_.end())
      name = (*it).second;
    else
      name = "Unknown";
    getInstance()->mutex_.unlock();
    return name;
  }

  static void remove(const unsigned int& id) {
    getInstance()->mutex_.lock();
    string name = getInstance()->idLookup_[id];
    getInstance()->nameLookup_.erase(name);
    getInstance()->idLookup_.erase(id);
    getInstance()->mutex_.unlock();
  }

  static size_t size() {
    return getInstance()->idLookup_.size();
  }
};

 ThreadNameLookup* ThreadNameLookup::instance_ = NULL;

 namespace plog
 {
     class MidiPatchFormatter
     {
     public:
         static util::nstring header()
         {
             return util::nstring();
         }

         static util::nstring format(const Record& record)
         {
           tm t;
           util::localtime_s(&t, &record.getTime().time);

           util::nostringstream ss;
           ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
           ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ");
           ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
           ss << PLOG_NSTR("[") << ThreadNameLookup::get(record.getTid())  << PLOG_NSTR("] ");
           ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
           ss << record.getMessage() << PLOG_NSTR("\n");

           return ss.str();
         }
     };
 }


namespace janosh {
  Logger* Logger::instance_ = NULL;

  Logger::Logger(const LogLevel l) : dblog_(false), level_(l) {
    plog::ConsoleAppender<plog::MidiPatchFormatter>* consoleAppender = new plog::ConsoleAppender<plog::MidiPatchFormatter>();
    switch (l) {
    case L_DEBUG:
      plog::init(plog::debug, consoleAppender);
      break;
    case L_INFO:
      plog::init(plog::info, consoleAppender);
      break;
    case L_WARNING:
      plog::init(plog::warning, consoleAppender);
      break;
    case L_ERROR:
      plog::init(plog::error, consoleAppender);
      break;
    case L_FATAL:
      plog::init(plog::fatal, consoleAppender);
      break;
    case L_GLOBAL:
      plog::init(plog::verbose, consoleAppender);
      break;
    }
  }

  void Logger::init(const LogLevel l) {
    Logger::instance_ = new Logger(l);
  }

  Logger& Logger::getInstance() {
    assert(Logger::instance_ != NULL);
    return *Logger::instance_;
  }

  LogLevel Logger::getLevel() {
    return Logger::getInstance().level_;
  }

  void Logger::registerThread(const string& name) {
    ThreadNameLookup::set(plog::util::gettid(),name);
    LOG_DEBUG_MSG("Register thread", ThreadNameLookup::size());
  }

  void Logger::removeThread() {
    LOG_DEBUG("Remove thread");
    ThreadNameLookup::remove(plog::util::gettid());
  }
}
