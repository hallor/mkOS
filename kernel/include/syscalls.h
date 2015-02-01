#ifndef SYSCALLS_H
#define SYSCALLS_H
struct task;
void do_syscall(int no, struct task *task);
#endif // SYSCALLS_H
