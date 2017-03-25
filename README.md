# Scheme

This project is a small implementation of an opinionated Scheme Lisp dialect.

## The REPL

Build the project and start the REPL using
```
make release && ./scheme.out
```

There you can write simple (one-line) programs and get them evaluated immediately.

## Running a script

A script has to begin with
```scheme
(begin <rest> ...)
```
For reference, see the folder examples/ in this repository

## Language

### Values

Possible values are:
    * Numbers, such as 1, 2, -3, 5.456, 7.00, 99e5
    * Strings delimited by parentheses
    * Boolean symbols - ```#t``` and ```#t``` meaning true and false respectively
    * ```()``` signifies the empty list (nil) with a false value in ```if```
    * Symbols with unique names
    * Lists - cons cells
        * Regular lists - the cdr of the last element is ```()```, such as ```(list "hello" "hi")```
        * Dotted lists - the cdr of the last element is not ```()```, such as ```(2 . 3)```

### Basic primitives

```(if <condition> <then> <otherwise>...)``` is the if loop. It evaluates ```<condition>``` and if it's true, it executes ```<then>```. Otherwise it executes ```<otherwise>```.

```eq?``` returns ```#t``` iff the two given objects are same in memory.

```equal?``` returns ```#t``` iff the two given objects are same in value.

```+, -, *``` are the predefined numerical operators.

```cons``` takes two objects and returns a new cons cell with first argument being the ```car``` and the second the ```cdr```.

```car, cdr``` are the accessors for the cons cells.

```write``` writes the object to stdout.

### Variables

You can define a variable using ```define```.

```scheme
(define x (+ 1 (* 5 4))) ; => x = 21
```

### Functions

You can create an unnamed function using ```lambda```

```scheme
(lambda (x) (+ x x))
```

If you want to name your function, you can use ```define``` directly or indirectly.
```scheme
(define square (lambda (x) (+ x x)))
(define (square x) (+ x x))
```

### Other constructs

```begin``` evaluates and executes all its arguments and returns the last result.
```;``` starts a single line comment.
