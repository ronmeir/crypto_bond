import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk
import socket  
import easygui
 
#we can change the port to send the msg in this function
#usefull because when we close the server the port must be changed
def getPort():
	return 12345

def getIP():
	return '127.0.0.1'
#defines the message that will be sent when pressing the "connect" button
def startMSG():
		return "start_msg"

#this msg is the one that is sent when we want to kill the connection
#this is also the msg the will shown in the massages menu bar
def quitMSG():
	return 'Quit!!!'

#the size of how much to read from the socket is defined in here
def getReadSize():
	return 8192		

#this function returns the msg that should be shown in the i'th place of the menu - we use it  is addOptionsToOptionList
def setMsg(i):
	if(i==1):
		return 'MSG1'
	if(i==2):
		return 'MSG2'
	if(i==3):
		return 'MSG3'
	if(i==4):
		return freeText()
	if(i==5):
		return quitMSG()
	
#this is the function that sets the menu bar
def addOptionsToOptionList(l=[]):
    om_selected_var.set('')
    om['menu'].delete(0,'end')
    for choice in l:
        om['menu'].add_command(label=choice,command=tk._setit(om_selected_var,choice))
    om_selected_var.set(l[0])

#called when the "send" button is preesed
def send_was_pressed():
	#check what msg do we want to send
	msg = om_selected_var.get()
	#if it's a freeText msg we need to send the text in the msgbox 
	#otherwise we send the actual MSG
	if(msg==freeText()):
		msg=(msgbox.get('1.0', 'end')).strip()	#remove emptySpaces
		msgbox.delete('1.0', 'end')				# clear the msgbox
		
		if(msg==""):
			#we must send some MSG - otherwise we'll get error
			easygui.msgbox("You cannot send an empty MSG!!!", title="Error")
			
	#now we can send the MSG		
	if(msg!=""):
		send(msg)
		
		

#called when the "clear" button is preesed	
def clear_was_pressed():
	textbox.delete('1.0', 'end')

#called when the "connect" button is preesed   
def connect_was_pressed():
   # print("connect button was pressed\n")
	insert_text("connecting...")
	insert_text("	IP  :"+str(getIP()))
	insert_text("	PORT:"+str(getPort())+"\n")
	send(startMSG())
    
#txt is the text to be shown in our terminal and also in the regular one
#the color parm sets the color of the text in the terminal
#it can be one of these colors only: {'green','blue','black','red'}
#if we don't choose coler the msg we be painted black
def insert_text(txt,color='black'):
	textbox.insert(tk.INSERT, txt+"\n",color)

#this is the message that'll be shown in the message menu bar
#when we choose it we can sent a free text to the other side sever 
def freeText():
	return 'freeText'			

#msg to be sent via socket	
def send(msg):
	#define host ip, port and the size of the buffer to be read from socket
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect((getIP(), getPort()))
	except ConnectionRefusedError:
		insert_text("Connection refused",'red')
		return
		
	#a normal msg that we send should be green
	color='green'
	
	
		
	insert_text(msg,color)
	s.send(msg.encode())
	recv=str(s.recv(getReadSize()).decode('utf-8'))

	#when we get a startMSG as echo it means the server work and we have connection
	#all the buttons and be enabled!!
	if(recv==str(startMSG())):
		b_send.state(["!disabled"])

	color='blue'
	#recv=str(recv))
	insert_text(recv+"\n",color)
	
	print(msg)
	
	s.close
	
	
		
		
	



	
if __name__ == "__main__":
	global var1, optionList, om_selected_var, om
	master = tk.Tk()
	master.geometry('800x250')
	master.title("Client GUI") # the title of the window
	#titleLabel = ttk.Label(master,text="press the Connect button")
	#titleLabel.grid(row=0,column=0)
	
	om_selected_var = tk.StringVar()
	optionList = ['None']
	om = ttk.OptionMenu(master,om_selected_var,*optionList)
	om_selected_var.set(optionList[0])
	
	om.grid(row=1,column=0,columnspan = 2)
	
	
	addOptionsToOptionList([setMsg(i) for i in range(1,6)])
	
	
	
	
	
	
	#im_ok  = tk.PhotoImage(file='ok.gif')
	#b_ok = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=ok_was_pressed)
	b_send = ttk.Button(master,text="Send"	  ,command=send_was_pressed,state='disabled')
	b_connect = ttk.Button(master,text="Connect",command=connect_was_pressed)
	b_clear	= ttk.Button(master,text="Clear"	  ,command=clear_was_pressed)    
	
	b_connect.grid(row=2,column=1, padx=1, pady=1)
	b_send.grid(row=10,column=0, padx=5, pady=5,columnspan = 2)
	b_clear.grid(row=2,column=0, padx=5, pady=5)

	
	
	textbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 10)
	textbox.grid(row=1,column=2, rowspan=10, columnspan=10, padx=15, pady=15)
	textbox.tag_configure('green', foreground='#01DF01')
	textbox.tag_configure('red', foreground='#DF0101')
	textbox.tag_configure('black', foreground='#000000')
	textbox.tag_configure('blue', foreground='#0000FF')


	
	
	msgbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 110,
		height = 1)
	msgbox.grid(row=12,column=0, rowspan=8, columnspan=10, padx=10, pady=5)
	
	

	#textbox.insert(tk.INSERT, "this is how\nwe insert test to the text field\n")
	master.mainloop()

