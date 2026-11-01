#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int passed = 0;
static int failed = 0;

void
test_pid_le_zero(void)
{
  struct procinfo pi;
  if (procinfo(0, &pi) == -1 && procinfo(-1, &pi) == -1 && procinfo(-999, &pi) == -1) {
    printf("PASS: PID <= 0 rejected correctly\n");
    passed++;
  } else {
    printf("FAIL: PID <= 0 accepted\n");
    failed++;
  }
}

void
test_nonexistent_pid(void)
{
  struct procinfo pi;
  if (procinfo(99999, &pi) == -1) {
    printf("PASS: Nonexistent PID rejected correctly\n");
    passed++;
  } else {
    printf("FAIL: Nonexistent PID accepted\n");
    failed++;
  }
}

void
test_invalid_pointer(void)
{
  int pid = getpid();
  if (procinfo(pid, 0) == -1 &&
      procinfo(pid, (struct procinfo *)0xffffffffffffffff) == -1) {
    printf("PASS: Invalid user-space pointers rejected correctly\n");
    passed++;
  } else {
    printf("FAIL: Invalid pointer accepted\n");
    failed++;
  }
}

void
test_valid_procinfo(void)
{
  struct procinfo pi;
  int pid = getpid();
  if (procinfo(pid, &pi) == 0 && pi.sz > 0 && pi.state == RUNNING) {
    printf("PASS: Valid process information retrieved correctly\n");
    passed++;
  } else {
    printf("FAIL: Failed to get valid process information\n");
    failed++;
  }
}

void
test_pid_and_name(void)
{
  struct procinfo pi;
  int pid = getpid();
  if (procinfo(pid, &pi) == 0 && pi.pid == pid && strcmp(pi.name, "procinfotest") == 0) {
    printf("PASS: Correct PID and process name verified\n");
    passed++;
  } else {
    printf("FAIL: PID or process name mismatch\n");
    failed++;
  }
}

void
test_ppid(void)
{
  int ppid = getpid();
  int cpid = fork();
  if (cpid < 0) {
    printf("FAIL: Fork failed during PPID test\n");
    failed++;
    return;
  }
  if (cpid == 0) {
    struct procinfo cpi;
    if (procinfo(getpid(), &cpi) == 0 && cpi.ppid == ppid && cpi.state == RUNNING) {
      exit(0);
    } else {
      exit(1);
    }
  } else {
    int status = 0;
    wait(&status);
    if (status == 0) {
      printf("PASS: Correct PPID verified via fork\n");
      passed++;
    } else {
      printf("FAIL: PPID or child state mismatch\n");
      failed++;
    }
  }
}

int
main(void)
{
  passed = 0;
  failed = 0;

  printf("Starting procinfotest...\n");

  test_pid_le_zero();
  test_nonexistent_pid();
  test_invalid_pointer();
  test_valid_procinfo();
  test_pid_and_name();
  test_ppid();

  printf("=== Test Summary ===\n");
  printf("Passed: %d, Failed: %d\n", passed, failed);

  if (failed == 0) {
    printf("All procinfo tests passed successfully!\n");
    exit(0);
  } else {
    printf("procinfotest FAILED with %d failure(s)!\n", failed);
    exit(1);
  }
}
