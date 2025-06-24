# Multithreaded-Matrix-Multiplication

This project implements **matrix multiplication using multithreading in C** with two different strategies:

1. `naive_multithreaded.c` – One thread per matrix element.
2. `task_queue_multithreaded.c` – A thread pool model using task queues.

Both versions compare their performance against a sequential baseline and compute the **speedup** achieved through parallelization.

---

## 🚀 Features

- User-input driven dynamic matrix dimensions and values.
- **Sequential matrix multiplication** for baseline performance.
- Two parallel strategies:
  - **Naive multithreading**: Spawns `m × p` threads (one per output matrix element).
  - **Task queue multithreading**: Uses a bounded queue and 6 persistent worker threads.
- **Thread synchronization** with `pthread_mutex_t` and `pthread_cond_t`.
- Performance measurement using `clock_t`.
- Final speedup displayed after execution.

---

## 🧪 Files

| File                       | Description                                                |
|----------------------------|------------------------------------------------------------|
| `naive_multithreaded.c`    | Naive implementation spawning one thread per element.      |
| `task_queue_multithreaded.c` | Optimized thread-pool model using a shared task queue.     |

---

## 🧰 Technologies Used

- **Language**: C
- **Libraries**: `pthread.h`, `stdlib.h`, `stdio.h`, `time.h`
- **Concepts**: Thread creation and joining, mutexes, condition variables, task queues, performance benchmarking

---

## 🧑‍💻 How to Compile and Run

Make sure you have GCC and POSIX threads support.

### Compile

```bash
gcc -o naive naive_multithreaded.c -lpthread
gcc -o taskqueue task_queue_multithreaded.c -lpthread
