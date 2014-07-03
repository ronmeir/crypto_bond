import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk
import socket  
import easygui



####################################################################################################################
######################################## CONSTS  ###############################################3###################
####################################################################################################################
#    __  ___  ____   ___________  _____
#   /  ]/   \|    \ / ___|      T/ ___/
#  /  /Y     |  _  (   \_|      (   \_ 
# /  / |  O  |  |  |\__  l_j  l_j\__  T
#/   \_|     |  |  |/  \ | |  |  /  \ |
#\     l     |  |  |\    | |  |  \    |
# \____j\___/l__j__j \___j l__j   \___j
                                      
#we can change the port to send the msg in this function
#usefull because when we close the server the port must be changed
def getPort():
	return 12346

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
####################################################################################################################




#-----------------------------------------------------------------------------------------------------------#
#############################################################################################################
############### MSGs to the GUI SERVER ######################################################################
# ___ ___  _____ ____  _____     ______  ___        _____  ___ ____  __ __   ___ ____  
#|   T   T/ ___//    T/ ___/    |      T/   \      / ___/ /  _|    \|  T  | /  _|    \ 
#| _   _ (   \_Y   __(   \_     |      Y     Y    (   \_ /  [_|  D  |  |  |/  [_|  D  )
#|  \_/  |\__  |  T  |\__  T    l_j  l_|  O  |     \__  Y    _|    /|  |  Y    _|    / 
#|   |   |/  \ |  l_ |/  \ |      |  | |     |     /  \ |   [_|    \l  :  |   [_|    \ 
#|   |   |\    |     |\    |      |  | l     !     \    |     |  .  Y\   /|     |  .  Y
#l___j___j \___l___,_j \___j      l__j  \___/       \___l_____l__j\_j \_/ l_____l__j\_j
#############################################################################################################                                                                                      

def FILED_SEPERATOR():
	return chr(7)


def MSG_create_sk_and_bond():
	return (("1"+FILED_SEPERATOR(),"sending:create SK and Bond MSG"))
		
def MSG_send_sk_and_bond_to_CA():
	return (("2"+FILED_SEPERATOR(),"sending:SK and Bond to the CA"))

def MSG_send_sk_and_bond_to_server():
	return (("3"+FILED_SEPERATOR(),"sending:SK and Bond to the Server"))

def MSG_send_this_msg(msg):
	return (("4"+FILED_SEPERATOR()),msg)
	


#this function returns the msg that should be shown in the i'th place of the menu - we use it  is addOptionsToOptionList
def setMsg(i):
	if(i==1):
		return freeText()	
	if(i==2):
		return 'MSG1'
	if(i==3):
		return 'MSG2'
	if(i==4):
		return 'MSG3'
	if(i==5):
		return quitMSG()


########## end of MSGs SERVER################################################################################
#############################################################################################################
#-----------------------------------------------------------------------------------------------------------#







###################################################################################################################
###################  User Interface    ############################################################################
###################################################################################################################
# __ __  _____  ___ ____       ____ ____  ______   ___ ____  _____  ____    __   ___ 
#|  T  T/ ___/ /  _|    \     l    |    \|      T /  _|    \|     |/    T  /  ] /  _]
#|  |  (   \_ /  [_|  D  )     |  T|  _  |      |/  [_|  D  |   __Y  o  | /  / /  [_ 
#|  |  |\__  Y    _|    /      |  ||  |  l_j  l_Y    _|    /|  l_ |     |/  / Y    _]
#|  :  |/  \ |   [_|    \      |  ||  |  | |  | |   [_|    \|   _]|  _  /   \_|   [_ 
#l     |\    |     |  .  Y     j  l|  |  | |  | |     |  .  |  T  |  |  \     |     T
# \__,_j \___l_____l__j\_j    |____l__j__j l__j l_____l__j\_l__j  l__j__j\____l_____j
###################################################################################################################                                                                                    
###################################################################################################################                                                                                    
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
		
		
		if(msg==""):
			#we must send some MSG - otherwise we'll get error
			easygui.msgbox("You cannot send an empty MSG!!!", title="Error")
			
	#now we can send the MSG		
	if(msg!=""):
		send(msg)
		
	msgbox.delete('1.0', 'end')				# clear the msgbox	

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
	textbox.see('end')

#this is the message that'll be shown in the message menu bar
#when we choose it we can sent a free text to the other side sever 
def freeText():
	return 'Text  '			

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
	
	#print(msg)
	
	s.close
	
def keyPress(event):


	#ESC clears text box
	if event.keysym == 'Escape':
		clear_was_pressed()
	#Enter sends MSG
	elif event.keysym == 'Return':
			send_was_pressed()
		
		
        
	
if __name__ == "__main__":
	global var1, optionList, om_selected_var, om,isConnected
	master = tk.Tk()
	master.geometry('680x210')
	master.title("Client GUI") # the title of the window
	isConnected="F"
	
	
	om_selected_var = tk.StringVar()
	optionList = ['None']
	om = ttk.OptionMenu(master,om_selected_var,*optionList)
	om_selected_var.set(optionList[0])
	
	om.grid(row=11,column=0)#,columnspan = 2
	
	
	addOptionsToOptionList([setMsg(i) for i in range(1,6)])
	
	
	
	
	
	
	#im_ok  = tk.PhotoImage(file='ok.gif')
	#b_ok = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=ok_was_pressed)
	
	
	b_send = ttk.Button(master,text="Send(Enter)"	  ,command=send_was_pressed)#,state='disabled'
	b_connect = ttk.Button(master,text="Connect",command=connect_was_pressed )
	b_clear	= ttk.Button(master,text="Clear(Esc)"	  ,command=clear_was_pressed)    
	
	b_clear.grid(row=1,column=1, pady=5)#, padx=5
	b_connect.grid(row=2,column=0, pady=1)#padx=1,
	
	titleLabel = ttk.Label(master,text="choose MSG:")
	titleLabel.grid(row=10,column=0)
	
	b_send.grid(row=12,column=0, pady=5, padx=5)#,columnspan = 2
	
	
	textbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 9)
	textbox.grid(row=2,column=1, rowspan=10, padx=5)# columnspan=10,, pady=15
	textbox.tag_configure('green', foreground='#01DF01')
	textbox.tag_configure('red', foreground='#DF0101')
	textbox.tag_configure('black', foreground='#000000')
	textbox.tag_configure('blue', foreground='#0000FF')


	
	
	msgbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 1)
	msgbox.grid(row=12,column=1, rowspan=8, columnspan=10)#, padx=10, pady=5
	msgbox.focus_set()
	
	

	#textbox.insert(tk.INSERT, "this is how\nwe insert test to the text field\n")
	master.bind_all('<KeyRelease>', keyPress)
	master.mainloop()
###################################################################################################################
#######################    END Of User Interface     ##############################################################
###################################################################################################################
