@echo off
mode con cols=30 lines=30
title automated controller 4 MOOC
start /min msedge.exe localhost/SwitchCamera
choice /t 1 /d y >nul
taskkill /IM msedge.exe