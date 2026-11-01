#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  struct procinfo pi;
  int pid = getpid();

  printf("Testing procinfo syscall...\n");

  // 1. Kiểm tra tiến trình hiện tại
  if (procinfo(pid, &pi) < 0) {
    printf("FAIL: Cannot get current process info\n");
    exit(1);
  }

  if (pi.pid != pid) {
    printf("FAIL: Incorrect PID\n");
    exit(1);
  }
  if (strcmp(pi.name, "procinfotest") != 0) {
    printf("FAIL: Incorrect process name\n");
    exit(1);
  }
  if (pi.sz == 0) {
    printf("FAIL: Invalid memory size\n");
    exit(1);
  }
  if (pi.state != RUNNING) {
    printf("FAIL: Incorrect process state\n");
    exit(1);
  }

  // 2. Kiểm tra PID không hợp lệ (99999, 0, âm)
  if (procinfo(99999, &pi) != -1 || procinfo(0, &pi) != -1 || procinfo(-1, &pi) != -1) {
    printf("FAIL: Invalid PID accepted\n");
    exit(1);
  }
  printf("PASS: Invalid PID rejected\n");

  // 3. Kiểm tra con trỏ không hợp lệ (NULL và địa chỉ rác)
  if (procinfo(pid, 0) != -1 || procinfo(pid, (struct procinfo *)0xffffffffffffffff) != -1) {
    printf("FAIL: Bad pointers accepted\n");
    exit(1);
  }
  printf("PASS: Bad pointers rejected\n");

  // 4. Kiểm tra trường PPID bằng fork()
  int cpid = fork();
  if (cpid < 0) {
    printf("FAIL: Fork failed\n");
    exit(1);
  }
  if (cpid == 0) {
    struct procinfo cpi;
    if (procinfo(getpid(), &cpi) < 0 || cpi.ppid != pid) {
      printf("FAIL: Child PPID mismatch\n");
      exit(1);
    }
    if (cpi.state != RUNNING) {
      printf("FAIL: Child state mismatch\n");
      exit(1);
    }
    exit(0);
  } else {
    wait(0);
    printf("PASS: PPID verified correctly\n");
  }

  printf("All procinfo tests passed successfully!\n");
  exit(0);
}
