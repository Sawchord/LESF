Welcome to LESF (Lightweight embedded system framework).
Given its early development the name "Lame embedded system framework" might fit better.

Dependencies:
- Building: 
    - AVR-Toolchain (avr-gcc, avr-nm, avr-ld, avr-objdump)

- Debug: 
    - Simulavr
    - avr-gdb 
    - ddd as GUI


Todolist:
- hal_delay() in powersave mode
- vmmu???
- Fix the Makefile
    - Rebuild libs when make program and they have been changed
    - get make debug right
    - test on Arduino Nano
    
