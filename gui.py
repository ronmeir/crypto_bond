import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk
import socket 
import easygui
import os


############################ IP and PORT form UI ########################################
IP='10.0.0.11' 
PORT= 60001
#########################################################################################





######################################## CONSTS  ###################################################################
####################################################################################################################
#    __  ___  ____   ___________  _____
#   /  ]/   \|    \ / ___|      T/ ___/
#  /  /Y     |  _  (   \_|      (   \_ 
# /  / |  O  |  |  |\__  l_j  l_j\__  T
#/   \_|     |  |  |/  \ | |  |  /  \ |
#\     l     |  |  |\    | |  |  \    |
# \____j\___/l__j__j \___j l__j   \___j
#
####################################################################################################################                                      
#we can change the port to send the msg in this function
#usefull because when we close the server the port must be changed
def getPort():
	return PORT

def getIP():
	return IP
#defines the message that will be sent when pressing the "connect" button
def startMSG():
		return "start_msg"

'''
#this msg is the one that is sent when we want to kill the connection
#this is also the msg the will shown in the massages menu bar
'''
def quitMSG():
	return 'Quit!!!'

#the size of how much to read from the socket is defined in here
def getReadSize():
	return 8192		
	
def numOfMSGs():
	return 5	


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

#send = msg content :      src	| dst | opc| cont-length	|cont
#txt will be shown at the client's terminal
# the   "send+=send+send+send+send"   padding is done to help the server parsing the MSG

def FILED_SEPERATOR():
	return chr(7)
def buildMSG(src,dst,op,leng,cont):
	return str(src)+FILED_SEPERATOR()+str(dst)+FILED_SEPERATOR()+str(op)+FILED_SEPERATOR()+ str(leng)+FILED_SEPERATOR()+str(cont)+FILED_SEPERATOR()



def MSG_req_sm_from_server():
	send=buildMSG(0,0,0,1,1)
	#send= str("0"+FILED_SEPERATOR())
	#send+=send+send+1+1
	txt="(0) request SM from server"
	return ((send,txt))
	
def MSG_create_sk_and_bond():
	#send= str("1"+FILED_SEPERATOR())
	#send+=send+send+1+1
	send=buildMSG(1,1,1,1,1)
	txt="(1) sent:create SK and Bond MSG"
	return ((send,txt))
		
def MSG_send_sk_and_bond_ca():
	#send= str("2"+FILED_SEPERATOR())
	#send+=send+send+1+1
	send=buildMSG(2,2,2,1,1)
	txt="(2) sent:SK and Bond to CA"
	return ((send,txt))
	
	
	
def MSG_send_sk_and_bond_to_server():
	#send= str("3"+FILED_SEPERATOR())
	#send+=send+send+"1+1
	send=buildMSG(3,3,3,1,1)
	txt="(3) sent:SK and Bond are sending to Server"
	return ((send,txt))
	


def MSG_send_this_msg(txt):
	#send= str("4"+FILED_SEPERATOR())
	#send+=send+send+str(len(txt))+txt
	send=buildMSG(4,4,4,len(txt),txt)
	return ((send,txt))
	


#this function returns the msg that should be shown in the i'th place of the menu - we use it  is addOptionsToOptionList
def setMsg(i):
	if(i==0):
		return '(0) request SM from Server      '		
	if(i==1):
		return '(1) create SK and Bond           '
	if(i==2):
		return '(2) send SK and Bond to CA     '
	if(i==3):
		return '(3) send SK and Bond to Server'
	if(i==4):
		return freeText()
		

########## end of MSGs to SERVER#############################################################################
############################# ################################################################################
#-----------------------------------------------------------------------------------------------------------#



################################# MSGs from server   #########################################################
## ___ ___  _____ ____  _____     _____ ____   ___  ___ ___       _____  ___ ____  __ __   ___ ____ 		##
##|   T   T/ ___//    T/ ___/    |     |    \ /   \|   T   T     / ___/ /  _|    \|  T  | /  _|    \ 		##
##| _   _ (   \_Y   __(   \_     |   __|  D  Y     | _   _ |    (   \_ /  [_|  D  |  |  |/  [_|  D  )		##
##|  \_/  |\__  |  T  |\__  T    |  l_ |    /|  O  |  \_/  |     \__  Y    _|    /|  |  Y    _|    / 		##
##|   |   |/  \ |  l_ |/  \ |    |   _]|    \|     |   |   |     /  \ |   [_|    \l  :  |   [_|    \ 		##
##|   |   |\    |     |\    |    |  T  |  .  l     |   |   |     \    |     |  .  Y\   /|     |  .  Y		##
##l___j___j \___l___,_j \___j    l__j  l__j\_j\___/l___j___j      \___l_____l__j\_j \_/ l_____l__j\_j		##
##																											##
##  msg content :      src	| dst | opc| cont-length	|cont												##
##############################################################################################################


#this function receives a msg (string) from the server and parses it to array
def MSG_parseMSG(msg,location=-1):
		msgList=msg.split(FILED_SEPERATOR()) #msglist[i]= {src,dst,opc,length of content,contend}
		return msgList
		




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
	
	#if it's a free text MSG:
	if(msg==freeText()):
		msg=(msgbox.get('1.0', 'end')).strip()	#remove emptySpaces
		if(msg==""):
			#we must send some MSG - otherwise we'll get error
			easygui.msgbox("You cannot send an empty MSG!!!", title="Error")		
		else:
			msg=MSG_send_this_msg(msg) #create a msg tuple (msg,txt)
			send(msg)
			msgbox.delete('1.0', 'end')				# clear the msgbox	
			
	#if it's a built-in MSG		
	else:
		if(msg==setMsg(0)):
			msg=MSG_req_sm_from_server()
			
		elif(msg==setMsg(1)):
			msg=MSG_create_sk_and_bond()
			
		elif(msg==setMsg(2)):
			msg=MSG_send_sk_and_bond_ca()
		
		elif(msg==setMsg(3)):
			#msg=MSG_send_to_server_sk_and_bond()
			#msg=MSG_send_sk_and_bond_to_server()
			msg=MSG_send_sk_and_bond_to_server()
		send(msg)				
					
			

