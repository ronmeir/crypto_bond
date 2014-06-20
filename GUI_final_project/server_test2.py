#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  untitled.py
#  
#  Copyright 2014 ron <ron@ron-Aspire-5738>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  


import socket

def main():	
	port=12350
	size=1000
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
		client_socket.close()
		
		if(data == 'Q' or data == 'q'):
			print('closing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
			break;
	return 0

if __name__ == '__main__':
	main()

