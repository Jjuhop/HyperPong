#pragma once

enum LogLevel {
	Trace = 0,
	Info,
	Warn,
	Error,
	FatalError,
};

#define LOGGER_INSTANCES X(GAME)\
X(RENDERER)

class Logger {
public:
	void Trace(const char* info);
	void Trace(const std::string& info);
	void Info(const char* info);
	void Info(const std::string& info);
	void Warn(const char* info);
	void Warn(const std::string& info);
	void Error(const char* info);
	void Error(const std::string& info);
	void FatalError(const char* info);
	void FatalError(const std::string& info);

	/** Overwrites the default naming, caller should be wary of overwriting existing files. */
	//static void SetLogFileName(const std::string& name);
	///** Overwrites the log output to be written to the specified stream eventually */
	//static void SetLogOutputStream(std::ostream& os);
	///** Prints the logged events */
	//static void Print();

	void Print(const char* label, const char* message, std::ostream& os = std::cout);
	void SetLevel(LogLevel level);

	/** No copying or moving for safety reasons */
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;
	Logger(Logger&& other) = delete;
	Logger& operator=(Logger&& other) = delete;
	~Logger();

	//static std::ostream& GetStorageStream();
#define X(n) static Logger& GetInst##n();
	LOGGER_INSTANCES
#undef X

private:
	/** No constructing from outside */
	Logger(const char* name);

		//static Logger& Instance();

		//std::stringstream m_contents;
		//std::string m_logFileName;
		//std::ostream* m_osOverwrite;
		const char* m_loggerName;
	LogLevel m_level;
};

#define RENDERER_TRACE(m) Logger::GetInstRENDERER().Trace(m);
#define RENDERER_INFO(m) Logger::GetInstRENDERER().Info(m);
#define RENDERER_WARN(m) Logger::GetInstRENDERER().Warn(m);
#define RENDERER_ERROR(m) Logger::GetInstRENDERER().Error(m);
#define RENDERER_FATAL(m) Logger::GetInstRENDERER().FatalError(m);

#define GAME_TRACE(m) Logger::GetInstGAME().Trace(m);
#define GAME_INFO(m) Logger::GetInstGAME().Info(m);
#define GAME_WARN(m) Logger::GetInstGAME().Warn(m);
#define GAME_ERROR(m) Logger::GetInstGAME().Error(m);
#define GAME_FATAL(m) Logger::GetInstGAME().FatalError(m);

#ifdef _MSC_VER

#define RENDERER_ASSERT(x, s) { if(!(x)) { RENDERER_ERROR(std::string("Assertion Failed: ") + s); __debugbreak(); } }
#define GAME_ASSERT(x, s) { if(!(x)) { GAME_ERROR(std::string("Assertion Failed: ") + s); __debugbreak(); } }

#else

#include <signal.h>
#ifdef SIGTRAP
#define RENDERER_ASSERT(x, s) { if(!(x)) { RENDERER_ERROR(std::string("Assertion Failed: ") + s); raise(SIGTRAP); } }
#define GAME_ASSERT(x, s) { if(!(x)) { GAME_ERROR(std::string("Assertion Failed: ") + s); raise(SIGTRAP); } }
#else
#define RENDERER_ASSERT(x, s) { if(!(x)) { RENDERER_ERROR(std::string("Assertion Failed: ") + s); raise(SIGABRT); } }
#define GAME_ASSERT(x, s) { if(!(x)) { GAME_ERROR(std::string("Assertion Failed: ") + s); raise(SIGABRT); } }
#endif

#endif