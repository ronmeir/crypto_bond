import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk
import socket               # Import socket module


def addOptionsToOptionList(l=[]):
    om_selected_var.set('')
    om['menu'].delete(0,'end')
    for choice in l:
        om['menu'].add_command(label=choice,command=tk._setit(om_selected_var,choice))
    om_selected_var.set(l[0])

def ok_was_pressed():
	#print("ok button was pressed\n")
	msg = om_selected_var.get()
	insert_text("ok - sending message: "+msg)
	send(msg)

    
def cancle_was_pressed():
   # print("cancle button was pressed\n")
    insert_text("cancle")


def insert_text(txt):
	textbox.insert(tk.INSERT, txt+"\n")
	print(txt)
	
def send(msg):
	HOST, PORT,size = '127.0.0.1', 12350, 1000
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	insert_text("		sending :"+msg)
	s.send(msg.encode())
	insert_text("		recieving:"+str(s.recv(size)))
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
    im_ok  = tk.PhotoImage(file='ok.gif')
    b_ok = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=ok_was_pressed)
    b_cancel = ttk.Button(master,text="Cancel",command=cancle_was_pressed)
    
    b_ok.grid(row=2,column=0, padx=5, pady=5)
    b_cancel.grid(row=2,column=1, padx=5, pady=5)



    textbox = tkst.ScrolledText(
        master = master,
        wrap = tk.WORD,
        width = 80,
        height = 10)
    textbox.grid(row=1,column=2, rowspan=10, columnspan=10, padx=15, pady=15)
    textbox.insert(tk.INSERT, "this is how\nwe insert test to the text field\n")
    master.mainloop()

