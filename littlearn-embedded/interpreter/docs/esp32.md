# Running the ./program_esp32.sh Script

* Should be run outside Docker on a Linux machine
* Before running the script, run:

```bash
sudo usermod -a -G dialout $USER
sudo chmod 666 /dev/ttyUSB0 # or /dev/ttyUSB1, etc.
```

You will need to restart your computer for the changes to take effect.