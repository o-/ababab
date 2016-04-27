# ababab

Randomized Search for Kolmogorov Complexity of a byte sequence.

## Usage

    # build
    make

    # search for a program to produce the sequence from 1 to 5
    ./comp s "1 2 3 4 5"

    # search for a program to produce "ababab"
    ./comp s "97 98 97 98 97 98"

    # execute a program to output 5 bytes
    ./comp r 5 "inc write jmp-2"

## Computational Model

The programs are generated for a simple stack based interpreter featuring a
finite length r/w output buffer.

## Limits

Programs which require stack discipline will almost never be discovered. Not
sure if due to bugs or inherent limitations.

## References

Randomized program synthesis for low-level representations due to
[stoke](http://stoke.stanford.edu/) project.
