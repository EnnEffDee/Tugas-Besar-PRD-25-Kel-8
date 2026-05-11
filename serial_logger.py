import time
import serial
import csv
import json
from datetime import datetime

PORT = "/dev/ttyUSB0"
BAUD = 9600

csv_file = "log.csv"
json_file = "users.json"

def load_users():
    try:
        with open(json_file, "r") as f:
            return json.load(f)
    except:
        return {}

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)  # wait for Arduino reset
    ser.reset_input_buffer()
    print("Connected")
except Exception as e:
    print("Cannot open serial:", e)
    exit()

while True:
    try:
        if ser.in_waiting > 0:
            line = ser.readline().decode(errors='ignore').strip()

            if not line:
                continue

            print("RX:", line)
            if line.startswith("PIN"):
                itype, device, pin, mode = line.split(",")

                users = load_users()

                valid = False

                if pin in users:                                    # PIN ada di JSON?
                    if isinstance(users[pin], dict):
                        valid = users[pin].get("enabled", False)
                    else:
                        valid = users[pin]

                if (valid):
                    status = "VALID"
                else:
                    status = "INVALID"

                with open(csv_file, 'a', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow([
                        datetime.now().isoformat(),
                        device,
                        pin,
                        status,
                        mode
                    ])
                ser.write((status + "\n").encode())
                print(f"{pin} -> {status}")
    except serial.SerialException as e:
        print("Serial error:", e)
        time.sleep(1)
    except Exception as e:
        print("ERROR:", e)
