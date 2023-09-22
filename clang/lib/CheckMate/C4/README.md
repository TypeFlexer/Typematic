# C4

C4 takes a [Checked C](https://github.com/microsoft/checkedc) program as input, and converts it to vanilla C. For example, here is some Checked C:
```
#include <stdio.h>
#include <stdchecked.h>

int main(int argc, char** argv : itype(_Array_ptr<_Nt_array_ptr<char>>) count(argc)) {
  checked{
    int arr checked[5]  = { 0, 1, 2, 3, 4 };
    tptr<tptr<int>> q;
  }

  int arr checked[5]  = { 0, 1, 2, 3, 4 };
  tptr<tptr<int>> q;
  tarray_ptr <int > current : bounds (start , end ) = start;
  dynamic_check(1 == 1);
  return 0;
}
```
and here is the result of running C4 on it:
```
#include <stdio.h>


int main(int argc, char** argv ) {
   {
    int arr [5]  = { 0, 1, 2, 3, 4 };
    int * * q;
  }

  int arr [5]  = { 0, 1, 2, 3, 4 };
  int * * q;
  int * current  = start;
  ;
  return 0;
}
```

C4 does not attempt to parse the full Checked C grammar. It parses just enough of the grammar to identify contextual uses of Checked C idioms, and converts them to their C equivalents. It is built using [ocamllex](https://ocaml.org/manual/lexyacc.html) and [menhir](http://gallium.inria.fr/~fpottier/menhir/).

# Setup

Set up opam (see `opam init --help`), then run `opam install --deps-only .` to install the OCaml dependencies. Make sure that regular (non Checked C) `clang` is on your `$PATH`.

# Running, Examples

The following runs the above example:
```
dune exec -- src/main.exe -f samples/hello-world.c
```
Some small tests may be found in the `tests` directory

# Limitations

- See the [issues](https://github.com/correctcomputation/C4/issues) for tabulated problems
- The `count`, `bounds`, etc. keywords can oftentimes be used as variables but still might break in some situations
- The `stdchecked.h` Checked C header file contains shorthands, e.g., `tptr<int>` instead of `_TPtr<int>`. However, these versions clash in programs that use (say) `tptr` as a variable name. C4 currently just assumes that `tptr` is the same as `_TPtr`, and will therefore clash in this way if `tptr` is used as a variable.
- C4 assumes that `itype` precedes any bounds, following a colon, when in fact both orders are acceptable
