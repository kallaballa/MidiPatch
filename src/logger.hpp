/*
 * Ctrl-Cut - A laser cutter CUPS driver
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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <initializer_list>
#include <assert.h>
#include <sstream>
#include <sstream>
#include <mutex>
#include "plog/Log.h"
#include "plog/Appenders/ConsoleAppender.h"

namespace midipatch {
  using std::string;

  enum LogLevel {
    L_FATAL = 0,
    L_ERROR = 1,
    L_WARNING = 2,
    L_INFO = 3,
    L_DEBUG = 4,
    L_GLOBAL = 5
  };

  class Logger {
  private:
    bool dblog_;
    Logger(const LogLevel l, const string& logFile);
    static Logger* instance_;
  public:
    LogLevel level_;

    static void init(const LogLevel l, const string& logFile);
    static Logger& getInstance();
    static LogLevel getLevel();
    static void registerThread(const string& name);
    static void removeThread();
  };
#ifndef NDEBUG
  #define LOG_GLOBAL_STR(x) LOG(plog::verbose) << x
  #define LOG_DEBUG_STR(x) LOG(plog::debug) << x
  #define LOG_INFO_STR(x) LOG(plog::info) << x
  #define LOG_WARN_STR(x) LOG(plog::warning) << x
  #define LOG_ERR_STR(x) LOG(plog::error) << "\033[1;31m" << x << "\033[0m"
  #define LOG_FATAL_STR(x) LOG(plog::fatal) << "\033[1;31m" << x << "\033[0m"
  #define LOG_GLOBAL_MSG(msg,x) LOG(plog::verbose) << msg << ": " << x
  #define LOG_DEBUG_MSG(msg,x) LOG(plog::debug) << msg << ": " << x
  #define LOG_INFO_MSG(msg,x) LOG(plog::info) << msg << ": " << x
  #define LOG_WARN_MSG(msg,x) LOG(plog::warning) << msg << ": " << x
  #define LOG_ERR_MSG(msg,x) LOG(plog::error) << msg << ": " << "\033[1;31m" << x << "\033[0m"
  #define LOG_FATAL_MSG(msg,x) LOG(plog::fatal) << msg << ": " << "\033[1;31m" << x << "\033[0m"
  #define LOG_GLOBAL(x) LOG(plog::verbose) << x
  #define LOG_DEBUG(x) LOG(plog::debug) << x
  #define LOG_INFO(x) LOG(plog::info) << x
  #define LOG_WARN(x) LOG(plog::warning) << x
  #define LOG_ERR(x) LOG(plog::error) << "\033[1;31m" << x << "\033[0m"
  #define LOG_FATAL(x) LOG(plog::fatal) << "\033[1;31m" << x << "\033[0m"
#else
	#define LOG_GLOBAL_STR(x)
	#define LOG_DEBUG_STR(x)
	#define LOG_INFO_STR(x)
	#define LOG_WARN_STR(x)
	#define LOG_ERR_STR(x) LOG(plog::error) << "\033[1;31m" << x << "\033[0m"
	#define LOG_FATAL_STR(x) LOG(plog::fatal) << "\033[1;31m" << x << "\033[0m"
	#define LOG_GLOBAL_MSG(msg,x)
	#define LOG_DEBUG_MSG(msg,x)
	#define LOG_INFO_MSG(msg,x)
	#define LOG_WARN_MSG(msg,x)
	#define LOG_ERR_MSG(msg,x) LOG(plog::error) << msg << ": " << "\033[1;31m" << x << "\033[0m"
	#define LOG_FATAL_MSG(msg,x) LOG(plog::fatal) << msg << ": " << "\033[1;31m" << x << "\033[0m"
	#define LOG_GLOBAL(x)
	#define LOG_DEBUG(x)
	#define LOG_INFO(x)
	#define LOG_WARN(x)
	#define LOG_ERR(x) LOG(plog::error) << "\033[1;31m" << x << "\033[0m"
	#define LOG_FATAL(x) LOG(plog::fatal) << "\033[1;31m" << x << "\033[0m"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif
  #define JANOSH_TRACE(...) if(Logger::isTracing()) Logger::trace(string(__FUNCTION__), ##__VA_ARGS__);
#ifdef __clang__
#pragma clang diagnostic pop
#endif

}

#endif /* LOGGER_H_ */
