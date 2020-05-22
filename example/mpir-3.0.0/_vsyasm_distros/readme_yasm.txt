1) As administrator, copy the folder yasm to "C\Program Files\yasm"
2) In the folder build.vc, identify the file "vsyasm.props"
3) Edit  and ensure that the path to vsyasm and the command line arguments agree.
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
