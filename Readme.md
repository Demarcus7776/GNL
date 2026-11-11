*This project has been created as part of the 42 curriculum by mohachad.*

# get_next_line

A function that reads a file descriptor and hands back **one line at a time**, no matter how big the file is or how small the read buffer is.

## Description

`get_next_line` is one of the first "real" C projects at 42. The prototype is:

```c
char *get_next_line(int fd);
```

Each call returns the next line from `fd`, including the trailing `\n` when there is one. When there is nothing left to read (or an error happens), it returns `NULL`.

It sounds small, but it forces you to deal with a few things that are easy to get wrong:

- `read()` gives you **chunks of bytes**, not lines. A chunk can contain half a line, exactly one line, or five lines.
- Whatever you read but don't use yet has to survive between calls. That means a **static variable**.
- The buffer size is set at compile time (`-D BUFFER_SIZE=n`) and the code must work for `1`, for `42`, and for `10000000`.
- No leaks. Every `malloc` has a `free`, including on the error paths.

Allowed functions: `read`, `malloc`, `free`.

### Files

| File | Role |
|------|------|
| `get_next_line.c` | main function and the logic around the leftover buffer |
| `get_next_line_utils.c` | small string helpers (length, search for `\n`, join, copy) |
| `get_next_line.h` | prototypes and the `BUFFER_SIZE` default |

If you did the bonus: `get_next_line_bonus.c`, `get_next_line_utils_bonus.c` and `get_next_line_bonus.h`, which handle several file descriptors at once.

## Instructions

### Compile

There's no Makefile for this project, so you compile it together with your own `main`:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
```

Change `BUFFER_SIZE` to test other values. If you don't pass it, the header uses a default.

### Run

A tiny `main.c` to try it:

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	if (argc != 2)
		return (1);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

```bash
./gnl some_file.txt
```

It also works with standard input: just pass `0` as the file descriptor.

### Things worth testing

- empty file
- file with no `\n` at the end
- file with only `\n` characters
- very long line (longer than `BUFFER_SIZE` many times over)
- `BUFFER_SIZE=1`, a huge `BUFFER_SIZE`
- invalid fd (`-1`) and a closed fd
- `valgrind --leak-check=full ./gnl file.txt`

## Algorithm

The idea is to keep a **static "stash"** that holds whatever has been read from the fd but not yet returned.

### How a call works

1. **Fill the stash.** Allocate a buffer of `BUFFER_SIZE + 1`, `read()` into it, null-terminate, and append it to the stash. Repeat until the stash contains a `\n` or `read()` returns `0` (end of file).
2. **Cut out the line.** Copy everything from the start of the stash up to and including the first `\n` (or to the end if there is none) into a new string. That's what gets returned.
3. **Keep the rest.** Whatever comes after the `\n` becomes the new stash. If nothing is left, free the stash and set it to `NULL`.
4. **Errors.** If `read()` fails (returns `-1`), free the stash and return `NULL`.

### Why this approach

- **A static variable is the only clean way to keep state** between calls without global variables, which are forbidden.
- **Reading only until a `\n` shows up** means the function never loads a whole file in memory. A 2 GB file and a 2 KB file behave the same, we only ever hold about one line plus one buffer.
- **Searching for `\n` after every read** (not after the whole file) keeps it fast and also works on stdin, where the file never "ends" until the user decides it does.
- **Allocating exactly what's needed** for each line and for the leftover keeps memory usage honest, and makes leak-hunting easier since every string has a clear owner.

The cost: appending to the stash with a join means copying the data again each time. With a very small `BUFFER_SIZE` and a very long line, that gets slow (quadratic-ish). I accepted that trade for simplicity, since the subject cares more about correctness and leaks than about raw speed.

### Bonus: several file descriptors

For the bonus, the single static pointer is replaced by an **array of stashes indexed by fd** (for example `static char *stash[FD_MAX]`). That way, reading a line from fd 3, then a line from fd 4, then going back to fd 3 doesn't mix their content.

## Resources

**References**

- `man 2 read`, `man 3 malloc`, `man 3 free`
- [Static variables in C](https://en.wikipedia.org/wiki/Static_variable)
- [Valgrind quick start](https://valgrind.org/docs/manual/quick-start.html)
- The subject PDF
- Talks with other students at 1337 about edge cases (BUFFER_SIZE=1, no trailing newline)

**Use of AI**

- _Example: asked an AI assistant to explain how static variables keep their value between calls._
- _Example: used it to come up with test cases (empty file, no final newline, etc.)._
- _Example: used it to help structure and proofread this README._
- All the code was written, tested and debugged by me.