import sqlite3

DELETE_MSG_COMMAND = """
                    DELETE FROM messages WHERE ToClient=?
                    """

GET_MSG_TO_CLIENTS_COMMAND = """
                SELECT * FROM messages WHERE ToClient = ?
                """

INSERT_MSG_TO_TABLE_COMMAND = """INSERT INTO messages
                                  (ToClient, FromClient, Type, Content )
                                  VALUES ( ?, ?, ?, ?);"""

GET_MSG_ID_COMMAND = """SELECT ID FROM messages ORDER BY ID DESC LIMIT 1"""

GET_PUB_KEY_COMMAND = """
            SELECT PublicKey,ID FROM clients WHERE ID = ?
            """

GET_NAME_LST_COMMAND = """
            SELECT Name,ID FROM clients WHERE ID != ?
            """

GET_CLIENT_NAME_COMMAND = """
            SELECT * FROM clients WHERE Name = ?
            """

GET_ID_COMMAND = """
            SELECT * FROM clients WHERE ID = ?
            """

INSERT_CLIENT_TO_TABLE_COMMAND = """INSERT INTO clients
                                  (ID, Name, PublicKey,LastSeen )
                                  VALUES (?, ?, ?, ?);"""

CHECK_IF_CLIENT_TABLE_EXISTS_COMMAND = """
                SELECT count(name) FROM sqlite_master WHERE type="table" AND name="clients"
                """

CREATE_CLIENT_TABLE_CMD = """
         CREATE TABLE clients(ID bytes(16)  ,
         Name bytes(255) NOT NULL PRIMARY KEY, PublicKey bytes(160),LastSeen );
         """

CHECK_IF_MSG_TABLE_EXISTS_CMD = """
                    SELECT count(name) FROM sqlite_master WHERE type="table" AND name="messages"
                    """

CREATE_MSG_TABLE_CMD = """
                     CREATE TABLE messages(ID INTEGER PRIMARY KEY AUTOINCREMENT,
                     ToClient bytes(16), FromClient bytes(16),Type byte(1),Content Blob);
                     """

SERVER_FILE = 'server.db'


def delnmessages(clientid):
    """
    This function clean the table from the messages sent to the client.
    :param clientid: the ID to clear the messages for.
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    cur.execute(DELETE_MSG_COMMAND, (clientid,))
    conn.commit()
    conn.close()


def returnmessages(clientid):
    """
    This function return list of the messages sent to the user.
    :param clientid: the client ID
    :return: list of the messages to the client.
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    cur.execute(GET_MSG_TO_CLIENTS_COMMAND, (clientid,))
    msglist = cur.fetchall()

    conn.commit()
    conn.close()
    return msglist


def msgtoclient(msgData):
    """
    This function enter the message details to the table.
    :param msgData: the message data.
    :return: the new message ID.
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    conn.execute(INSERT_MSG_TO_TABLE_COMMAND, msgData)
    conn.commit()

    cur.execute(GET_MSG_ID_COMMAND)
    msgid = cur.fetchall()
    conn.commit()
    conn.close()
    return msgid


def returnuuid(clientId):
    """
    This function receive client id and return the client public key and ID.
    :param clientId: the client ID.
    :return:  public key, id of all the users.
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    cur.execute(GET_PUB_KEY_COMMAND, (clientId,))
    pub_id = cur.fetchall()
    conn.commit()
    conn.close()
    return pub_id


#
def returnnames(clientId):
    """
    This function receive client id and return list of all the names an ID's except of ID name.
    :param clientId: the client ID.
    :return: list of names and ID's.
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    cur.execute(GET_NAME_LST_COMMAND, (clientId,))
    namelist = cur.fetchall()
    conn.commit()
    conn.close()
    return namelist



def checkusername(name) -> bool:
    """
    This function will check if user name is all ready in use.
    :param name: the user name.
    :return: True - user name don't exist, False - user name already exist
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    cur.execute(GET_CLIENT_NAME_COMMAND, (name,))
    a = cur.fetchall()
    conn.commit()
    conn.close()
    if not a:
        return True
    return False



def checkuuid(clientId) -> bool:
    """
    This function will check if user ID is all ready in use.
    :param name: the client ID.
    :return: True - user ID don't exist, False - user ID already exist
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()
    cur.execute(GET_ID_COMMAND, (clientId,))
    a = cur.fetchall()
    conn.commit()
    conn.close()
    if not a:
        return True
    return False


def signuser(dataTuple):
    """
    This function insert new user to the table.
    :param dataTuple: full user sign in details.
    """
    connect = sqlite3.connect(SERVER_FILE)
    connect.text_factory = bytes
    connect.execute(INSERT_CLIENT_TO_TABLE_COMMAND, dataTuple)
    connect.commit()
    connect.close()



def createTables():
    """
    This function create the SQL tables, if they are not already created.
    """
    conn = sqlite3.connect(SERVER_FILE)
    conn.text_factory = bytes
    cur = conn.cursor()

    cur.execute(CHECK_IF_CLIENT_TABLE_EXISTS_COMMAND)
    a = cur.fetchall()
    if a[0][0] == 0:
        cur.executescript(CREATE_CLIENT_TABLE_CMD)

    cur.execute(CHECK_IF_MSG_TABLE_EXISTS_CMD)
    a = cur.fetchall()
    if a[0][0] == 0:
        cur.executescript(CREATE_MSG_TABLE_CMD)

    conn.commit()
    conn.close()
