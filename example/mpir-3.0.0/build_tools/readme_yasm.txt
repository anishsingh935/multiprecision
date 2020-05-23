1) As administrator, copy the folder yasm to "C\Program Files\yasm"
2a) Supply custom build rule to VS2019.
2b) Identify a directory such as:
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\BuildCustomizations"
2c) Here there should be custom build rules in files having extensions ".props" and ".targets"
2d) Copy "vsyasm.props" and "vsyasm.targets" to this directory so VS can find the custom rules for yasm.
3) Edit "vsyasm.props" and ensure that the path to vsyasm from step 1) agrees with the entries in "vsyasm.props".

4) TBD: Steps 1-3 use an old vsyasm from 2014. TBD: Investigate yasm_2018


--------------------------
Sample of "vsyasm.props"
--------------------------

<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup
    Condition="'$(YASMBeforeTargets)' == '' and '$(YASMAfterTargets)' == '' and '$(ConfigurationType)' != 'Makefile'">
    <YASMBeforeTargets>Midl</YASMBeforeTargets>
    <YASMAfterTargets>CustomBuild</YASMAfterTargets>
  </PropertyGroup>
  <PropertyGroup>
    <YASMDependsOn
      Condition="'$(ConfigurationType)' != 'Makefile'">_SelectedFiles;$(YASMDependsOn)</YASMDependsOn>
  </PropertyGroup>
  <PropertyGroup>
    <YasmPath Condition= "'$(YASMPATH)' == ''">C:\Program Files\yasm\</YasmPath>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <YASM>
      <Debug>False</Debug>
      <ObjectFile>$(IntDir)</ObjectFile>
      <PreProc>0</PreProc>
      <Parser>0</Parser>
      <CommandLineTemplate>"$(YasmPath)vsyasm.exe" -Xvc -f x64 [AllOptions] [AdditionalOptions] [Inputs]</CommandLineTemplate>
      <Outputs>%(ObjectFile)</Outputs>
      <ExecutionDescription>Assembling %(Filename)%(Extension)</ExecutionDescription>
      <ShowOnlyRuleProperties>false</ShowOnlyRuleProperties>
    </YASM>
  </ItemDefinitionGroup>
</Project>
