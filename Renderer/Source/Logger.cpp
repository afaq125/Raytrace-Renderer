#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

namespace
{
    std::vector<std::string> Split(const std::string& text, const char separator)
    {
        std::vector<std::string> results;
        std::istringstream iss(text);

        for (std::string line; std::getline(iss, line, separator);)
        {
            results.push_back(line);
        }

        return results;
    }

    std::string ConvertToString(const std::wstring& wline)
    {
        std::vector<char> line(wline.size());
        std::transform(wline.begin(), wline.end(), line.begin(),
            [](const wchar_t c) { return static_cast<char>(c); });
        return std::string(line.data(), line.size());
    }

    std::wstring ConvertToWString(const std::string& line)
    {
        std::vector<wchar_t> wline(line.size());
        std::transform(line.begin(), line.end(), wline.begin(),
            [](const char c) { return static_cast<wchar_t>(c); });
        return std::wstring(wline.data(), wline.size());
    }

    void RenameFile(const std::wstring& from_file_path, const std::wstring& to_file_path)
    {
        namespace filesystem = std::experimental::filesystem;
        const auto from_path = filesystem::canonical(from_file_path);
        const auto to_path = filesystem::canonical(to_file_path);
        filesystem::rename(from_path, to_path);
    }

    void RemoveFile(const std::wstring& file_path)
    {
        namespace filesystem = std::experimental::filesystem;
        const auto path = filesystem::canonical(file_path);
        filesystem::remove(path);
    }
}

std::string BaseLogger::VerbositToString(const Verbosity verbosity)
{
    switch (verbosity)
    {
    case Verbosity::Info:
        return "INFO   ";
    case Verbosity::Warning:
        return "WARNING";
    case Verbosity::Error:
        return "ERROR  ";
    case Verbosity::Failure:
        return "FAILURE";
    case Verbosity::Debug:
        return "DEBUG  ";
    default:
        return {};
    }
}

std::pair<std::tm, std::chrono::milliseconds> BaseLogger::SystemTime()
{
    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    const std::time_t tt = std::chrono::system_clock::to_time_t(now);
    return { *std::localtime(&tt), ms };
}

std::string BaseLogger::CurrentSystemTime(const std::pair<std::tm, std::chrono::milliseconds>& time, const std::string& format, const bool milliseconds, const std::string& separator)
{
    std::stringstream ss;
    ss << std::put_time(&time.first, format.c_str());
    if (milliseconds)
    {
        ss << separator << std::setfill('0') << std::setw(3) << time.second.count() % 1000;
    }
    return ss.str();
}

std::string BaseLogger::CurrentSystemTime(const std::string& format, const bool milliseconds, const std::string& separator)
{
    const auto time = SystemTime();
    return CurrentSystemTime(time, format, milliseconds, separator);
}

std::chrono::time_point<std::chrono::system_clock> BaseLogger::ParseToSystemTime(const std::string& value, const std::string& format)
{
    std::tm tm = {};
    std::stringstream ss(value);
    ss >> std::get_time(&tm, format.c_str());
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));;
}

template <typename T>
void BaseLogger::WriteToStream(T& stream, const Verbosity verbosity, const std::string& line, const std::string& format) const
{
    stream << CurrentSystemTime(format) << ' ' << VerbositToString(verbosity) << ' ' << line << std::endl;
}

FileLogger::LogFileInfo FileLogger::LogFileInfo::FromTime(const std::string& time, const char separator)
{
    const auto parts = Split(time, separator);

    FileLogger::LogFileInfo result = {};
    result.Year = std::stoi(parts[0]);
    result.Month = std::stoi(parts[1]);
    result.Day = std::stoi(parts[2]);
    result.Hours = std::stoi(parts[3]);
    result.Minutes = std::stoi(parts[4]);
    result.Seconds = std::stod(parts[5]);
    return result;
}

