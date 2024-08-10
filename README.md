# mlisj

Minimal Lisp Interpreter for SKK-JISYO.

## SYNOPSIS

A minimal interpreter that evaluates Lisp expressions within SKK-JISYO.
It is intended to be used as a library.

## USAGE

In C:


```
#include <mlisj.h>

int mlisj_eval(const char *src, char *dest, size_t size, char **skk_num_list, const char *skk_henkan_key)
```

### ARGUMENTS

* `src`
     * String of S-expression to evaluate.
* `dest`
     * Buffer where evaluation results are written.
* `size`
     * Maximum size of `dest`.
* `skk_num_list`
     * List of numbers to be passed in numeric conversion.
     * Array of strings (NULL terminated).
         * Example: `{"3", "1", NULL}`
     * Can be NULL if not specified.
* `skk_henkan_key`
     * Pass the string (headword) before conversion.
     * Only used in `skk-gengo-to-ad` function.
     * Normally it can be NULL.

### RETURN VALUE

* 0: Success.
* MLISJ_ERROR_EVALUATION: Error interpreting S-expression.  When a string like "(C)" is passed, etc.
* MLISJ_ERROR_RUNTIME: Execution error.  When the allocated memory resources are used up, etc.

### CAUTION

* Character code is UTF-8.
* Numerical conversions such as "#0" are assumed to have been expanded by the caller.

## FEATURES

### Function

* \-
* car
* concat
* current-time-string
* make-string
* pwd
* string-to-char
* symbol-value
* substring
* string-to-number
* window-width
* skk-version
* skk-times
* skk-gadget-units-conversion
* skk-ad-to-gengo
* skk-gengo-to-ad
* skk-default-current-date
* skk-current-date
* skk-relative-date

### variable

* fill-column
* comment-start
* user-full-name
* user-mail-address
* skk-num-list

## NOTE

Below are the specifications and implementation notes.

### Overall policy

* Only functions, special forms, and features used in the SKK dictionaries are implemented.
* Specialized in evaluating one expression and returning a string.
* Create and clear the environment each time.  No GC.
    * Don't waste time initializing
* Preferred implementation simplicity over performance.
* Datum are immutable at the Lisp level.
* Returns immediately if there is an execution error (e.g. when a string like “(lol)” is passed).
* I don't want to differ too much from external libraries other than libc.

### Example of unimplemtented Emacs Lisp specifications

* Vector
* Variable assignment in Lisp
* Function definition in Lisp
* Definition of t

### Current implementation limitations

* Lists are represented as cdr-coded arrays.  Associative lists using dotted pair expressions and set-cdr are not possible.
* “\0” cannot be included in strings or symbols (because C string functions are used).
* Unimplemented features of reader
     * Integer literals only support decimal numbers.  Leading '+' and trailing '.' are also not supported.
     * Escapes in symbols.
     * Escape character literals.
     * Multibyte character literal.
     * Hexadecimal notation and unicode code notation in strings.
     * Escape multibyte characters in strings.
* Name resolution from string to symbol and from symbol to function is performed using linear search.
     * Since the number is small, it should not be a problem.
* Lisp_Object structures (like tagged pointers) are roughly the size of two pointers, which is wasteful.
     * It should be possible to make it smaller just by changing the definition of src/lispobject.h.
* Only integer strings are assumed to enter skk_num_list (same as when skk-num-convert-float is nil in DDSKK).
* Regarding the `skk-default-current-date` function, only the arguments used in SKK-JISYO.lisp are supported.

### Specifications different from Emacs Lisp

* () and nil are different.
* `concat` function with one argument returns the argument string as is.
     * Because it is often used as a means of string escape in SKK dictionaries,
