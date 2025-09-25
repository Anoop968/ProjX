import serial
import time
import random

# === Serial Configuration ===
# Update COM port for Windows (e.g., "COM3") or Linux/Mac (e.g., "/dev/ttyUSB0")
SERIAL_PORT = "COM5"
BAUD_RATE = 9600

# Open serial connection
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
except Exception as e:
    print("Error opening serial port:", e)
    exit()

def generate_water_data(condition="random"):
    """
    Simulate sensor readings based on condition
    Conditions:
      - good      : Safe drinking water
      - warning   : Slightly unsafe
      - dangerous : Highly contaminated
      - random    : Any random state
    """
    if condition == "good":
        pH = round(random.uniform(6.8, 7.5), 2)
        tds = round(random.uniform(80, 200), 1)
        turbidity = round(random.uniform(0.5, 3.0), 2)
        endotoxin = round(random.uniform(0, 10), 2)
        ecoli = 0  # no contamination
    elif condition == "warning":
        pH = round(random.uniform(6.2, 8.8), 2)
        tds = round(random.uniform(250, 500), 1)
        turbidity = round(random.uniform(3.5, 7.0), 2)
        endotoxin = round(random.uniform(10, 25), 2)
        ecoli = random.choice([0, 1])  # maybe contamination
    elif condition == "dangerous":
        pH = round(random.uniform(5.0, 9.5), 2)
        tds = round(random.uniform(500, 1000), 1)
        turbidity = round(random.uniform(7.0, 20.0), 2)
        endotoxin = round(random.uniform(25, 50), 2)
        ecoli = 1  # contamination present
    else:  # completely random
        pH = round(random.uniform(5.0, 9.5), 2)
        tds = round(random.uniform(50, 1000), 1)
        turbidity = round(random.uniform(0.5, 20.0), 2)
        endotoxin = round(random.uniform(0, 50), 2)
        ecoli = random.choice([0, 1])

    return pH, tds, turbidity, endotoxin, ecoli

try:
    while True:
        # Pick a random condition to simulate
        condition = random.choice(["good", "warning", "dangerous"])
        pH, tds, turbidity, endotoxin, ecoli = generate_water_data(condition)

        # Create data string to send to ESP32
        data_string = f"{pH},{tds},{turbidity},{endotoxin},{ecoli}\n"
        ser.write(data_string.encode('utf-8'))

        print(f"Sent -> Condition: {condition.upper()} | Data: {data_string.strip()}")

        # Delay before next reading
        time.sleep(2)

except KeyboardInterrupt:
    print("\nSimulation stopped by user.")
    ser.close()
