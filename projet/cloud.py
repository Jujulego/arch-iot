import asyncio
import aiohttp
import logging
import pyee
import serial_asyncio

from typing import Dict, Optional, Union

# Constants
SERIAL_DEVICE = "/dev/ttyUSB0"
BAUDRATE = 115200

TOKEN = "BBFF-a2Ur7AGWJy97Wrxc9g5QlTs7eVzITD"  # Put your TOKEN here
DEVICE_LABEL = "ThermoCarrote"  # Put your device label here


# Classes
class SerialProtocol(asyncio.Protocol, pyee.AsyncIOEventEmitter):
    """
    class SerialProtocol:
    Manages communication with the device

    Events :
    - recv (data: str)
    """

    def __init__(self, *, loop: Optional[asyncio.AbstractEventLoop] = None):
        if loop is None:
            loop = asyncio.get_event_loop()

        pyee.AsyncIOEventEmitter.__init__(self, loop=loop)

        # Attributes
        self.transport = None  # type: Optional[serial_asyncio.SerialTransport]
        self._logger = logging.getLogger("serial")

    # Methods
    def connection_made(self, transport: serial_asyncio.SerialTransport):
        self._logger.info(f'Connected to port {SERIAL_DEVICE}')
        self.transport = transport

    def data_received(self, data: bytes):
        data = data.decode('ascii')

        self._logger.debug(f'Received data: {data[:-1]}')
        self.emit('recv', data)

    def send_data(self, data: str):
        if not data.endswith('\n'):
            data += '\n'

        self._logger.debug(f'Send data: {data[:-1]}')
        self.transport.write(data.encode('ascii'))

    def connection_lost(self, exc: Optional[Exception]):
        print("port closed")
        self.transport.loop.stop()

    def pause_writing(self) -> None:
        print("pause writing", self.transport.get_write_buffer_size())

    def resume_writing(self) -> None:
        print("resume writing", self.transport.get_write_buffer_size())


class CloudDevice(pyee.AsyncIOEventEmitter):
    BASE_URL = "http://things.ubidots.com/api/v1.6"

    def __init__(self, name: str, token: str, *, loop: Optional[asyncio.AbstractEventLoop] = None):
        if loop is None:
            loop = asyncio.get_event_loop()

        super().__init__(loop=loop)

        # Attributes
        self.name = name
        self.token = token

        # - internals
        self._session = aiohttp.ClientSession()
        self._logger = logging.getLogger("cloud")

    # Methods
    async def post_values(self, values: Dict[str, Union[str, int, float]]):
        url = f'{self.BASE_URL}/devices/{self.name}/'
        headers = {
            'X-Auth-Token': self.token,
            'Content-Type': 'application/json'
        }

        self._logger.debug(f'Send values: {values}')
        await self._session.post(url, headers=headers, json=values)

    async def get_value(self, name: str) -> Union[str, int, float]:
        url = f'{self.BASE_URL}/devices/{self.name}/{name}/lv/'
        headers = {
            'X-Auth-Token': self.token,
            'Content-Type': 'application/json'
        }

        self._logger.debug(f'Get value of {name}')
        await with self._session.get(url, headers=headers) as rep:
            return await rep.json()

    async def watch(self, name: str):
        value = await self.get_value(name)
        self.emit(name, value)

        while True:
            await asyncio.sleep(5, loop=self._loop)
            nv = await self.get_value(name)

            if nv != value:
                value = value
                self.emit(name, value)
                self._logger.info(f'Value of {name} changed to: {value}')


async def setup(loop: asyncio.AbstractEventLoop):
    # Connect to device
    _, protocol = await serial_asyncio.create_serial_connection(
        loop,
        lambda : SerialProtocol(loop=loop),
        SERIAL_DEVICE,
        baudrate=BAUDRATE
    )

    # Connect to cloud
    device = CloudDevice(DEVICE_LABEL, TOKEN, loop=loop)
    await device.post_values({ "hot": 30 })

    loop.create_task(device.watch("hot"))

    # Callbacks
    @protocol.on("recv")
    def recv(data: str):
        if data.startswith('data:'):
            _, name, value = data.split(':')
            value = float(value)

            print(f'send : {name} = {value}')
            device.post_values({ name: value })

        else:
            print(data)

    @device.on("hot")
    def hot(value: int):
        print(f'change hot: {value}')
        protocol.send_data(f'hot {value}')


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    loop = asyncio.get_event_loop()

    loop.run_until_complete(setup(loop))
    loop.run_forever()
