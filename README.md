# Readers–Writers Concurrent Database Simulator
## Team 23 — OS Hackathon
### Project Overview
The Readers–Writers Concurrent Database Simulator is an Operating Systems project developed using the C programming language on Ubuntu. It demonstrates how multiple threads access shared data safely using synchronization mechanisms.

The project also includes demonstrations of operating system concepts such as process management, inter-process communication, memory management, file operations, and thread synchronization.

## Objectives
- Understand process and thread management.
- Demonstrate concurrent access to shared data.
- Prevent conflicting access using synchronization mechanisms.
- Explore inter-process communication.
- Understand memory management and copy-on-write.
- Demonstrate file operations and memory mapping.
- Verify program behavior through automated tests.

## Technologies Used
- **Programming Language:** C
- **Operating System:** Ubuntu Linux
- **Threading:** POSIX Threads (`pthreads`)
- **Synchronization:** Read-write locks and mutexes
- **Process Management:** `fork()`, `exec()`, and `waitpid()`
- **Inter-Process Communication:** Anonymous pipes and signals
- **Memory Management:** Dynamic memory allocation and copy-on-write
- **File Handling:** File I/O and `mmap()`
- **Version Control:** Git and GitHub

## Project Structure

```text
readers-writers-simulator/
├── src/
│   ├── main.c
│   ├── co1_system_info.c
│   ├── co2_process.c
│   ├── co3_ipc.c
│   ├── co4_memory.c
│   ├── co5_file_io.c
│   └── co6_concurrency.c
├── include/
├── tests/
├── docs/
├── presentation/
├── Makefile
├── README.md
└── .gitignore
```

## Project Modules

### Main Module: Readers–Writers Simulator
- Creates five reader threads and two writer threads.
- Initializes the shared database value to `100`.
- Each writer performs three write operations.
- Each write operation increments the database value by `10`.
- Uses `pthread_rwlock_t` to synchronize database access.
- Displays the final database value and verifies the result.

**Expected final database value:**

`100 + (2 × 3 × 10) = 160`

### CO-1: OS as a Service Layer
Demonstrates operating system and system information, including process IDs, kernel details, and machine architecture.

### CO-2: Process Management
Demonstrates process creation, program execution, and parent-child process coordination.

### CO-3: Inter-Process Communication
Demonstrates communication between processes using an anonymous pipe and signals.

### CO-4: Memory Management
Demonstrates dynamic memory allocation and copy-on-write behavior.

### CO-5: File Operations and Memory Mapping
Demonstrates file creation, reading, file metadata, and memory-mapped file access using `mmap()`.

### CO-6: Thread Synchronization and Race Conditions
Demonstrates race conditions and how mutex locks help protect shared counter updates.

## Requirements
- Ubuntu Linux or a compatible Linux environment.
- GCC compiler.
- GNU Make.
- POSIX Threads support.

## How to Compile and Run

### 1. Clone the repository

```bash
git clone https://github.com/hemaswi2008/OSSP-HACKATHON.git
```

### 2. Enter the project directory

```bash
cd OSSP-HACKATHON
```

### 3. Compile the project

```bash
make
```

### 4. Run the main simulator

```bash
./bin/main
```

### 5. Run the automated tests

```bash
make test
```

## Expected Results
- The main simulator should report a final database value of `160`.
- The program should verify the final value against the expected result.
- The automated test suite is designed to check the main simulator and the six CO demonstrations.

Run `make test` to see the actual test results in your environment.

## Future Enhancements
- Accept user-defined configuration values.
- Add more reader and writer threads.
- Record execution statistics and performance metrics.
- Extend the simulator to demonstrate additional synchronization algorithms.
- Improve automated reporting and visualization.

## Conclusion
The Readers–Writers Concurrent Database Simulator provides practical demonstrations of important operating system concepts using C on Linux. It illustrates concurrent execution, synchronization, process management, inter-process communication, memory management, and file operations. The project helps students understand how operating systems coordinate access to shared resources and maintain data consistency.
