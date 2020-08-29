1) As administrator, copy the folder yasm to "C\Program Files\yasm"
2) Chekck/Edit "vsyasm.props" and ensure that the path you have selected for vsyasm in step 1) agrees with the entries in "vsyasm.props".
3a) Supply custom build rule to VS2019.
3b) Identify a directory such as:
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\BuildCustomizations"
3c) Here there should be custom build rules in files having extensions ".props" and ".targets"
3d) Copy "vsyasm.props", "vsyasm.targets" and "vsyasm.xml" to this directory so VS can find the custom rules for yasm.

4) TBD: Steps 1-3 use an old vsyasm from 2014. TBD: Investigate yasm_2018
