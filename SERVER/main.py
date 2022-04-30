import selectors
import socket
import sql
import uuid
import struct
import sqlite3
import os.path
from datetime import datetime

sel = selectors.DefaultSelector()

# Constants

CLIENT_ID_LEN = 16
BUFFER_SIZE = 1024
SERVER_VERSION = 2
CLIENT_VER_OP1 = 1
CLIENT_VER_OP2 = 2
PORT_INFO_FILE = "port.info"
HOST = 'localhost'
SERVER_LISTEN = 100
REQ_CODE_REG_USER = 1000
REQ_CODE_USERS_LIST = 1001
REQ_CODE_GET_PUBKEY = 1002
REQ_CODE_SEND_MSG = 1003
REQ_CODE_GET_MSGS = 1004

RSP_CODE_REG_USER = 2000
RSP_CODE_USERS_LIST = 2001
RSP_CODE_GET_PUBKEY = 2002
RSP_CODE_SEND_MSG = 2003
RSP_CODE_GET_MSGS = 2004
RSP_CODE_ERROR = 9000

FAIL = -1
SUCCESS = 0
UNPACK_HEADER = '<16sBHI'
UNPACK_PAY_CLIENT_ID = '16s'
UNPACK_MSG = '<16sBI'
UNPACK_USER_REGISTER = '255s160s'
PACK_HDR = '<BHI'
PACK_FULL_DATA = '<BhI%ds'
PACK_USER_ID_NAME = '16s255s'
PACK_USER_ID_PUBKEY = '16s160s'
PACK_USER_ID_MSGID = '16sI'
PACK_MSG_TO_USER = '<16sIBI'

DATE_TIME_STRING = "%d/%m/%Y %H:%M:%S"


def req_1000(payload):
    """
     This function receive the client input payload it give it a uuid, check if
     the user name already exists and sign  new user to table,
    :param payload: the data from client
    :return: the user uuid if everything is ok, FAIL if something didn't work
    """
    # get date and time
    now = datetime.now()
    date_string = now.strftime(DATE_TIME_STRING)
    # get uuid and check it is unique
    uid = uuid.uuid4().bytes
    while not sql.checkuuid(uid):
        uid = uuid.uuid4().bytes
    # unpack payload
    name, pubkey = struct.unpack(UNPACK_USER_REGISTER, payload)
    name = payload[:payload.find(b'\x00')].decode('ascii')
    data_tuple = (uid, name, pubkey, date_string)

    if sql.checkusername(data_tuple[1]):
        sql.signuser(data_tuple)
        return uid
    else:
        return FAIL


def req_1001(client_id):
    """
    This function send back to the asking user the user list
    :param client_id: the requesting client ID
    :return: the client name list .
    """
    if not sql.checkuuid(client_id):
        namelist = sql.returnnames(client_id)
        return namelist
    return FAIL


def req_1002(payload):
    """
    This function send back to the asking user  the ashed client public key.
    :param payload: the client request payload data
    :return: the requested client pubkey and ID, FAIL if something didn't work
    """
    client_id = struct.unpack(UNPACK_PAY_CLIENT_ID, payload)
    if not sql.checkuuid(client_id[0]):
        client_pubkey = sql.returnuuid(client_id[0])
        return client_pubkey
    else:
        return FAIL


def req_1003(client_id, payload):
    """
    This function save a message to a client from a different client.
    :param client_id: the request client ID
    :param payload: the client request details.
    :return: the requested the receiver ID, the message ID.
    """

    receiver_id, msg_type, content_size = struct.unpack(UNPACK_MSG, payload[:struct.calcsize(UNPACK_MSG)])
    if not sql.checkuuid(client_id) and not sql.checkuuid(receiver_id):
        msg_content = payload[struct.calcsize(UNPACK_MSG):struct.calcsize(UNPACK_MSG) + content_size]
        data_tuple = (receiver_id, client_id, msg_type, sqlite3.Binary(msg_content))
        msg_id = sql.msgtoclient(data_tuple)
        msg_id = msg_id[0]
        return [receiver_id, msg_id]
    else:
        return FAIL


def req_1004(data):
    """
    This function send back to the asking user all of his awaiting messages.
    :param data: the client request details
    :return: the requested client messages from the table.
    """
    if not sql.checkuuid(data[:CLIENT_ID_LEN]):
        msg_list = sql.returnmessages(data[:CLIENT_ID_LEN])
        return msg_list
    else:
        return FAIL


def delmaessages(data):
    """
    This function delete all the messages that where send back to the user
    :param data: the client details
    :return:
    """
    sender_id = data[:CLIENT_ID_LEN]
    sql.delnmessages(sender_id)


def accept(sock, mask):
    """
    This function create new connection
    :param sock:
    :param mask:
    :return:
    """
    conn, addr = sock.accept()
    conn.setblocking(False)
    sel.register(conn, selectors.EVENT_READ, read)


