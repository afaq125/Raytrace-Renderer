#pragma once

namespace Renderer
{
#define LOG(verbostiy, ...) Log(verbostiy, "[", __FUNCTION__, "]", " [Line: ", __LINE__, "] ", __VA_ARGS__);
#define LOG_INFO(...)    LOG(BaseLogger::Verbosity::Info,    __VA_ARGS__)
#define LOG_WARNING(...) LOG(BaseLogger::Verbosity::Warning, __VA_ARGS__)
#define LOG_ERROR(...)   LOG(BaseLogger::Verbosity::Error,   __VA_ARGS__)
#define LOG_FAILURE(...) LOG(BaseLogger::Verbosity::Failure, __VA_ARGS__)
#define LOG_DEBUG(...)   LOG(BaseLogger::Verbosity::Debug,   __VA_ARGS__)

    const std::wstring DEFAULT_LOG_FILE_PATH               = L"./";
    const std::wstring DEFAULT_LOG_FILE_NAME               = L"Logs";
    const std::wstring DEFAULT_LOG_FILE_EXTENSION          = L".txt";
    const std::string  DEFAULT_LOG_FILE_HEADER             = "FILE_LOGGER";
    const std::string  DEFAULT_LOG_FILE_FORMAT             = "%Y_%m_%d";
    const std::string  DEFAULT_LOG_FILE_HEADER_TIME_FORMAT = "%Y_%m_%d_%H_%M_%S";
    const std::string  DEFAULT_LOG_TIME_FORMAT             = "%Y-%m-%d %T";
    const char DEFAULT_SEPARATOR = '_';

    class BaseLogger
    {
    public:
        virtual ~BaseLogger() {}

        enum class Verbosity
        {
            Info = 0,
            Warning,
            Error,
            Failure,
            Debug,
        };

        virtual void WriteLine(const Verbosity verbosity, const std::string& line) = 0;

        static std::string VerbositToString(const Verbosity verbosity);
        static std::pair<std::tm, std::chrono::milliseconds> SystemTime();
        static std::string CurrentSystemTime(const std::pair<std::tm, std::chrono::milliseconds>& time, const std::string& format, const bool milliseconds = true, const std::string& separator = ".");
        static std::string CurrentSystemTime(const std::string& format, const bool milliseconds = true, const std::string& separator = ".");
        static std::chrono::time_point<std::chrono::system_clock> ParseToSystemTime(const std::string& value, const std::string& format);

        template <typename T>
        void WriteToStream(T& stream, const Verbosity verbosity, const std::string& line, const std::string& format = DEFAULT_LOG_TIME_FORMAT) const;
    };

    class FileLogger : public BaseLogger
    {
    public:
        struct Settings
        {
            std::wstring LogFilePath = DEFAULT_LOG_FILE_PATH;
            std::wstring LogFileName = DEFAULT_LOG_FILE_NAME;
            std::wstring LogFileExtension = DEFAULT_LOG_FILE_EXTENSION;
            std::wstring LogFilePostFix = {};
            std::string Header = DEFAULT_LOG_FILE_HEADER;
            Size RetentionDays = 2;
            Size MaxLogFileSize = 1024 * 1000 * 10;
            Size MaxLogFiles = 10;

            char Separator = DEFAULT_SEPARATOR;

            inline std::wstring FullPath() const
            {
                return LogFilePath + LogFileName + static_cast<wchar_t>(Separator) + LogFilePostFix + LogFileExtension;
            }
        };

        struct LogFileInfo
        {
            std::wstring FullPath = {};
            std::size_t SizeInBytes = 0u;
            std::size_t Year = 0;
            std::size_t Month = 0;
            std::size_t Day = 0;
            std::size_t Hours = 0;
            std::size_t Minutes = 0;
            double Seconds = 0.0;

            static LogFileInfo FromTime(const std::string& time, const char separator);

            bool operator < (const LogFileInfo& rhs) const;
            std::string ToString() const;
        };

        FileLogger() :
            m_settings(Settings{})
        {
            SetSettings(m_settings);
        }
        ~FileLogger() {};

        void SetSettings(const Settings& settings);
        inline const Settings& GetSettings() const { return m_settings; }

        void WriteLine(const Verbosity verbosity, const std::string& line) override;

    private:
        std::optional<LogFileInfo> IsLogFile(const std::wstring& path);
        void GetAllLogFiles();
        bool GetCurrentLogFile();
        void GenerateNewLogFile();
        void OpenLogFile(const std::wstring& path);
        bool CheckCurrentLogFileSize();
        void CheckLogFilesDate();
        void CheckMaximumNumberOfLogFiles();
        void IncrementLogFile();
        bool IsNewDay() const;

        Settings m_settings;
        std::ofstream m_ostream;
        std::deque<LogFileInfo> m_log_files;
    };

    class ConsoleLogger : public BaseLogger
    {
    public:
        ConsoleLogger() = default;
        ~ConsoleLogger() {}

        void WriteLine(const Verbosity verbosity, const std::string& line) override;
    };

    class Logger : public Singleton<Logger>
    {
    public:
        using LoggerAsyncQueue = AsyncQueue<std::pair<BaseLogger::Verbosity, std::string>>;

        Logger() :
            m_console_logger(std::make_unique<ConsoleLogger>()),
            m_file_logger(std::make_unique<FileLogger>())
        {
        }
        ~Logger() {};
        Logger(const Logger &rhs) = delete;
        Logger(Logger &&rhs) = delete;
        Logger& operator=(const Logger& rhs) = delete;
        Logger& operator=(Logger&& rhs) = default;

        void WriteLine(const BaseLogger::Verbosity verbosity, const std::string& line);

        inline ConsoleLogger& GetConsoleLogger() { return *m_console_logger.get(); }
        inline FileLogger& GetFileLogger() { return *m_file_logger.get(); }
        inline LoggerAsyncQueue& GetQueue() { return m_queue; }

    private:
        std::unique_ptr<ConsoleLogger> m_console_logger;
        std::unique_ptr<FileLogger> m_file_logger;

        LoggerAsyncQueue m_queue = ([&](const auto& line)
        {
            m_console_logger->WriteLine(line.first, line.second);
            m_file_logger->WriteLine(line.first, line.second);
        });
    };

    template <typename T>
    void CreateLine(std::stringstream& ss, T arg)
    {
        ss << arg;
    }

    template <typename T, typename ...Ts>
    void CreateLine(std::stringstream& ss, T arg, Ts... args)
    {
        ss << arg;
        return CreateLine(ss, args...);
    }

    template <typename ...Ts>
    std::string CreateLine(Ts... args)
    {
        std::stringstream ss;
        CreateLine(ss, args...);
        return ss.str();
    }

    template <typename ...Ts>
    void Log(const BaseLogger::Verbosity verbosity, Ts... args)
    {
        Logger::GetInstance().WriteLine(verbosity, CreateLine(args...));
    }
}