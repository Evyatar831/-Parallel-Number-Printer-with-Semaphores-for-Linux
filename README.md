 Semaphores for Linux

This C program demonstrates parallel processing in Linux by printing numbers from 1 to 100 using 5 synchronized processes and POSIX semaphores.

## Features:
- Utilizes 5 child processes, each responsible for printing every 5th number
- Implements 5 POSIX semaphores for precise inter-process synchronization
- Each process waits on its designated semaphore before printing
- After printing, each process 'signals' the other 4 semaphores
- Showcases Linux-specific parallel processing and IPC mechanisms
