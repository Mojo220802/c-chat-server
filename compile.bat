@echo off

gcc server_MC-pt.c -o server_MC-pt.exe -lws2_32
gcc zwischenServer.c -o zwischenServer.exe -lws2_32
gcc listen.c -o listen.exe -lws2_32
gcc tell.c -o tell.exe -lws2_32