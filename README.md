# Measure ESP32 internal temperature with the ULP

ULP wakes up in intervals to measure the temperature with internal sensor while main cores are in deep sleep.
If the data buffer is full, the ULP wakes up the main cores to process the data.
Main cores will connect to wifi, post data to a webserver and go to sleep again.
WLAN, Webserver, interval and buffer size can be configured in sdkconfig (make menuconfig).
Also, dont forget to set CONFIG_ULP_COPROC_RESERVE_MEM for the ULP program and the buffer.

## Build - Flash - Monitor
* Prepare shell to use ESP-IDF (see my [Blink-ULP repo](https://github.com/joba-1/Blink-ULP/blob/master/README.md) for details)
```
. ~/esp32/env.sh
```

* Fetch and build this example, flash it and open serial console (adapt ESPPORT and ESPBAUD to your setup. Can be configured in sdkconfig file)
```
REPO="Temp-ULP"
BASE="/tmp/$REPO-$$"
mkdir -p "$BASE" && \
cd "$BASE" && \
git clone "https://github.com/joba-1/$REPO.git" && \
cd "$REPO" && \
make menuconfig && \
make -j8 flash monitor ESPPORT=/dev/ttyUSB0 ESPBAUD=115200

```

Exit the monitor with [Ctrl]-] (which is [Strg]-[Alt-Gr]-] on german keyboard)

## Example data

Testing a MH-ET live mini kit board with a wemos 18650 (3000mAh) adapter.
The listing contains the wake up interval sending buffered measurements, 
the total runtime and the average current draw.

Interval | Working [h] | Draw [mA]
:---:|:---:|:---:
5s   |   8 | 375
5min |  42 |  70
5h   |  50 |  60

The actual current draw was a bit lower since the battery capacity was not the advertised 3000mAh for sure. 
But anyways, this board and battery adapter draw a lot of current while the ESP32 is in deepsleep drawing <10µA.
So better search for another ESP32 setup for your low power applications (and tell me about it, please). 

JoBa1
