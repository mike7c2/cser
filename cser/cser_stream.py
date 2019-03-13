import select
import socket
import logging
import struct
import threading
import traceback
import sys

logger = logging.getLogger(__name__)

# Header value for packets
PACKET_HEADER_VALUE = 0xAA


class CSERStream(object):
    """Base class for serial interfaces

    Handles the packet layer of cser
    once started a thread runs receiving packets from
    the serial interface, these will be passed on to the
    callback function.

    """

    def __init__(self, stream, callback):
        self._stream = stream
        self._running = False
        self._packet_callback = callback
        self._listen_thread = None

    def start(self, blocking=False):
        self._running = True
        self._listen_thread = threading.Thread(target=self._packet_reader_thread)
        if blocking:
            self._listen_thread.run()
        else:
            self._listen_thread.start()

    def stop(self):
        self._running = False
        logger.info("Closing stream")
        self._stream.close()
        logger.info("Stream closed")

    def send_packet(self, cmd):
        logger.debug("cmd is " + ":".join("{:02x}".format(ord(c)) for c in cmd))
        packet = self._packetise(cmd)
        logger.debug("pkt is " + ":".join("{:02x}".format(ord(c)) for c in packet))
        self._stream.write(packet)

    def _packetise(self, cmd):
        msg = struct.pack('B', PACKET_HEADER_VALUE)
        msg += struct.pack('B', len(cmd))
        msg += cmd
        chk = 0
        for b in msg:
            chk += ord(b)
        msg += struct.pack('B', chk % 256)

        return msg

    def _packet_reader_thread(self):
        while self._running:
            try:
                packet = self._read_packet()
                if packet and self._packet_callback:
                    self._packet_callback(packet)

            except Exception as ex:
                template = "An exception of type {0} occured. Arguments:\n{1!r}"
                message = template.format(type(ex).__name__, ex.args)
                logger.error("Failed reading packet: " + message)
                traceback.print_exc(file=sys.stdout)
                self.stop()

    def _read_packet(self):
        chk = 0

        logger.debug("Waiting for header")

        # Wait for header
        hd = self._stream.read(1)
        while hd != "\xAA":
            if len(hd) == 0:
                logger.warning("Socket closed")
                self._running = False
                return None
            logger.debug("Discarded : " + str(hex(ord(hd))))
            if not self._running:
                return None
            hd = self._stream.read(1)

        # Get length of packet
        length = int(ord(self._stream.read(1)))
        logger.debug("Reading packet of length : " + str(length))
        chk += length

        # Read payload and checksum
        data = self._stream.read(length + 1)

        # Calculate checksum
        for i in range(length):
            chk += int(ord(data[i]))

        logger.debug("pkt is " + ":".join("{:02x}".format(ord(c)) for c in data))

        # Check checksum is correct
        if (chk % 256) != int(ord(data[length])):
            # Checksum incorrect
            logger.warning("Bad checksum, got " + str(chk % 256) + " wanted " + str(int(ord(data[length]))))
            logger.warning("pkt is " + ":".join("{:02x}".format(ord(c)) for c in data))
            return None

        logger.debug("received pkt is " + ":".join("{:02x}".format(ord(c)) for c in data))

        return data[0:-1]


class FileStream(object):
    def __init__(self, fname):
        self.file = open(fname)

    def close(self):
        return self.file.close()

    def read(self, length):
        return self.file.read(length)

    def write(self, data):
        return self.file.write(data)


class SerialStream(object):

    def __init__(self, port):
        import serial
        self.port = serial.Serial(port, timeout=0.01)
        self.port.write_timeout = 0.01
        self.running = True

    def close(self):
        self.running = False
        return self.port.close()

    def read(self, length):
        rd_len = 0
        data = ""

        while self.running and rd_len < length:
            dat = self.port.read(length - rd_len)
            rd_len += len(dat)
            data += dat

        return data

    def write(self, data):
        return self.port.write(data)


class SocketStream(object):

    def __init__(self, ip, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((ip, port))
        self.socket.setblocking(0)

        self.running = True

    def close(self):
        self.running = False
        self.socket.close()

    def read(self, length):
        rd_len = 0
        data = ""
        while rd_len < length and self.running:
            ready = select.select([self.socket], [], [], 0.1)
            if ready[0]:
                rd_dat = self.socket.recv(length - rd_len)
                rd_len += len(rd_dat)
                data += rd_dat

        return data

    def write(self, data):
        return self.socket.send(data)
