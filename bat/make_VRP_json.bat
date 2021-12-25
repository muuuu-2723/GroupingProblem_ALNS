@echo off
pushd %~dp0\..\Data\VRP
g++ make_VRP_json.cpp -std=c++17 -o make_VRP_json.exe
for %%f in (*.txt) do (
    call make_VRP_json.exe %%f
)
popd