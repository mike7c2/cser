import re
import struct
import logging

logger = logging.getLogger(__name__)


def byte_array_to_int(a, signed=False):
    mul = 1
    v = 0

    for x in a:
        v += ord(x) * mul
        mul *= 256

    if signed:
        if a[0] & 0x80:
            v -= (mul/2)
            v = -(v+1)

    return v


class Token(object):
    def __init__(self, regex):
        self.regexStr = regex
        self.regex = re.compile(regex)


class FloatToken(Token):
    def __init__(self):
        super(FloatToken, self).__init__("%f")

    def token_parse(self, string, data):
        ret = str(struct.unpack("f", "".join(x for x in data[0:4]))[0])
        return (4, ret)


class StringToken(Token):
    def __init__(self):
        super(StringToken, self).__init__("%s")

    def token_parse(self, string, data):
        ret = "".join(data)
        width = len(data)
        return (width, ret)


class ArrayToken(Token):
    def __init__(self):
        super(ArrayToken, self).__init__("%H")

    def token_parse(self, string, data):
        ret = ":".join([hex(ord(d)) for d in data])
        width = len(data)
        return (width, ret)


class IntToken(Token):
    def __init__(self):
        super(IntToken, self).__init__(r"%(-?)(\+?)(0{0,1}\d{0,1})(l{1,2}|h{0,2})(u|d|x|X)")
        self._width_token_map = {
            "hh": 1,
            "h": 2,
            "": 4,
            "l": 4,
            "ll": 8
        }

    def token_parse(self, string, data):
        match = self.regex.match(string).groups()

        # match[0] is - sign if present
        # match[1] is + sign if present
        # match[2] is padding (0\d?) specifying width and 0 padding if required
        # match[3] is size modifier
        # match[4] is duxX

        left_justify = True if len(match[0]) > 0 else False
        # print_sign = True if len(match[1]) > 0 else False
        pad_width = int(match[2][-1]) if match[2] else 0
        zero_pad = True if len(match[2]) > 1 and "0" in match[2] else False
        signed = True if "d" in match[3] else False

        # Figure out width in bytes and take from data
        width = self._width_token_map[match[3]]
        data_slice = data[0:width]
        val = byte_array_to_int(data_slice, signed)
        ret = ("%" + match[4]) % val

        if len(ret) < pad_width:
            if zero_pad:
                p = "0"
            else:
                p = " "
            pad = p * (pad_width - len(ret))

            if left_justify:
                ret = ret + pad
            else:
                ret = pad + ret

        return (width, ret)


class TokenParser(object):

    def __init__(self, tokens):
        self.tokens = tokens
        regex = "|".join(["(" + re.sub("[(]", "(?:", token.regexStr) + ")" for token in tokens])
        self.regex = re.compile(regex)

    # Parse a string returning its representation with token replaced with data
    def parse(self, string, data):
        end = 0
        ret = ""

        while end < len(string):
            m = self.regex.search(string)

            # If there are no matches copy rest of string
            if not m:
                ret += string[end:]
                break
            # If match wasnt at start of string copy the characters before it
            if m.start() > 0:
                ret += string[0:m.start()]

            # Determine which token type matched
            matches = m.groups()
            for i, match in enumerate(matches):
                if match:
                    # Interpret the token and copy the appropriate output
                    width, s = self.tokens[i].token_parse(match, data)
                    data = data[width:]
                    ret += s

            string = string[m.end():]
        return ret


class DbgStr(object):
    def __init__(self, fil, line, msg):
        self.file = fil
        self.line = line
        self.msg = msg

    def get_hash(self):
        h = 0

        for c in self.msg:
            h += ord(c)
            h *= 65599
            h &= 0xFFFFFFFF

        return h

    def __str__(self):
        return ":".join([self.file, self.line, self.msg, hex(self.get_hash())])


class CserDebugParser(object):

    def __init__(self, messages, handler=None):

        self.name = "dbg_parser"
        if messages:
            logger.debug("Initialising parser with messages : " + str(messages))
            self.hash_mode = True
        else:
            self.hash_mode = False

        self.dbg_messages = messages
        self.parser = TokenParser([IntToken(), ArrayToken(), FloatToken(), StringToken()])
        self._handler = handler

    def receive_packet(self, msg):
        # logger.debug("Received message " + str(msg) + " len " + str(len(msg)))
        if len(msg) < 4:
            logger.warning("Short message! length " + str(len(msg)))
            return

        data_start = None
        dstr = None

        if self.hash_mode:
            # Get format string from hash lookup table
            hsh = struct.unpack("<I", "".join(x for x in msg[0:4]))[0]

            if hsh in self.dbg_messages:
                dstr = self.dbg_messages[hsh].msg
                data_start = 4
            else:
                logging.warning("Missing hash: " + str(hsh))
                return

        else:
            # Get the format string from the message
            null_idx = msg.index("\x00")
            # s = "".join([x for x in msg[0:null_idx]])
            data_start = null_idx + 1
            dstr = msg[:data_start]

        output = self.parser.parse(dstr, msg[data_start:]).replace("\n", "")
        logging.info("Log Msg:" + output)

        if self._handler:
            self._handler(output)
