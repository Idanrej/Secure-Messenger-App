# Secure-Messenger-App
The app server side is written in python, the client side is written in C++.

The messages sent by the clients and encrypted and stored in the server,
so that user can be online or offline ans get the messages.
the end-to-end encryption is implemented usind private and public key, so that only the user can read the messages.
the simetric encryption use AES-CBC and the key length is 128 bit.
for the a-simetric encryption I used RSA with key length of 1024 bit.

