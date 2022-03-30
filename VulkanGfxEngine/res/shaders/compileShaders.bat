@echo off
for %%f in (*.vert) do  (
   %clibs%\vulkanSDK\Bin\glslc.exe %%~nf.vert -o %%~nf.spv
)
for %%f in (*.frag) do  (
   %clibs%\vulkanSDK\Bin\glslc.exe %%~nf.frag -o %%~nf.spv
)   

