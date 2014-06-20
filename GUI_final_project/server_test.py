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
	HOST = '127.0.0.1'
	PORT = 12345
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind((HOST, PORT))
	s.listen(1)
	print('server starts')
	conn, addr = s.accept()




	while True:
		data = conn.recv(1024)
		print(data.encode())
		conn.sendall('server echo: '.encode()+data)
	
	print('server is DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
	return 0

if __name__ == '__main__':
	main()

