
#!/bin/bash

# Team 23 - Readers-Writers OS Hackathon Test Suite

set -u

PASS_COUNT=0
FAIL_COUNT=0

run_test() {
    name="$1"
    executable="$2"
    log_file="/tmp/os-test-${executable}.log"

    echo ""
    echo "Testing: $name"

    if ! timeout 15s "./bin/$executable" > "$log_file" 2>&1; then
        status=$?
        # Preserve a useful failure report.
        echo "FAIL: Program execution failed or timed out"
        cat "$log_file"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        return
    fi

    local_failed=0

    case "$executable" in
        readers_writers)
            grep -Fq "Final value     : 160" "$log_file" || local_failed=1
            grep -Fq "Verification    : PASS" "$log_file" || local_failed=1
            ;;

        co1_system_info)
            grep -Fq "Operating System: Linux" "$log_file" || local_failed=1
            grep -Fq "Kernel Release:" "$log_file" || local_failed=1
            grep -Fq "Machine Architecture:" "$log_file" || local_failed=1
            grep -Fq "Online Processors:" "$log_file" || local_failed=1
            ;;

co2_process)
    grep -Fq "Parent waiting for child PID:" "$log_file" || local_failed=1
    grep -Fq "total " "$log_file" || local_failed=1
    grep -Fq "Child exited with status: 0" "$log_file" || local_failed=1
    grep -Fq "Parent process completed." "$log_file" || local_failed=1
    ;;
        co3_ipc)
            grep -Fq "Message received: Hello from parent through an anonymous pipe!" "$log_file" || local_failed=1
            grep -Fq "Child completed successfully." "$log_file" || local_failed=1
            grep -Fq "Parent received SIGUSR1 from child." "$log_file" || local_failed=1
            ;;

        co4_memory)
            grep -Fq "Child changed data[0] to 999" "$log_file" || local_failed=1
            grep -Fq "Parent still sees data[0] = 10" "$log_file" || local_failed=1
            grep -Fq "Dynamic memory released." "$log_file" || local_failed=1
            ;;

        co5_file_io)
            grep -Fq "File created: database_record.txt" "$log_file" || local_failed=1
            grep -Fq "Content read using read():" "$log_file" || local_failed=1
            grep -Fq "Content accessed through mmap():" "$log_file" || local_failed=1
            grep -Fq "File descriptor closed successfully." "$log_file" || local_failed=1
            grep -Fq "Readers-Writers Database Simulator" "$log_file" || local_failed=1
            ;;

        co6_concurrency)
            grep -Fq "Result                   : Data race demonstrated" "$log_file" || local_failed=1
            grep -Fq "Observed counter         : 400000" "$log_file" || local_failed=1
            grep -Fq "Verification             : PASS" "$log_file" || local_failed=1
            grep -Fq "Mutex prevented lost updates." "$log_file" || local_failed=1
            ;;

        *)
            echo "FAIL: Unknown test target"
            FAIL_COUNT=$((FAIL_COUNT + 1))
            return
            ;;
    esac

    if [ "$local_failed" -eq 0 ]; then
        echo "PASS: All expected checks passed"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "FAIL: One or more expected checks failed"
        echo "---- Program output ----"
        cat "$log_file"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
}

run_test "Readers-Writers Synchronization" "readers_writers"
run_test "System Information and OS Services" "co1_system_info"
run_test "Process Management" "co2_process"
run_test "Inter-Process Communication" "co3_ipc"
run_test "Memory Management and Copy-on-Write" "co4_memory"
run_test "File Operations and Memory Mapping" "co5_file_io"
run_test "Thread Synchronization and Race Conditions" "co6_concurrency"

echo ""
echo "========================================"
echo "          PROJECT TEST SUMMARY"
echo "========================================"
echo "Passed: $PASS_COUNT"
echo "Failed: $FAIL_COUNT"
echo "Total : $((PASS_COUNT + FAIL_COUNT))"

if [ "$FAIL_COUNT" -eq 0 ]; then
    echo "OVERALL RESULT: PASS"
    exit 0
else
    echo "OVERALL RESULT: FAIL"
    exit 1
fi
