from flask import Flask, jsonify, send_from_directory
import csv
import json
import os

app = Flask(__name__)
BASE = os.path.dirname(os.path.abspath(__file__))

@app.route('/')
def home():
    return send_from_directory(BASE, 'home.html')

@app.route('/users')
def users():
    return send_from_directory(BASE, 'users.html')

@app.route('/api/logs')
def api_logs():
    logs = []
    try:
        with open(os.path.join(BASE, 'log.csv'), 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                logs.append(dict(row))
    except Exception:
        pass
    return jsonify(logs)

@app.route('/api/users')
def api_users():
    try:
        with open(os.path.join(BASE, 'users.json'), 'r') as f:
            return jsonify(json.load(f))
    except Exception:
        return jsonify({})

if __name__ == '__main__':
    app.run(debug=True, port=5000)
