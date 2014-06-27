import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk
import socket               

#we can change the port to send the msg in this function
#usefull because when we close the server the port must be changed
def getPort():
	return 12345

#defines the message that will be sent when pressing the "connect" button
def startMSG():
		return "start_msg"

#this msg is the one that is sent when we want to kill the connection
#this is also the msg the will shown in the massages menu bar
def quitMSG():
	return 'Quit!!!'

#the size of how much to read from the socket is defined in here
def getReadSize():
	return 1000		

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
	#print("ok button was pressed\n")
	msg = om_selected_var.get()
	if(msg==freeText()):
		msg=(msgbox.get('1.0', 'end')).strip()
		msgbox.delete('1.0', 'end')
	insert_text("send - sending message: "+msg)
	send(msg)

#called when the "clear" button is preesed	
def clear_was_pressed():
	textbox.delete('1.0', 'end')

#called when the "connect" button is preesed   
def connect_was_pressed():
   # print("connect button was pressed\n")
	insert_text("connecting...")
	send(startMSG())
    
#txt is the text to be shown in our terminal and also in the regular one
def insert_text(txt):
	textbox.insert(tk.INSERT, txt+"\n")
	print(txt)

#this is the message that'll be shown in the message menu bar
#when we choose it we can sent a free text to the other side sever 
def freeText():
	return 'freeText'			

#msg to be sent via socket	
def send(msg):
	HOST, PORT,size = '127.0.0.1', getPort(), getReadSize()
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	insert_text("	sending :"+msg)
	s.send(msg.encode())
	recv=str(s.recv(size))
	if(recv.find(startMSG())):
		b_send.state(["!disabled"])
		b_clear.state(["!disabled"])

	insert_text("	recieving:"+recv)
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
	
	
	#addOptionsToOptionList(['MSG 1    ','MSG 2    ','MSG 3    ','free text ',quitMSG()])
	addOptionsToOptionList([setMsg(1)    ,setMsg(2)    ,setMsg(3)    ,setMsg(4),setMsg(5)])
	
	
	
	
	
	
	#im_ok  = tk.PhotoImage(file='ok.gif')
	#b_ok = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=ok_was_pressed)
	b_send = ttk.Button(master,text="Send"	  ,command=send_was_pressed,state='disabled')
	b_connect = ttk.Button(master,text="Connect",command=connect_was_pressed)
	b_clear	= ttk.Button(master,text="clear"	  ,command=clear_was_pressed,state='disabled')    
	
	b_connect.grid(row=2,column=1, padx=1, pady=1)
	b_send.grid(row=10,column=0, padx=5, pady=5,columnspan = 2)
	b_clear.grid(row=2,column=0, padx=5, pady=5)

	
	
	textbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 10)
	textbox.grid(row=1,column=2, rowspan=10, columnspan=10, padx=15, pady=15)
	
	
	
	
	msgbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 110,
		height = 1)
	msgbox.grid(row=12,column=0, rowspan=8, columnspan=10, padx=10, pady=5)
	
	

	#textbox.insert(tk.INSERT, "this is how\nwe insert test to the text field\n")
	master.mainloop()

