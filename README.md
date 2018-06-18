# regency

A CLI tool for generating strings from regex-like patterns, written in C.

Note that regency uses `rand()` to generate its random values, meaning it is not cryptographically sound. Use for password generation at your own risk.

## Usage

To use regency, `make` it and pass it your regex-ish pattern as an argument:

```
regency [pattern]
```

At least one option flag will be added, but none have been as of yet.

Regency current accepts the following patterns:

* `\d`: a random digit
* `\w`: a random word character (a-z, A-Z, 0-9, \_)
* `[a-z|]`: character ranges, produces a random character of the characters specified, based on ASCII value
* `\y`: a word from the default dictionary, or the user-provided dictionary if present
* All other characters are currently treated as literals

I'd like to include the following patterns and features:

* `-d dict.txt`: a list of whitespace-delimited words to be used by the `\y` pattern
* `{2}`, `{2,}`, `{2,5}`: quantifiers 

