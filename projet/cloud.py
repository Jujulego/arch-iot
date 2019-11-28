#!/usr/bin/env python3
import time
import requests
import math
import random
import json

TOKEN = "BBFF-a2Ur7AGWJy97Wrxc9g5QlTs7eVzITD"  # Put your TOKEN here
DEVICE_LABEL = "ThermoCarrote"  # Put your device label here

def send_data(payload):
    # Creates the headers for the HTTP requests
    url = "http://things.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, DEVICE_LABEL)
    headers = {"X-Auth-Token": TOKEN, "Content-Type": "application/json"}

    # Makes the HTTP requests
    status = 400
    attempts = 0
    while status >= 400 and attempts <= 5:
        req = requests.post(url=url, headers=headers, data=json.dumps(payload))
        status = req.status_code
        attempts += 1
        time.sleep(1)

    # Processes results
    if status >= 400:
        print("[ERROR] Could not send data after 5 attempts, please check \
            your token credentials and internet connection")
        return False

    #print("[INFO] request made properly, your device is updated")
    return True


def main():
    payload = build_payload(
        VARIABLE_LABEL_1, VARIABLE_LABEL_2, VARIABLE_LABEL_3)

    print("[INFO] Attemping to send data")
    post_request(payload)
    print("[INFO] finished")


if __name__ == '__main__':
    while (True):
        data = input()

        if data.startswith('data:'):
            _, name, value = data.split(':')
            value = float(value)

            print(f'send : {name} = {value}')
            send_data({ name: value })

        else:
            print(data)