bool FileLogger::LogFileInfo::operator < (const FileLogger::LogFileInfo& rhs) const
{
    const auto time = BaseLogger::ParseToSystemTime(ToString(), DEFAULT_LOG_FILE_HEADER_TIME_FORMAT);
    const auto time_rhs = BaseLogger::ParseToSystemTime(rhs.ToString(), DEFAULT_LOG_FILE_HEADER_TIME_FORMAT);
    return time < time_rhs;
}

std::string FileLogger::LogFileInfo::ToString() const
{
    return CreateLine(
        Year, DEFAULT_SEPARATOR, 
        Month, DEFAULT_SEPARATOR, 
        Day, DEFAULT_SEPARATOR, 
        Hours, DEFAULT_SEPARATOR, 
        Minutes, DEFAULT_SEPARATOR, 
        Seconds);
}

void FileLogger::SetSettings(const Settings& settings)
{
    m_settings = settings;

    ASSERT(m_settings.LogFilePath.empty(), "Logger header cannot be empty.");
    ASSERT(m_settings.LogFileName.empty(), "Logger file name cannot be empty.");
    ASSERT(m_settings.LogFileExtension.empty(), "Logger file extension cannot be empty.");
    ASSERT(m_settings.Header.empty(), "Logger header cannot be empty.");
    ASSERT(m_settings.RetentionDays < 1, "Logger retention days must be 1 or more.");
    ASSERT(m_settings.MaxLogFileSize < 64, "Logger file size must be 64 bytes or more.");
    ASSERT(m_settings.MaxLogFiles < 1, "Logger max log files days must be 1 or more.");

    GetAllLogFiles();
    if (!GetCurrentLogFile())
    {
        GenerateNewLogFile();
    }
    CheckLogFilesDate();
    CheckMaximumNumberOfLogFiles();
}

std::optional<FileLogger::LogFileInfo> FileLogger::IsLogFile(const std::wstring& path)
{
    std::ifstream istream;
    istream.open(path, std::ios::in | std::ios::app);
    if (istream.is_open())
    {
        std::string line = {};
        std::getline(istream, line);
        if (line.find(m_settings.Header) != std::string::npos)
        {
            istream.seekg(0, std::ios::end);

            // Plus 1 for separator.
            const std::string time_stamp = line.substr(m_settings.Header.length() + 1);
            auto info = LogFileInfo::FromTime(time_stamp, '_');
            info.FullPath = path;
            info.SizeInBytes = static_cast<std::size_t>(istream.tellg());
            return info;
        }
    }
    return {};
}

void FileLogger::GetAllLogFiles()
{
    namespace filesystem = std::experimental::filesystem;

    m_log_files.clear();

    for (const auto& file : filesystem::directory_iterator(m_settings.LogFilePath))
    {
        const std::wstring& name = file.path().stem().wstring();
        const std::wstring& extension = file.path().extension().wstring();
        if (name.find(m_settings.LogFileName) != std::wstring::npos &&
            extension.find(m_settings.LogFileExtension) != std::wstring::npos)
        {
            const auto& fullpath = file.path().wstring();
            const auto is_log_file = IsLogFile(fullpath);
            if (is_log_file.has_value())
            {
                m_log_files.push_back(is_log_file.value());
            }
        }
    }
    std::sort(m_log_files.rbegin(), m_log_files.rend());
}

bool FileLogger::GetCurrentLogFile()
{
    if (m_log_files.empty())
    {
        return false;
    }

    const auto& log_file_info = m_log_files.back();
    const auto current_time = SystemTime().first;
    if (log_file_info.Year == (current_time.tm_year + 1900) &&
        log_file_info.Month == (current_time.tm_mon + 1) &&
        log_file_info.Day == current_time.tm_mday)
    {
        OpenLogFile(log_file_info.FullPath);
        return true;
    }
    return false;
}

