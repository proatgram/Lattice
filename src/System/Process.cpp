module;

#if defined(__unix__) || defined(__apple__)

#include <unistd.h>
#include <sys/wait.h>

#endif

#include <cstdlib>
#include <cerrno>
#include <cstring>

module Lattice.System.Process;

using namespace Lattice::System;

namespace Lattice::Internal {
#if defined(__unix__) || defined(__APPLE__)
    template <typename CharT, auto BufSize = 4096>
    class basic_fdstreambuf : public std::basic_streambuf<CharT, std::char_traits<CharT>> {
        public:
            using char_type = CharT;
            using traits_type = std::char_traits<CharT>;
            using int_type = typename traits_type::int_type;

            basic_fdstreambuf(int fd, std::ios_base::openmode openMode = std::ios_base::in | std::ios_base::out) : std::basic_streambuf<char>(), m_fd(fd) {
                this->setp(m_put.data(), m_put.data() + BufSize);
            }

        protected:
            virtual auto overflow(int_type ch = traits_type::eof())  -> int_type override {
                if (this->pbase() == nullptr) {
                    if (traits_type::eq_int_type(ch, traits_type::eof()))
                        return traits_type::not_eof(ch);
                    if (write(m_fd, &ch, 1) == -1)
                        return traits_type::eof();

                    return ch;
                }

                std::size_t toConsume = this->pptr() - this->pbase();
                
                // Safe to do - we allocate the buffer 1 extra than what we tell streambuf.
                if (!traits_type::eq_int_type(ch, traits_type::eof())) {
                    toConsume++;
                    *(this->pptr() + 1) = ch;
                }

                std::size_t written{0};
                if (written = write(m_fd, m_put.data(), toConsume); written == -1)
                    return traits_type::eof();

                std::size_t notConsumed = toConsume - written;

                // If everything, including a possible value from ch is written (not not-consumed).
                if (!notConsumed) {
                    this->setp(m_put.data(), m_put.data() + BufSize);
                    return (traits_type::eq_int_type(ch, traits_type::eof()) ? traits_type::not_eof(ch) : ch);
                }

                // If not, we copy the unwritten portion to the beginning of the buffer.
                std::memcpy(m_put.data(), (this->pbase() + toConsume) - notConsumed, notConsumed);

                // And then update the pointers accordingly.
                this->setp(m_put.data(), m_put.data() + BufSize);
                this->pbump(notConsumed);

                return (traits_type::eq_int_type(ch, traits_type::eof()) ? traits_type::not_eof(ch) : ch);
            }

            virtual auto xsputn(const char_type *s, std::streamsize count) -> std::streamsize override {
                int written{0};
                for (std::streamsize i = 0; i < count; i++) {
                    if (traits_type::eq_int_type(this->sputc(s[i]), traits_type::eof()))
                        break;
                    written++;
                }

                return written;
            }

            virtual auto underflow() -> int_type override {
                size_t amountRead = read(m_fd, m_get.data(), BufSize);

                if (amountRead == 0 || amountRead == -1)
                    return traits_type::eof();

                this->setg(m_get.data(), m_get.data(), m_get.data() + amountRead);

                return traits_type::to_int_type(*this->gptr());
            }

        private:
            int m_fd;

            std::array<char_type, BufSize> m_get;
            std::array<char_type, BufSize + 1> m_put;
    };

    template <typename CharT>
    class basic_fdistream : public std::basic_istream<CharT> {
        public:
            basic_fdistream(int fd) : std::basic_istream<CharT>(new basic_fdstreambuf<CharT>(fd, std::ios_base::in)) {}

            virtual ~basic_fdistream() {
                delete this->rdbuf();
            }
    };

    template <typename CharT>
    class basic_fdostream : public std::basic_ostream<CharT> {
        public:
            basic_fdostream(int fd) : std::basic_ostream<CharT>(new basic_fdstreambuf<CharT>(fd, std::ios_base::out)) {}

            virtual ~basic_fdostream() {
                delete this->rdbuf();
            }
    };

#endif
}  // namespace Lattice::Tooling::Internal

Process::Process(Constructable) {}

