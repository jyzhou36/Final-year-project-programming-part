.origin 0
.entrypoint START

//Refer to this mapping in the file - prussdrvincludepruss_intc_mapping.h
#define PRU0_ARM_INTERRUPT      19
#define CONST_PRUCFG            C4


START:
         // Enable OCP master port
        LBCO      r0, CONST_PRUCFG, 4, 4
        CLR       r0, r0, 4         i
        SBCO      r0, CONST_PRUCFG, 4, 4
         // Reading the memory that was set by the C++ host  program into registers
        // r1 to read the PWM percent high
        MOV     r0, 0x00000000     //load the memory location
        LBBO    r1, r0, 0, 4       //load the percent value into r1
        // r2 to  load the time delay ---velocity
        MOV     r0, 0x00000004     //load the memory location
        LBBO    r2, r0, 0, 4       //load the step delay value into r2
        // r3  The PWM precent that the signal is low
        MOV     r3, 100            //load 100 into r3
        SUB     r3, r3, r1         //subtract r1 from 100

MAINLOOP:
        MOV     r4, r1             // start counter
        SET     r30.t5             // set the output P9_27 high
SIGNAL_HIGH:
        MOV     r0, r2             //move r2 to r0
DELAY_HIGH:
        SUB     r0, r0, 1          // reduce 1 each loop
        QBNE    DELAY_HIGH, r0, 0  // repeat until r0 is clear
        SUB     r4, r4, 1          // the signal was high for the counter
        QBNE    SIGNAL_HIGH, r4, 0 // repeat until counter is clear

        MOV     r4, r3             // number of steps low loaded
        CLR     r30.t5             // set the output P9_27 low
SIGNAL_LOW:
        MOV     r0, r2             // load r2 to r0
DELAY_LOW:
        SUB     r0, r0, 1          // reduce 1 each loop
        QBNE    DELAY_LOW, r0, 0   // repeat until r9 is clear
        SUB     r4, r4, 1          // the signal was low for the counter
        QBNE    SIGNAL_LOW, r4, 0  // repeat until counter is clear

        QBBS    END, r31.t3        // quit the program if button P9_28 ispressed
        QBA     MAINLOOP           // otherwise back to Mainloop and loop again
END:
        MOV       r31.b0, PRU0_ARM_INTERRUPT+16        //End PRU interrupt

    // Halt the processor
        HALT