void FileLogger::GenerateNewLogFile()
{
    const auto time = BaseLogger::SystemTime();
    const auto time_stamp = BaseLogger::CurrentSystemTime(time, DEFAULT_LOG_FILE_HEADER_TIME_FORMAT, true);
    const auto time_stamp_filename = BaseLogger::CurrentSystemTime(time, DEFAULT_LOG_FILE_FORMAT, false);

    m_settings.LogFilePostFix = ConvertToWString(time_stamp_filename);
    const auto full_path = m_settings.FullPath();

    OpenLogFile(full_path);

    m_ostream
        << m_settings.Header
        << m_settings.Separator
        << time_stamp << std::endl;

    auto info = LogFileInfo::FromTime(time_stamp, '_');
    info.FullPath = full_path;
    info.SizeInBytes = static_cast<std::size_t>(m_ostream.tellp());
    m_log_files.push_front(info);
}

void FileLogger::OpenLogFile(const std::wstring& path)
{
    if (m_ostream.is_open())
    {
        m_ostream.close();
    }

    m_ostream.open(path, std::ios::out | std::ios::app);
    m_ostream.exceptions(std::ofstream::badbit | std::ofstream::failbit);

    if (m_ostream.fail())
    {
        throw std::runtime_error(std::string("Failed to open logfile: ") + ConvertToString(path));
    }
}

bool FileLogger::CheckCurrentLogFileSize()
{
    auto& info = m_log_files.front();
    info.SizeInBytes = m_ostream.tellp();

    if (info.SizeInBytes < m_settings.MaxLogFileSize)
    {
        return false;
    }

    const auto time_stamp = BaseLogger::CurrentSystemTime(DEFAULT_LOG_FILE_HEADER_TIME_FORMAT, false);
    m_settings.LogFilePostFix = ConvertToWString(time_stamp);

    const auto current_path = info.FullPath;
    const auto new_path = m_settings.FullPath();
    info.FullPath = new_path;

    m_ostream.close();
    RenameFile(current_path, new_path);
    GenerateNewLogFile();

    return true;
}

void FileLogger::CheckLogFilesDate()
{
    const auto current_time = SystemTime().first;
    const auto file_out_of_date = [&](const auto& info)
    {
        return info.Year < (current_time.tm_year + 1900) ||
            info.Month < (current_time.tm_mon + 1) ||
            info.Day < current_time.tm_mday;
    };

    const auto remove_dated_file = [&](const auto& info)
    {
        const bool out_of_date = file_out_of_date(info);
        if (out_of_date)
        {
            RemoveFile(info.FullPath);
        }
        return out_of_date;
    };

    const bool current_file_out_of_date = file_out_of_date(m_log_files.front());
    if (current_file_out_of_date)
    {
        m_ostream.close();
    }

    m_log_files.erase(m_log_files.rend().base(),
        std::remove_if(m_log_files.rbegin(), m_log_files.rend(), remove_dated_file).base());

    if (current_file_out_of_date)
    {
        GenerateNewLogFile();
    }
}

void FileLogger::CheckMaximumNumberOfLogFiles()
{
    if (m_log_files.size() <= m_settings.MaxLogFiles)
    {
        return;
    }

    for (std::size_t i = m_settings.MaxLogFiles; i < m_log_files.size(); ++i)
    {
        RemoveFile(m_log_files[i].FullPath);
    }

    m_log_files.erase(m_log_files.begin() + m_settings.MaxLogFiles);
}

void FileLogger::IncrementLogFile()
{
    if (CheckCurrentLogFileSize())
    {
        CheckMaximumNumberOfLogFiles();
    }
    if (IsNewDay())
    {
        CheckLogFilesDate();
    }
}

bool FileLogger::IsNewDay() const
{
    const auto time = BaseLogger::CurrentSystemTime(DEFAULT_LOG_FILE_HEADER_TIME_FORMAT);
    const auto log_file_info = m_log_files.front();
    const auto time_info = LogFileInfo::FromTime(time, m_settings.Separator);
    return log_file_info < time_info;
}

void FileLogger::WriteLine(const Verbosity verbosity, const std::string& line)
{
    WriteToStream(m_ostream, verbosity, line);
    IncrementLogFile();
}

void ConsoleLogger::WriteLine(const Verbosity verbosity, const std::string& line)
{
    WriteToStream(std::cout, verbosity, line);
}

void Logger::WriteLine(const BaseLogger::Verbosity verbosity, const std::string& line)
{
    m_queue.Push(line);
}