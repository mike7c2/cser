import cser_dbg_parser
import cser_stream
import argparse
import re
import sys
import logging
import signal

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class CserDebugTarget(object):

    def __init__(self, stream, messages=None, cser_mode=False):
        self.parser = cser_dbg_parser.CserDebugParser(messages)

        def packet_callback(pkt):

            if cser_mode:
                if ord(pkt[0]) == 10:
                    self.parser.receive_packet(pkt[1:])
            else:
                self.parser.receive_packet(pkt)

        self.stream = cser_stream.CSERStream(stream, packet_callback)

    def start(self):
        self.stream.start(blocking=True)

    def stop(self):
        self.stream.stop()


def get_messages_from_pragma(filename):
    if not filename:
        return None
    dats = open(filename, "r").readlines()

    # i.e. pragma message: cser_dbg_test.c:22:Blarg %d %d!
    pragma_re = re.compile(r".*pragma message: (.*):(\d*):(.*)")

    debug_strs = {}

    for line in dats:
        if "pragma message: " in line:
            msg = pragma_re.match(line)

            if not msg:
                continue

            logging.debug("Found message desc" + msg.groups()[0])

            dstr = cser_dbg_parser.DbgStr(msg.groups()[0], msg.groups()[1], msg.groups()[2])
            logging.debug("DbgStr is " + dstr.msg + " Hash is " + str(dstr.get_hash()))
            debug_strs[dstr.get_hash()] = dstr

    return debug_strs


def main():
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--messages', default=None, action='store', type=str,
                        help='Build log containing hash pragmas (required for hash mode)')
    parser.add_argument('--serial', default=None, action='store', type=str, help='Serial port')
    parser.add_argument('--ip', default=None, action='store', type=str, help='IP address')
    parser.add_argument('--port', default=None, action='store', type=str, help='IP port')
    parser.add_argument('--file', default=None, action='store', type=str, help='File to stream from')
    parser.add_argument('--cser-mode', default=False, action='store_true',
                        help='Pass true to connect to a target which has full cser installed')

    args = parser.parse_args()

    stream = None
    if args.serial:
        stream = cser_stream.SerialStream(args.serial)
    elif args.ip and args.port:
        stream = cser_stream.SocketStream(args.ip, int(args.port))
    elif args.file:
        stream = cser_stream.FileStream(args.file)

    if not stream:
        logging.error("No stream provided")
        sys.exit(0)

    target = CserDebugTarget(stream, get_messages_from_pragma(args.messages), cser_mode=args.cser_mode)

    def handler(signum, frame):
        target.stop()
        sys.exit(0)

    signal.signal(signal.SIGTERM, handler)

    target.start()


if __name__ == "__main__":
    main()
