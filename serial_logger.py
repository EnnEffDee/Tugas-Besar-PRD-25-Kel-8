import time
import serial
import csv
import json
from datetime import datetime

PORT = "COM3"
BAUD = 9600

csv_file = "log.csv"
json_file = "users.json"

def load_users():
    try:
        with open(json_file, "r") as f:
            return json.load(f)
    except:
        return {}

def last_valid_mode(pin):
    """Return mode of the last VALID log entry for this pin, or None."""
    try:
        with open(csv_file, "r") as f:
            reader = csv.DictReader(f)
            last = None
            for row in reader:
                if row["pin"] == pin and row["status"] == "VALID":
                    last = row["mode"]
            return last
    except:
        return None

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

                alasan = "-"
                valid = False

                if pin not in users:
                    alasan = "PIN_TIDAK_ADA"
                else:
                    user_data = users[pin]
                    enabled = user_data.get("enabled", False) if isinstance(user_data, dict) else user_data
                    if not enabled:
                        alasan = "USER_DISABLED"
                    elif mode == "MASUK" and last_valid_mode(pin) == "MASUK":
                        alasan = "MASUK_GANDA"
                    else:
                        valid = True

                status = "VALID" if valid else "INVALID"

                with open(csv_file, 'a', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow([
                        datetime.now().isoformat(),
                        device,
                        pin,
                        status,
                        mode,
                        alasan
                    ])
                ser.write((status + "\n").encode())
                print(f"{pin} -> {status}")
    except serial.SerialException as e:
        print("Serial error:", e)
        time.sleep(1)
    except Exception as e:
        print("ERROR:", e)
