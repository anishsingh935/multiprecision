 1. Obtain a copy of [vsyasm](https://yasm.tortall.net/Download.html).
 2. Copy the file `yasm` to `C:\Program Files\yasm\`.
 3. Set a user variable `YASMPATH=<path to YASM assembler>` (e.g. `C:\Program Files\yasm\`).
 4. Ensure that path you have selected matches that in `example\mpfr_vc_and_mpir_vc\mpir_vc\yasm\vsyasm.props`.
 5. Supply a custom build rule to Visual Studio 2019:
    1. Identify a directory such as:
       `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\BuildCustomizations`
    2. Here, custom build rules reside in files with extensions ".props" and ".targets"
    3. Copy `vsyasm.props`, `vsyasm.targets` and `vsyasm.xml` to this directory, so that VS can find the custom rules for yasm.
 5. (Make sure that you have set `BOOST_ROOT` to the root directory of the Boost version you want to run).
 6. Open `example\mpfr_vc_and_mpir_vc\test_with_boost\test_with_boost.sln` and run.


TBD: Steps 1-3 use an old vsyasm from 2014. TBD: Investigate yasm_2018
