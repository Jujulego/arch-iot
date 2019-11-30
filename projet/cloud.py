#!/usr/bin/env python3
import asyncio
import aiohttp
import logging
import serial_asyncio

from typing import Callable, Optional

# Constants
SERIAL_DEVICE = "/dev/ttyUSB0"
BAUDRATE = 115200

BASE_URL = "http://things.ubidots.com"
TOKEN = "BBFF-a2Ur7AGWJy97Wrxc9g5QlTs7eVzITD"  # Put your TOKEN here
DEVICE_LABEL = "ThermoCarrote"  # Put your device label here


# Classes
class SerialProtocol(asyncio.Protocol):
    def __init__(self, recv_cb: Callable[[str], None]):
        # Attributes
        self.recv_cb = recv_cb
        self.transport = None  # type: Optional[serial_asyncio.SerialTransport]
        self.logger = logging.getLogger("serial")

    # Methods
    def connection_made(self, transport: serial_asyncio.SerialTransport):
        self.logger.info(f'Connected to port {SERIAL_DEVICE}')
        self.transport = transport

    def data_received(self, data: bytes):
        data = data.decode('ascii')

        self.logger.debug(f'Received data: {data[:-1]}')
        self.recv_cb(data)

    def send_data(self, data: str):
        if not data.endswith('\n'):
            data += '\n'

        self.logger.debug(f'Send data: {data[:-1]}')
        self.transport.write(data.encode('ascii'))

    def connection_lost(self, exc: Optional[Exception]):
        print("port closed")
        self.transport.loop.stop()

    def pause_writing(self) -> None:
        print("pause writing", self.transport.get_write_buffer_size())

    def resume_writing(self) -> None:
        print("resume writing", self.transport.get_write_buffer_size())


def on_recv(data: str):
    print(data.strip())


def send(protocol: SerialProtocol):
    protocol.send_data("Test !")


def protocol_factory():
    return SerialProtocol(on_recv)


async def main(loop: asyncio.AbstractEventLoop):
    _, protocol = await serial_asyncio.create_serial_connection(loop, protocol_factory, SERIAL_DEVICE, baudrate=BAUDRATE)

    loop.call_later(1, lambda: send(protocol))


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    loop = asyncio.get_event_loop()

    loop.run_until_complete(main(loop))
    loop.run_forever()


# import time
# import requests
# import math
# import random
# import json
#
#
#
#
# def send_data(payload):
#     # Creates the headers for the HTTP requests
#     url =
#     url = "{}/api/v1.6/devices/{}/".format(url, DEVICE_LABEL)
#     headers = {"X-Auth-Token": TOKEN, "Content-Type": "application/json"}
#
#     # Makes the HTTP requests
#     status = 400
#     attempts = 0
#     while status >= 400 and attempts <= 5:
#         req = requests.get(url=url, headers=headers, data=json.dumps(payload))
#         status = req.status_code
#         attempts += 1
#         time.sleep(1)
#
#     # Processes results
#     if status >= 400:
#         print("[ERROR] Could not send data after 5 attempts, please check \
#             your token credentials and internet connection")
#         return False
#
#     #print("[INFO] request made properly, your device is updated")
#     return True
#
#
# def main():
#     payload = build_payload(
#         VARIABLE_LABEL_1, VARIABLE_LABEL_2, VARIABLE_LABEL_3)
#
#     print("[INFO] Attemping to send data")
#     post_request(payload)
#     print("[INFO] finished")
#
#
# if __name__ == '__main__':
#     while (True):
#         data = input()
#
#         if data.startswith('data:'):
#             _, name, value = data.split(':')
#             value = float(value)
#
#             print(f'send : {name} = {value}')
#             send_data({ name: value })
#
#         else:
#             print(data)
