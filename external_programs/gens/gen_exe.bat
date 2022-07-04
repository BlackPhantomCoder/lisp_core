%MAIN_PYTHON% -m PyInstaller --onefile fsm_gen.py
rmdir /q /s build
del fsm_gen.spec