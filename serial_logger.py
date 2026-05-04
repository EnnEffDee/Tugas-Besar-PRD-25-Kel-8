import serial
import csv
import json
from datetime import datetime

# ser = serial.Serial('COM3', 9600, timeout=1)          # - Windows
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)    # - Linux
# ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)  # - Mac

csv_file = "log.csv"
json_file = "users.json"

def load_users():
    with open(json_file, "r") as f:
        return json.load(f)

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode().strip()                      # Baca masukan dari serial
        if line.startswith("PIN"):
            try:
                itype, device, pin = line.split(",")

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
                        status
                    ])

                ser.write((status + "\n").encode())
            except Exception as e:
                print("ERROR:", e)
