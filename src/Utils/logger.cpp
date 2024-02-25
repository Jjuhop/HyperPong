#include "logger.h"

void Logger::Trace(const char* info)
{
	if (m_level <= LogLevel::Trace)
		this->Print("TRACE", info);
}

void Logger::Trace(const std::string& info)
{
	if (m_level <= LogLevel::Trace)
		this->Print("TRACE", info.c_str());
}

void Logger::Info(const char* info)
{
	if (m_level <= LogLevel::Info)
		this->Print("INFO", info);
}

void Logger::Info(const std::string& info)
{
	if (m_level <= LogLevel::Info)
		this->Print("INFO", info.c_str());
}

void Logger::Warn(const char* info)
{
	if (m_level <= LogLevel::Warn)
		this->Print("WARN", info);
}

void Logger::Warn(const std::string& info)
{
	if (m_level <= LogLevel::Warn)
		this->Print("WARN", info.c_str());
}

void Logger::Error(const char* info)
{
	if (m_level <= LogLevel::Error)
		this->Print("ERROR", info, std::cerr);
}

void Logger::Error(const std::string& info)
{
	if (m_level <= LogLevel::Error)
		this->Print("ERROR", info.c_str(), std::cerr);
}

void Logger::FatalError(const char* info)
{
	if (m_level <= LogLevel::FatalError)
		this->Print("FATAL ERROR", info, std::cerr);
}

void Logger::FatalError(const std::string& info)
{
	if (m_level <= LogLevel::FatalError)
		this->Print("FATAL ERROR", info.c_str(), std::cerr);
}

static std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> UTCTime()
{
	return std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
}

static std::chrono::zoned_seconds LocalTime()
{
	return std::chrono::zoned_time(std::chrono::current_zone(), UTCTime());
}

static std::string LocalTimeStr()
{
	return std::format("[{0:%H}:{0:%M}:{0:%S}]", LocalTime());
}

void Logger::Print(const char* label, const char* message, std::ostream& os)
{
	std::string name(m_loggerName);
	while (name.size() < 8)
		name += " ";
	os << LocalTimeStr() << " " << name << " - " << label << ": " << message << std::endl;
}

void Logger::SetLevel(LogLevel level)
{
	m_level = level;
}

Logger::~Logger()
{
	std::string m = std::string("Destructed the logger: ") + m_loggerName;
	Print("SPECIAL", m.c_str());
}

Logger::Logger(const char* name)
	: m_loggerName(name), m_level(LogLevel::Trace)
{
	std::string m = std::string("Constructed a logger: ") + name;
	Print("SPECIAL", m.c_str());
}

#define X(n) Logger& Logger::GetInst##n()\
{\
	static Logger logger(#n);\
	return logger;\
}
LOGGER_INSTANCES
#undef X