'''
	#now we can send the MSG		
	if(msg!=""):
		msg=MSG_send_this_msg(msg) #create a msg tuple (msg,txt)
		send(msg)
			msgbox.delete('1.0', 'end')				# clear the msgbox	
'''		


#called when the "clear" button is preesed	
def clear_was_pressed():
	textbox.delete('1.0', 'end')
	#print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nscreen was cleared!!!")


#called when the "connect" button is preesed   
def connect_was_pressed():
	insert_text("connecting...")
	insert_text("	IP  :"+str(getIP()))
	insert_text("	PORT:"+str(getPort())+"\n")
	
	#print("GUI server info:	IP:"+str(getIP())+"	PORT:"+str(getPort()))
	msg=MSG_req_sm_from_server()	#get to relevant msg tuple (toSend,toShow)
	send(msg)						#this is the actual MSG sent 
	
'''    
#txt is the text to be shown in our terminal and also in the regular one
#the color parm sets the color of the text in the terminal
#it can be one of these colors only: {'green','blue','black','red'}
#if we don't choose coler the msg we be painted black
'''
def insert_text(txt,color='black'):
	textbox.insert(tk.INSERT, txt+"\n",color)
	textbox.see('end')
	
'''
#this is the message that'll be shown in the message menu bar
#when we choose it we can sent a free text to the other side sever 
'''
def freeText():
	return '(4) type and send free text  '			

#msg to be sent via socket	
def send(msg):
	
	#define host ip, port and the size of the buffer to be read from socket
	color='green'
	insert_text(str(msg[1]),color)
	
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect((getIP(), getPort()))
	except ConnectionRefusedError:
		insert_text("Connection refused",'red')
		return
		
	#a normal msg that we send should be green
	
	
	#print("printing msg:"+str(msg))	
	#insert_text(msg,color) 
	
	s.send(msg[0].encode())
	#recv=str(s.recv(getReadSize()).decode('utf-8'))
	recv=str(s.recv(getReadSize()).decode('utf-8', 'ignore'))

	#when we get a startMSG as echo it means the server work and we have connection
	#all the buttons and be enabled!!
	if(recv==str(startMSG())):
		b_send.state(["!disabled"])

	color='blue'
	insert_text(MSG_parseMSG(recv)[4],color)
	insert_text("--------------------------------------------------------------------------------\n")
	
	
	#print(msg)
	
	s.close
	
def keyPress(event):

	#ESC clears text box
	if event.keysym == 'Escape':
		clear_was_pressed()
	#Enter sends MSG
	elif event.keysym == 'Return':
			send_was_pressed()
			msgbox.delete('1.0', 'end')
		
	
if __name__ == "__main__":
	
	global var1, optionList, om_selected_var, om,isConnected
	master = tk.Tk()
	master.geometry('680x233')
	master.title("Client GUI") 	# the title of the window
	master.resizable(0,0)		# disable maximize
	isConnected="F"

	
	
	om_selected_var = tk.StringVar()
	optionList = ['None']
	om = ttk.OptionMenu(master,om_selected_var,*optionList)
	om_selected_var.set(optionList[0])
	
	om.grid(row=20,column=1,pady=2)#,columnspan = 2
	
	addOptionsToOptionList([setMsg(i) for i in range(0,numOfMSGs())])
	
	#im_ok  = tk.PhotoImage(file='ok.gif')
	#b_ok = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=ok_was_pressed)
	
	
	b_send = 	ttk.Button(master,text="Send(Enter)",command=send_was_pressed)#,state='disabled'
	b_connect = ttk.Button(master,text="Connect"	,command=connect_was_pressed)
	b_clear	= 	ttk.Button(master,text="Clear(Esc)"	,command=clear_was_pressed)    
	
	b_clear.grid(	row=1,column=1, pady=5)	#, padx=5
	b_connect.grid(	row=2,column=0, pady=1)	#padx=1,
	
	titleLabel = ttk.Label(master,text="choose MSG:")
	titleLabel.grid(row=20,column=0)
	
	b_send.grid(row=12,column=0, pady=0, padx=5)#,columnspan = 2
	
	textbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 9)
	textbox.grid(row=2,column=1, rowspan=10, padx=5)# columnspan=10,, pady=15
	textbox.tag_configure('green'	,foreground='#01DF01')
	textbox.tag_configure('red'		,foreground='#DF0101')
	textbox.tag_configure('black'	,foreground='#000000')
	textbox.tag_configure('blue'	,foreground='#0000FF')
	textbox.tag_configure('orange'	,foreground='#DF7401')
	
	msgbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 1)
	msgbox.grid(row=12,column=1, rowspan=8, columnspan=10)#, padx=10, pady=5
	msgbox.focus_set()
	

	master.bind_all('<KeyRelease>', keyPress)
	master.mainloop()
###################################################################################################################
#######################    END Of User Interface     ##############################################################
###################################################################################################################