def checkinput(version, pay_size, data_len):
    """
    This function check the data received from the user for errors.
    :param version: user version
    :param pay_size: payload size
    :param data_len: total data size
    :return: True if all is fine, False if something went wrong
    """
    if version != CLIENT_VER_OP1 and version != CLIENT_VER_OP2:
        return False
    if (pay_size + struct.calcsize(UNPACK_HEADER)) != data_len:
        return False
    return True


def handeldata(data):
    """
    This function receive the data from the client parse it and
     direct it to the correct function by the received code.
    :param data: the data from the client.
    :return: the respond to the client,
    """
    client_id, ver, code, pay_size = struct.unpack(UNPACK_HEADER, data[:struct.calcsize(UNPACK_HEADER)])
    if not checkinput(ver, pay_size, len(data)):
        code = RSP_CODE_ERROR
    if code == REQ_CODE_REG_USER:
        respond = req_1000(data[struct.calcsize(UNPACK_HEADER):])
        if respond != FAIL:
            data = struct.pack(PACK_FULL_DATA % CLIENT_ID_LEN, SERVER_VERSION, RSP_CODE_REG_USER, CLIENT_ID_LEN,
                               respond)
        else:
            header = struct.pack(PACK_HDR, SERVER_VERSION, RSP_CODE_ERROR, 0)
            data = header
    elif code == REQ_CODE_USERS_LIST:
        name_list = req_1001(client_id)
        payload = b''
        for name, cl_id in name_list:
            payload += struct.pack(PACK_USER_ID_NAME, cl_id, name)
        header = struct.pack(PACK_HDR, SERVER_VERSION, RSP_CODE_USERS_LIST, len(payload))
        data = header + payload
    elif code == REQ_CODE_GET_PUBKEY:
        pub = req_1002(data[struct.calcsize(UNPACK_HEADER):])
        if pub != FAIL:
            payload = b''
            for pubkey, cl_id in pub:
                payload += struct.pack(PACK_USER_ID_PUBKEY, cl_id, pubkey)
            data = struct.pack(PACK_FULL_DATA % len(payload), SERVER_VERSION, RSP_CODE_GET_PUBKEY, len(payload),
                               payload)
        elif pub == FAIL:
            header = struct.pack(PACK_HDR, SERVER_VERSION, RSP_CODE_ERROR, 0)
            data = header
    elif code == REQ_CODE_SEND_MSG:
        ret = req_1003(client_id, data[struct.calcsize(UNPACK_HEADER):])
        cid, mid = ret
        payload = struct.pack(PACK_USER_ID_MSGID, cid, mid[0])
        data = struct.pack(PACK_FULL_DATA % len(payload), SERVER_VERSION, RSP_CODE_SEND_MSG, len(payload), payload)
    elif code == REQ_CODE_GET_MSGS:
        msg_list = req_1004(data)
        payload = b''
        for msg_id, to_client_id, from_client_id, msg_type, content in msg_list:
            payload += struct.pack(PACK_MSG_TO_USER, from_client_id, msg_id, msg_type, len(content))
            payload += content
        delmaessages(data[:CLIENT_ID_LEN])
        header = struct.pack(PACK_HDR, SERVER_VERSION, RSP_CODE_GET_MSGS, len(payload))
        data = header + payload
    else:
        header = struct.pack(PACK_HDR, SERVER_VERSION, RSP_CODE_ERROR, 0)
        data = header
    return data


def read(conn, mask):
    """
     this function read the data from the client and send
     the data back to the user.
    :param conn: connection object
    :param mask: provide info about the channel status
    :return:
    """
    data = bytes()
    while True:
        try:
            peace = conn.recv(BUFFER_SIZE)
        except ConnectionResetError:
            return None
        data += peace
        if len(peace) < BUFFER_SIZE:
            break

    data = handeldata(data)
    if data:
        conn.send(data)
        sel.unregister(conn)
        conn.close()
    else:
        sel.unregister(conn)
        conn.close()


def socke(port):
    """
    This function define the socket,ant connect it to the selector
    :param port: port number
    """
    sock = socket.socket()
    sock.bind((HOST, int(port)))
    sock.listen(SERVER_LISTEN)
    sock.setblocking(False)
    sel.register(sock, selectors.EVENT_READ, accept)
    while True:
        events = sel.select()
        for key, mask in events:
            callback = key.data
            callback(key.fileobj, mask)


def main():
    """
    the main function of the program, open the port file, and create the sql table if exists
    """
    if os.path.isfile(PORT_INFO_FILE):
        f = open(PORT_INFO_FILE, "r")
        po = f.readline().strip()
        if po.isdigit() and 1 < int(po) <= 65535:
            sql.createTables()
            socke(po)
        else:
            print("invalid port in port.info file.")
    else:
        print("port.info file is missing.")


if __name__ == '__main__':
    main()
