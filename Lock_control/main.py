from gpiozero import OutputDevice
import time
import sys

# Set up output pin
OUTPUT_PIN = 26 # example pin number, change to your desired pin number
output_pin = OutputDevice(OUTPUT_PIN)

# Output high signal for 15 seconds
output_pin.on()
print("lock is unlocked")
time.sleep(3)
output_pin.off()
print("lock is locked")

# Exit program with status code 0
sys.exit(0)