# 0 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
# 84 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
        .syntax unified
# 108 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
        .global reset_handler
        .global Reset_Handler
        .equ reset_handler, Reset_Handler
        .section .init.Reset_Handler, "ax"
        .balign 2
        .thumb_func
Reset_Handler:




        bl SystemInit
# 166 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
        movw R0, 0xED88
        movt R0, 0xE000
        ldr R1, [R0]
        orrs R1, R1, #(0xf << 20)
        str R1, [R0]




        bl _start





        .weak SystemInit





        .section .init_array, "aw"
        .balign 4
        .word SystemCoreClockUpdate
# 212 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
        .weak HardFault_Handler
        .section .init.HardFault_Handler, "ax"
        .balign 2
        .thumb_func
HardFault_Handler:



        ldr R1, =0xE000ED2C
        ldr R2, [R1]
        cmp R2, #0

.LHardFault_Handler_hfLoop:
        bmi .LHardFault_Handler_hfLoop
# 240 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
        tst LR, #4
        ite eq
        mrseq R0, MSP
        mrsne R0, PSP
# 254 "/Users/kavidey/github/NeoObscura/software/mcu_usb/STM32L4xx/Source/STM32L4xx_Startup.s"
        orr R2, R2, #0x80000000
        str R2, [R1]




        ldr R1, [R0, #24]
        adds R1, #2
        str R1, [R0, #24]

        bx LR
