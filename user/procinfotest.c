#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int passed = 0;
static int failed = 0;

// Map a numeric procstate to its name, in user space only.
// The kernel stores the state as an int (struct procinfo.state);
// the đề's Usage Example prints it with %s, so the mapping lives here.
static char*
state_name(int state)
{
  switch(state){
  case UNUSED:
    return "UNUSED";
  case USED:
    return "USED";
  case SLEEPING:
    return "SLEEPING";
  case RUNNABLE:
    return "RUNNABLE";
  case RUNNING:
    return "RUNNING";
  case ZOMBIE:
    return "ZOMBIE";
  default:
    return "UNKNOWN";
  }
}

// Print one procinfo record the way the đề's Usage Example does:
// Process, PID/PPID, State (as a name), Memory.
static void
print_procinfo(struct procinfo *pi)
{
  printf("Process: %s\n", pi->name);
  printf("PID: %d, PPID: %d\n", pi->pid, pi->ppid);
  printf("State: %s\n", state_name(pi->state));
  printf("Memory: %ld bytes\n", pi->sz);
}

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
    print_procinfo(&pi);
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

void
test_query_child(void)
{
  struct procinfo pi;
  int cpid = fork();
  if (cpid < 0) {
    printf("FAIL: Fork failed during child query test\n");
    failed++;
    return;
  }
  if (cpid == 0) {
    sleep(30);
    exit(0);
  }
  // Parent asks about the living child: it must be found, with the
  // parent as its PPID, in a live state (sleeping, runnable, running).
  if (procinfo(cpid, &pi) == 0 && pi.pid == cpid &&
      pi.ppid == getpid() &&
      (pi.state == SLEEPING || pi.state == RUNNABLE ||
       pi.state == RUNNING)) {
    print_procinfo(&pi);
    printf("PASS: Parent queried child process correctly\n");
    passed++;
  } else {
    printf("FAIL: Parent could not query child process\n");
    failed++;
  }
  wait(0);
}

void
test_init_ppid(void)
{
  struct procinfo pi;
  // init (PID 1) has no parent, so its PPID must be 0.
  if (procinfo(1, &pi) == 0 && pi.pid == 1 && pi.ppid == 0) {
    print_procinfo(&pi);
    printf("PASS: init (PID 1) has PPID 0\n");
    passed++;
  } else {
    printf("FAIL: init PPID is not 0\n");
    failed++;
  }
}

void
test_zombie(void)
{
  struct procinfo pi;
  int cpid = fork();
  if (cpid < 0) {
    printf("FAIL: Fork failed during zombie test\n");
    failed++;
    return;
  }
  if (cpid == 0) {
    exit(0);
  }
  // Poll until the child has exited but has not been reaped yet.
  // A ZOMBIE child stays in the table until the parent calls wait().
  int seen_zombie = 0;
  for (int i = 0; i < 50; i++) {
    if (procinfo(cpid, &pi) == 0 && pi.state == ZOMBIE) {
      seen_zombie = 1;
      break;
    }
    sleep(1);
  }
  if (seen_zombie) {
    print_procinfo(&pi);
    printf("PASS: Exited child is ZOMBIE until reaped\n");
    passed++;
  } else {
    printf("FAIL: Exited child was not observed as ZOMBIE\n");
    failed++;
  }
  wait(0);
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
  test_query_child();
  test_init_ppid();
  test_zombie();

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
