## Type conversions
These serve as a way to convert between primitive types to satisfy the type checker.

#### `fn char(int) -> char`
Converts from `int` to `char`

#### `fn int(char) -> int`
Converts from `char` to `int`

#### `fn int(double) -> int`
Converts from `double` to `int`

#### `fn double(int) -> double`
Converts from `int` to `double`

## IO

#### `fn get_char() -> char`
Gets a character from stdin.

#### `fn get_line() -> string`
Gets a line of input from stdin.

#### `fn println() -> void`
Output a newline to stdout

#### `fn println(int) -> void`
Output an `int` followed by a newline to stdout

#### `fn println(char) -> void`
Output a `char` followed by a newline to stdout

#### `fn println(bool) -> void`
Output a `bool` (true or false) followed by a newline to stdout

#### `fn println(double) -> void`
Output a `double` followed by a newline to stdout

#### `fn println(buffer) -> void`
Output a `buffer` followed by a newline to stdout

#### `fn println(string) -> void`
Output a `string` followed by a newline to stdout

#### `fn print(int) -> void`
Output an `int` to stdout

#### `fn print(char) -> void`
Output a `char` to stdout

#### `fn print(bool) -> void`
Output a `bool` (true or false) to stdout

#### `fn print(double) -> void`
Output a `double` to stdout

#### `fn print(buffer) -> void`
Output a `buffer` to stdout

#### `fn print(string) -> void`
Output a `string` to stdout


## Tuning/Debugging

#### `fn stack_trace() -> void`
Output the stacktrace from the current state of the malangVM

#### `fn breakpoint() -> void`
When compiled with `USE_COMPUTED_GOTO=0` this will break into an interactive malangVM debugger
mode, otherwise the malangVM aborts.

#### `fn gc_pause() -> void`
Disable the GC from running automatically

#### `fn gc_resume() -> void`
Enable the GC to run automatically

#### `fn gc_run() -> void`
Manually force a garbage collection
