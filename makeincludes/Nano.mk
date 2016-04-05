
MCU = atmega328
F_CPU =  16000000

# Nanos can only be programmed on this baud rate for some reasnons
AVRDUDE_FLAGS += -b57600