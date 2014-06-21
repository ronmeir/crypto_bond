#this is server that only echos the messages it recieves
#on close the server we should send to it 'Q' to 'q'
#stands for 'quit'


import socket

def getPort():
	return 12310
	
def startMSG():
		return 'start_msg'
		
def getReadSize():
	return 1000	
	
	
				
def main():	
	port=getPort()
	size=getReadSize()
	server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_socket.bind(("", port))
	server_socket.listen(5)
	print("TCP Server Waiting for client on port: "+str(port))

	while 1:
		client_socket, address = server_socket.accept()
		print("TCP Server received connect from: " + str(address))
		data = client_socket.recv(1000)
		print('		Received: ' +data+"\n")
		client_socket.send(data)
		client_socket.shutdown(0)
		client_socket.close()
		
		if(data == 'Q' or data == 'q'):
			print('closing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
			server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

			break;
	return 0

if __name__ == '__main__':
	main()

