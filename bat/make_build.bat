@echo off
for %%i in (%~1) do (
    if not exist %%i mkdir %%i
)
if not exist %2 mkdir %2