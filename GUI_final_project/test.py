import tkinter as tk
import tkinter.scrolledtext as tkst
import ttk


def addOptionsToOptionList(l=[]):
    om_selected_var.set('')
    om['menu'].delete(0,'end')
    for choice in l:
        om['menu'].add_command(label=choice,command=tk._setit(om_selected_var,choice))
    om_selected_var.set(l[0])

def fff():
    print("fff")

if __name__ == "__main__":
    global var1, optionList, om_selected_var, om
    master = tk.Tk()
    master.geometry('800x300')
    master.title("The title") # Foo
    titleLabel = ttk.Label(master,text="Welcome")
    titleLabel.grid(row=0,column=0)
    
    om_selected_var = tk.StringVar()
    optionList = ['None']
    om = ttk.OptionMenu(master,om_selected_var,*optionList)
    om_selected_var.set(optionList[0])

    om.grid(row=1,column=0)
    addOptionsToOptionList(['a','dlf','asdfasdf'])
    im_ok  = tk.PhotoImage(file='ok.gif')
    b1 = ttk.Button(master, compound=tk.LEFT, image=im_ok, text="OK",command=fff)
    b2 = ttk.Button(master,text="Cancel",command=fff)
    
    b1.grid(row=2,column=0, padx=5, pady=5)
    b2.grid(row=2,column=1, padx=5, pady=5)



    textbox = tkst.ScrolledText(
        master = master,
        wrap = tk.WORD,
        width = 80,
        height = 10)
    textbox.grid(row=1,column=2, rowspan=10, columnspan=10, padx=15, pady=15)
    textbox.insert(tk.INSERT, "Floooooordsflskdjf\nbtgb")
    master.mainloop()

