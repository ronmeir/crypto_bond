import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk
import socket               # Import socket module

def getPort():
	return 12310
	
def startMSG():
		return "start_msg"
		
def getReadSize():
	return 1000		

def addOptionsToOptionList(l=[]):
    om_selected_var.set('')
    om['menu'].delete(0,'end')
    for choice in l:
        om['menu'].add_command(label=choice,command=tk._setit(om_selected_var,choice))
    om_selected_var.set(l[0])

def send_was_pressed():
	#print("ok button was pressed\n")
	msg = om_selected_var.get()
	insert_text("send - sending message: ")
	send(msg)
	
def clear_was_pressed():
	textbox.delete('1.0', 'end')

    
def connect_was_pressed():
   # print("connect button was pressed\n")
	insert_text("connecting...")
	send(startMSG())
    


def insert_text(txt):
	textbox.insert(tk.INSERT, txt+"\n")
	print(txt)
	
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
	master.geometry('800x300')
	master.title("The title") # the title of the window
	titleLabel = ttk.Label(master,text="Welcome")
	titleLabel.grid(row=0,column=0)
	
	om_selected_var = tk.StringVar()
	optionList = ['None']
	om = ttk.OptionMenu(master,om_selected_var,*optionList)
	om_selected_var.set(optionList[0])
	
	om.grid(row=1,column=0)
	addOptionsToOptionList(['MSG 1','MSG 2','MSG 3','Q'])
	#im_ok  = tk.PhotoImage(file='ok.gif')
	#b_ok = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=ok_was_pressed)
	b_send = ttk.Button(master,text="Send"	  ,command=send_was_pressed,state='disabled')
	b_connect = ttk.Button(master,text="Connect",command=connect_was_pressed)
	b_clear	= ttk.Button(master,text="clear"	  ,command=clear_was_pressed,state='disabled')    
	
	b_connect.grid(row=2,column=1, padx=1, pady=1)
	b_send.grid(row=10,column=0, padx=5, pady=5)
	b_clear.grid(row=2,column=0, padx=5, pady=5)

	
	
	textbox = tkst.ScrolledText(
		master = master,
		wrap = tk.WORD,
		width = 80,
		height = 10)
	textbox.grid(row=1,column=2, rowspan=10, columnspan=10, padx=15, pady=15)
	textbox.insert(tk.INSERT, "this is how\nwe insert test to the text field\n")
	master.mainloop()