auto Process::Spawn(const Command &command) -> std::expected<std::shared_ptr<Process>, std::string> {

    std::shared_ptr<Process> process = std::make_shared<Process>(Constructable());
    process->m_command = command;


#if defined(__unix__) || defined (__APPLE__)

    // First, define pipes for the stdout and stdin of the new process.
    int processOutPipes[2];
    int processInPipes[2];
    int processErrPipes[2];

    if (pipe(processOutPipes))
        return std::unexpected(std::strerror(errno));
    if (pipe(processInPipes))
        return std::unexpected(std::strerror(errno));
    if (pipe(processErrPipes))
        return std::unexpected(std::strerror(errno));


    // Fork into new duplicated child proc.
    pid_t newPid = fork();

    if (newPid == -1)
        return std::unexpected(std::format("Fork failed: {}", std::strerror(errno)));

    if (newPid == 0) {
        // Don't need to read from output or write to input
        // of this process.
        close(processOutPipes[0]);
        close(processInPipes[1]);
        close(processErrPipes[0]);

        int newStdout = processOutPipes[1];
        int newStdin = processInPipes[0];
        int newStderr = processErrPipes[1];

        // Redirect new process's io streams to parent process pipes.
        if (dup2(newStdout, STDOUT_FILENO) == -1) {
            std::cerr << "Failed to redirect to new stdout." << std::endl;
            exit(1);
        }
        if (dup2(newStdin, STDIN_FILENO) == -1) {
            std::cerr << "Failed to redirect to new stdin." << std::endl;
            exit(1);
        }
        if (dup2(newStderr, STDERR_FILENO) == -1) {
            std::cerr << "Failed to redirect to new stderr." << std::endl;
            exit(1);
        }

        // Change working directory for the new process.
        if (command.GetWorkingDirectory().has_value())
            chdir(command.GetWorkingDirectory().value().c_str());


        
        std::size_t argc = command.GetArguments().size() + 2;
        char *argv[argc];

        std::string exec = command.GetExecutable();
        std::vector<std::string> args = command.GetArguments();
        argv[0] = exec.data();
        argv[argc] = nullptr;
        for (std::size_t i = 1; i < argc - 1; i++) {
            argv[i] = args[i - 1].data();
        }

        if (command.GetEnvironments().empty()) {
            // This process ends here.
            if (execvp(command.GetExecutable().c_str(), argv) == -1) {
                std::cerr << std::strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            std::size_t environCount{0};
            while (environ[environCount]) {
                environCount++;
            }

            char **envp = new char*[environCount + command.GetEnvironments().size() + 1];
            for (std::size_t i = 0; i < environCount; i++) {
                envp[i] = strdup(environ[i]);
            }

            std::size_t count{0};
            for (std::unordered_map<std::string, std::string>::const_iterator it = std::cbegin(command.GetEnvironments()); it != std::cend(command.GetEnvironments()); ++it) {
                envp[environCount + count] = strdup(std::format("{}={}", it->first, it->second).c_str());
                count++;
            }

            envp[environCount + command.GetEnvironments().size()] = nullptr;

            if (execvpe(command.GetExecutable().c_str(), argv, envp) == -1) {
                std::cerr << std::strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    process->m_startTime.store(std::chrono::steady_clock::now());

    auto pollingFunc = [process](const std::stop_token &stopToken) -> void {
        std::lock_guard<std::mutex> lock(process->m_pollingMutex);

        while (true) {
#if defined(__unix__) || defined (__APPLE__)
            pid_t processPid;
            int badCastCount{0};
            try {
                processPid = std::any_cast<pid_t>(process->m_systemHandle);
            } catch (const std::bad_any_cast &err) {
                if (badCastCount >= 3)
                    throw std::runtime_error(std::format("Failed to cast process handle to native handle after {} tries.", badCastCount));
                badCastCount++;
                continue;

            }

            if (stopToken.stop_requested()) {
                kill(processPid, SIGTERM);
            }

            int status{};
            int ret = waitpid(processPid, &status, WNOHANG);


            if (ret != 0 && ret != -1) {
                if (WIFEXITED(status)) {
                    process->m_exitCode.store(WEXITSTATUS(status));
                    break;
                } else {
                    // TODO: Handle other exit cases for subprocess

                    break;
                }
            } else if (ret == -1) {
                throw std::runtime_error(std::format("Failed to poll for sub-process termination (waitpid returned {}).", ret));
            }
#elif defined(_WIN32)
            HANDLlE processHandle = std::any_cast<HANDLE>(process->m_systemHandle);
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        process->m_endTime.store(std::chrono::steady_clock::now());
        process->m_executionTime.store(process->m_endTime.load() - process->m_startTime.load());
        process->m_isFinished.store(true);

        process->m_pollingConditionVariable.notify_all();
    };

    process->m_pollingThread = std::jthread(pollingFunc);

    // Don't need to read from output or write to input
    // of this process.
    close(processOutPipes[1]); close(processInPipes[0]); close(processErrPipes[1]);

    int processStdOutFd = processOutPipes[0];
    int processStdInFd = processInPipes[1];
    int processStdErrFd = processErrPipes[0];

    process->m_processStdIn = std::unique_ptr<std::basic_ostream<char>>(new Internal::basic_fdostream<char>(processStdInFd));
    process->m_processStdOut = std::unique_ptr<std::basic_istream<char>>(new Internal::basic_fdistream<char>(processStdOutFd));
    process->m_processStdErr = std::unique_ptr<std::basic_istream<char>>(new Internal::basic_fdistream<char>(processStdErrFd));
    
    process->m_systemHandle = newPid;

#elif defined(_WIN32)

#endif

    return std::expected<std::shared_ptr<Process>, std::string>{process};
}

auto Process::GetExitCode() const -> std::optional<int> {
    if (!m_isFinished.load())
        return {};

    return m_exitCode.load();
}

auto Process::GetStartTime() const -> std::chrono::steady_clock::time_point {
    return m_startTime;
}

auto Process::GetEndTime() const -> std::optional<std::chrono::steady_clock::time_point> {
    if (!m_isFinished.load())
        return {};

    return m_endTime.load();
}

auto Process::GetExecutionTime() const -> std::optional<std::chrono::steady_clock::duration> {
    if (!m_isFinished.load())
        return {};

    return m_executionTime.load();
}

auto Process::GetCommand() const -> Command {
    return m_command;
}

auto Process::Wait(const std::optional<std::chrono::system_clock::duration> waitTime) const -> bool {
    std::unique_lock<std::mutex> lock(m_pollingMutex);

    if (waitTime.has_value()) {
        return m_pollingConditionVariable.wait_for(lock, waitTime.value(), [this]() -> bool {return m_isFinished.load();});
    }


    m_pollingConditionVariable.wait(lock, [this]() -> bool {return m_isFinished.load();});

    return true;
}

auto Process::IsFinished() const -> bool {
    return m_isFinished.load();
}

auto Process::RequestCancel() -> bool {
    return m_pollingThread.request_stop();
}

auto Process::GetProcessStdOut() -> std::istream& {
    return *m_processStdOut;
}

auto Process::GetProcessStdIn() -> std::ostream& {
    return *m_processStdIn;
}

auto Process::GetProcessStdErr() -> std::istream& {
    return *m_processStdErr;
}
