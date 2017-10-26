# Client class
import socket

# Simple client to mock pics 
class Client():

	def __init__(self, addr, port):
		# Initialize server configuration settings
		self.addr 			= addr
		self.port 			= port
		self.size 			= 1
		# Initialize socket configuration settings
		self.socket 		= None
		# Initialize message constants
		
	# CLIENT METHODS
	#--------------------------------------------------------------------
	# Initializing socket options
	def socket_init(self):
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	# Connect to server
	def connect(self):
		# print("Client connecting to server ...")
		self.socket.connect((self.addr, self.port))
		# print("Client is connected to ...")
	
	# No disconnect because wifly will never have a self generating disconnect

	# Send message to client, returns true if successfully sent
	def sendmsg(self, msg):
		try:
			self.socket.send(msg.encode())
		except ConnectionError:
			raise ConnectionError("Socket is disconnected!")
		return True

	# Receive message from client
	def recvmsg(self):
		data = None
		try:
			data = self.socket.recv(self.size).decode()
		except ConnectionError:
			raise ConnectionError("socket is disconnected!")
		return data 

	#--------------------------------------------------------------------