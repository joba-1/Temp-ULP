/*
JoBa1 ESP32 ULP temperature measurement example

ULP program that writes temperature values in a ring buffer.
If the buffer is full, the main cores are started to process the data.
*/

#include "soc/soc_ulp.h"      // for READ_RTC_FIELD()
#include "soc/rtc_cntl_reg.h" // for RTC_CNTL_LOW_POWER_ST_REG
#include "soc/sens_reg.h"     // for TSENS SENS_SAR_MEAS_WAIT2_REG
#include "sdkconfig.h"        // for CONFIG_ULP_BUFFER

// Declare data visible to main cores
.global entry                 // ulp start address used by main core program

.global offset                // tell main cores our idea of buffer address
.global head                  // write position of ringbuffer
.global tail                  // read position of ring buffer
.global buffer                // start address of ring buffer
.global buffer_end            // adress behind buffer


.bss

data:                         // data frame pointer

offset: .long 0               // address of buffer for main core
head:   .long 0
tail:   .long 0
buffer: .long 0               // circular buffer tail-head-gap
        .skip (CONFIG_ULP_BUFFER-4) // buffer items
buffer_end:


// Offsets to variables relative to data (which I try to keep in r2 for ld/st)
.set    o_offset,     (offset-data)
.set    o_head,       (head-data)
.set    o_tail,       (tail-data)


.text

entry:
        move  r2, data        // constant data frame pointer
        ld    r0, r2, o_head
        jumpr init_done, 1, ge // head > 0: already initialized

// Ringbuffer is not initialized by previous runs. Do it now.
        move  r0, buffer
        st    r0, r2, o_head  // head = buffer
        st    r0, r2, o_tail  // tail = buffer == head -> empty
        st    r0, r2, o_offset // offset = buffer (for main cores)
init_done:

// Enable TSENS
        WRITE_RTC_REG(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR_S, 2, SENS_FORCE_XPD_SAR_PU)

// Do the measurement
        tsens r0, 1000        // measure for 1000 cycles

// Put r0 byte with the temperature to the ringbuffer
put:    ld    r1, r2, o_head  // r1 points to buffer at head position
        st    r0, r1, 0       // store word in buffer at head position
        add   r1, r1, 1       // head++
        move  r0, buffer_end
        sub   r0, r0, r1      // 0 means head beyond buffer
        jumpr check_tail, 1, ge
        move  r1, buffer      // set head at start of buffer
check_tail:
        ld    r0, r2, o_tail
        sub   r0, r0, r1
        jump  overflow, eq    // need to wake up main cores
have_space:
        st    r1, r2, o_head  // new head saved
        halt
overflow:
        READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP)
        and r0, r0, 1
        jump overflow, eq     // Retry until the bit is set
        wake                  // Trigger wake up
check_again:
        wait  10              // wait some ticks, so main core can update tail
        ld    r0, r2, o_tail
        sub   r0, r0, r1
        jump  check_again, eq // still need to wait for main cores
        jump  have_space
