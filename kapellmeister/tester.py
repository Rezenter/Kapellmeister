import socket

_s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print('WTF???')
print(_s.sendto((0b000011100000000000000000000000000000111000001110).to_bytes(6, byteorder='little'), ('192.168.10.70', 8888)))
print('\n...\n')

print(_s.recv(6))
print('code OK')
