#---  Senior Project - Fall 2023  ---
#---  Autonomous Delivery Vehicle  ---
#---  By: Laith Fakhruldin and Spencer Martinez  ---
#---  Dr. Ece Yaprak and prof. Moise Sunda  ---
#---  ET4999  ---
#---  10/28/2023  ---
# This program is designed for controlling miniature autonomous vehicle in a warehouse environment
# using a Bluetooth connection to an Arduino device. It offers various commands to control
# the vehicle's movements, pick-up, and drop-off actions. The GUI interface allows users
# to select pick-up and drop-off stations, establish a Bluetooth connection, send commands,
# and monitor the vehicle's status. Additionally, it provides options for resetting, stopping,
# continuing, and quitting the application.

# Required libraries for this application include tkinter for GUI, serial for serial
# communication with the Arduino, os for system-related operations, time for timing delays,
# and tkinter.messagebox for displaying messages.

#these libraries are required to complete this application and connect with Arduino program
import serial
import tkinter as tk 
import os
import time
from tkinter import simpledialog
import tkinter.messagebox as messagebox
import threading

connected = False # bool to check the connection status
ser = None  # Initialize serial port as None
sending_color = None # initialize command character to be sent to Arduino
com_port = None # initialize COM port to be as COM1,COM2, etc.

stop_vehicle = 's' #command value to stop vehicle
reset_val = 'g' #command value to reset vehicle 
sim_val = 'i'   # simulation command
cnt_val = 'n'   #continue cycle command

# method for the Bluetooth Connection
def is_bluetooth_connected():
    return ser is not None

#This method to retrive data from the Arduino side while the vehicle is running.
def retrieveData():
    global ser
    try:
        if ser is not None and ser.is_open:
            #retrieve the data from the arduino side as readline from serial port "ser"
            data = ser.readline().decode('utf-8')
            if data:
                data_label.config(text=f"Response: {data}")
    except serial.SerialException as e:
         data_label.config(text=f"Serial Error: {str(e)}")  # Return the error message
   

#using thread here for data retreival, so there are no conflicts will occure while running windows.loop() of tkinter
def data_retrieve_thread():
    while True:
        retrieveData()
        

data_thread = threading.Thread(target=data_retrieve_thread)
data_thread.daemon = True #Daemonize the thread to exit when main loop of windows exits
data_thread.start()
        

def get_com_port():
    
    # Small popup window to insert the COM port into text field
    com_window = tk.Toplevel(window)
    com_window.geometry('{}x{}'.format(260, 150))
    com_window.title("COM - PORT")
        
    com_port_label = tk.Label(com_window, text="Enter COM Port:", bg='#89A3B6')
    com_port_label.grid(row=4, column=1, padx=10, sticky='w')
    com_port_entry = tk.Entry(com_window, bg="white")
    com_port_entry.grid(row=4, column=2, padx=10, sticky='w')
    
    # Definition to add the connection of serial port to the Bluetooth in Arduino side
    def ok_button_clicked():
        #using some global variables for connection    
        global ser, connected, com_port 
        
        if connected:
            btooth_label.config(text="Bluetooth is already connected.")
        else:    
          com_port = com_port_entry.get()
          connected = True
        
        if not com_port:
            #popup  error message window if you didn't add the COM port
            messagebox.showerror("Error", "Please enter a COM port.")
            connected = False
        else:
            try:
                #This is the connection to the serial connection with Bluetooth on the Arduino side
                ser = serial.Serial(com_port, 9600, timeout=1)
                btooth_label.config(text="Bluetooth Connected")
                com_window.destroy()  # Close the COM port input window
            except serial.SerialException as e:
                btooth_label.config(text=f"Bluetooth Connection Error: {str(e)}")
    
    ok_button = tk.Button(com_window, text="OK", command=ok_button_clicked)
    ok_button.grid(row=2, column=2, sticky='nsew')



# Function to disconnect from Bluetooth
def disconnect_bluetooth():
    global ser, connected
    if connected:
        ser.close()
        btooth_label.config(text="Bluetooth Disconnected")
        connected = False
    else:
        messagebox.showinfo("Info", "Bluetooth is not connected.")


#defention to send the multi colored option to Arduino
def send_command(sending_color):
    global ser
    if ser == None:
        messagebox.showerror("Error", "Bluetooth is not connected.")
        return
    else:
        try:
            command = f"{sending_color}"
            ser.write(command.encode('utf-8'))
            time.sleep(0.1)  # give a delay time to communicate with Arduino
        except serial.SerialException as e:
            data_label.config(text=f"Serial Error: {str(e)}")

def sim_command(sending_color):
    global ser
    if ser==None:
        messagebox.showerror("Error", "Bluetooth is not connected.")
        return
    else:
      try:
        command = f"{sending_color}"
        ser.write(command.encode('utf-8'))
        time.sleep(0.1)  # give a delay time to communicate with Arduino
        #data = retrieveData()
       # data_label.config(text=f"Response: {data}")
      except serial.SerialException as e:
        data_label.config(text=f"Serial Error: {str(e)}")
 
# this will stop the vehicle at any moment in any case.
def stop_command(sending_color):
    global ser
    if ser==None:
        messagebox.showerror("Error", "Bluetooth is not connected.")
        return
    else:
        command = f"{sending_color}"
        ser.write(command.encode())
        #time.sleep(0.1) 
        data = retrieveData()
        data_label.config(text=f"Response: {data}")
        if ser:
            try:
                command = f"{sending_color}"
                ser.write(command.encode())
                time.sleep(0.1)  # give a delay time to communicate with Arduino
                #data = retrieveData()
                #data_label.config(text=f"Response: {data}")
            except serial.SerialException as e:
                data_label.config(text=f"Serial Error: {str(e)}")
        #else:
        #    data_label.config(text="Serial port is not initialized.")

# this will make continue the cycle where it was left after clicking stop command
def continue_cycle(sending_color):
    global ser
    if ser==None:
        messagebox.showerror("Error", "Bluetooth is not connected.")
        return
    else:
        command = f"{sending_color}"
        ser.write(command.encode())
        #time.sleep(0.1) 
        data = retrieveData()
        data_label.config(text=f"Response: {data}")
        if ser:
            try:
                command = f"{sending_color}"
                #this is important piece of code when trying to send command to the other side or Arduino is ser.write(....)
                ser.write(command.encode())
                time.sleep(0.1) # give a delay time to communicate with Arduino
                #data = retrieveData()
                #data_label.config(text=f"Response: {data}")
            except serial.SerialException as e:
                data_label.config(text=f"Serial Error: {str(e)}")
        #else:
        #    data_label.config(text="Serial port is not initialized.")

# reset the values to default
def reset_command(sending_color):
    global ser
    if ser==None:
        messagebox.showerror("Error", "Bluetooth is not connected.")
        return
    else:
        data_label.config(text=f"")
        command = f"{sending_color}"
        ser.write(command.encode())
        #time.sleep(0.1) 
        data = retrieveData()
        data_label.config(text=f"Response: {data}")
        if ser:
            try:
                command = f"{sending_color}"
                ser.write(command.encode())
                time.sleep(0.1)  # give a delay time to communicate with Arduino
                #data = retrieveData()
                #data_label.config(text=f"Response: {data}")
            except serial.SerialException as e:
                data_label.config(text=f"Serial Error: {str(e)}")
        #else:
        #    data_label.config(text="Serial port is not initialized.")

#this function will close the application exit from the program
def quit_application():
    global ser
    if ser is not None:
        if ser.is_open:
            ser.close()
    os._exit(0)


#resize the window scale
def on_resize(event):
    new_width = event.width
    top_frame.config(width=new_width)
    
#this function will help in determining what command to send when choosing different color in radio buttons. 
def get_pick_drop():
    #getting the values of each radio button
    pick_color = pickup_color_var.get()
    drop_color = dropoff_color_var.get()
    
    #this is just to check values of the radio buttons in console 
    print("Pickup Color:", pick_color)
    print("Drop-off Color:", drop_color)
    
    # Initialize sending_color with a default value
    if pick_color == 'o' and drop_color == 'r':
        sending_color = 'o'
    elif pick_color == 'o' and drop_color == 'l':
        sending_color = 'b'
    elif pick_color == 'o' and drop_color == 'u':
        sending_color = 'p'
    elif pick_color == 'b' and drop_color == 'r':
        sending_color = 'l'
    elif pick_color == 'b' and drop_color == 'l':
        sending_color = 'e'
    elif pick_color == 'b' and drop_color == 'u':
        sending_color = 'm'
    elif pick_color == 'p' and drop_color == 'r':
        sending_color = 'c'
    elif pick_color == 'p' and drop_color == 'l':
        sending_color = 'd'
    elif pick_color == 'p' and drop_color == 'u':
        sending_color = 'a'
    print("SendingColor: ", sending_color)
    return sending_color

""" --- This part of the code will handle the window and any GUI commands in this program --- """
# Create a tkinter window
window = tk.Tk()
window.title("Senior Project")
window.geometry('{}x{}'.format(940, 600))
window.resizable(True, True)

#start to retrieve data from Arduino side
retrieveData()

top_frame = tk.Frame(window, bg='#29465B', height=55, pady=5, padx=10)
top_frame.grid(row=0, columnspan=20, sticky='nesw')

copyright = tk.Label(top_frame, bg='#2E8B57', foreground='white', text='--- All Rights Reserved - Copyright 2023 - Autonomous Delivery Vehicle --- ', font='Helvetica 12', height=2, width=80)
copyright.grid(row=0, column=1, columnspan=20,  sticky='n')
window.grid_columnconfigure(0, weight=1)
top_frame.grid_columnconfigure(1,weight=1)

# adding control buttons in this frame
control_frame = tk.Frame(window, bg='#89A3B6', width=600, height=100, pady=5, padx=100)
control_frame.grid(row=1, column=0, columnspan=20, rowspan=20, sticky='nesw')

# changing the grid rows and columns for row 1 that is control_frame 
window.grid_rowconfigure(1, weight=1)
window.grid_columnconfigure(1, weight=1)
control_frame.grid_columnconfigure(1,weight=0)
control_frame.grid_rowconfigure(1,weight=0)


#setting up the variables that will be taken from radio buttons.
pickup_color_var = tk.StringVar()
dropoff_color_var = tk.StringVar()
pickup_color_var.set(None)
dropoff_color_var.set(None)

#spacer to add between rows and columns

spacer2 = tk.Label(control_frame, text="",bg='#89A3B6').grid(row=6, column=0, columnspan=5, rowspan=50)
spacer3 = tk.Label(control_frame, text="",bg='#89A3B6').grid(row=6, column=0, columnspan=5, rowspan=50)
spacer4 = tk.Label(control_frame, text="",bg='#89A3B6').grid(row=6, column=0, columnspan=5, rowspan=50)
spacer5 = tk.Label(control_frame, text="",bg='#89A3B6').grid(row=6, column=0, columnspan=5, rowspan=50)
spacer6 = tk.Label(control_frame, text="",bg='#89A3B6').grid(row=6, column=0, columnspan=5, rowspan=50)


# Color selection buttons for pickup and drop-off
color_label_pickup = tk.Label(control_frame, text="Pickup Station:", bg='#89A3B6')
color_label_pickup.grid(row=2, column=0, sticky='w', padx=10)

color_label_dropoff = tk.Label(control_frame, text="Drop-off Station:", bg='#89A3B6')
color_label_dropoff.grid(row=3,column= 0, sticky='w',padx=10)

#these are radio buttons are organized in three set for each pickup or drop off station
orange_pick = tk.Radiobutton(control_frame, text="Orange", variable=pickup_color_var, bg="orange",value="o", highlightbackground="green").grid(row=2, column=1, sticky='w')
blue_pick = tk.Radiobutton(control_frame, text="Blue", variable=pickup_color_var, bg="lightblue",value="b", highlightbackground="green").grid(row=2, column=2, sticky='w')
purple_pick = tk.Radiobutton(control_frame, text="Purple", variable=pickup_color_var, bg="#D8A7D8",value="p", highlightbackground="green").grid(row=2, column=3, sticky='w')

orange_drop = tk.Radiobutton(control_frame, text="Orange", variable=dropoff_color_var, bg="orange",value="r", highlightbackground="green").grid(row=3, column=1, sticky='w')
blue_drop = tk.Radiobutton(control_frame, text="Blue", variable=dropoff_color_var, bg="lightblue",value="l", highlightbackground="green").grid(row=3, column=2, sticky='w')
pruple_drop = tk.Radiobutton(control_frame, text="Purple", variable=dropoff_color_var, bg="#D8A7D8",value="u", highlightbackground="green").grid(row=3, column=3, sticky='w')

# Create a "Connect" button
connect_button = tk.Button(control_frame, text="Connect Bluetooth", width=17,command=get_com_port, bg='#29465B', foreground='white')
connect_button.grid(row=2, column=4, padx=10, sticky='w')

# Create a "Disconnect" button
disconnect_button = tk.Button(control_frame, text="Disconnect Bluetooth",width=17, command=disconnect_bluetooth, bg='#29465B', foreground='white')
disconnect_button.grid(row=3, column=4, padx=10,sticky='sw')

#This button for the multicolor command
button_send = tk.Button(control_frame, text="Send Command", command=lambda: send_command(get_pick_drop()),width=14, bg='#29465B',foreground='white')
button_send.grid(row=4, column=0, padx=10,pady=12,sticky='w')

#Simulation button will activate three cycle command in Arduino
button_sim = tk.Button(control_frame, text="Simulation Cycles", command=lambda: sim_command(sim_val),width=14, bg='#29465B',foreground='white')
button_sim.grid(row=4, column=1, padx=10,pady=12,sticky='w')

# Label for Bluetooth
btooth_status = tk.Label(control_frame, text="Bluetooth Status: ", bg='#89A3B6')
btooth_status.grid(row=4, column=4, columnspan=4, padx=5, sticky='w')

#This label to show the status of the Bluetooth it is connected 
btooth_label = tk.Label(control_frame, text="", bg='#89A3B6')
btooth_label.grid(row=4, column=5, columnspan=4, padx=5, sticky='e')

# Adding a label for displaying the response
data_status = tk.Label(control_frame, text="Data Status: ", bg='#89A3B6')
data_status.grid(row=5, column=0, columnspan=1, padx=5,pady=10, sticky='nw')

#this is a data label will be used to receive any data from Arduino
data_label = tk.Label(control_frame, text="", bg='#89A3B6')
data_label.grid(row=5, column=1, columnspan=4, padx=5,pady=10, sticky='nw')

#Reset the vehicle
reset_button = tk.Button(control_frame, text="Reset Command", command=lambda: reset_command(reset_val),width=14, bg='#29465B',foreground='white')
reset_button.grid(row=6, column=0, padx=10,sticky='w')

#Stop the vehicle at any point for emergncy or other things.
stop_vcle = tk.Button(control_frame, text="Stop Command", command=lambda: stop_command(stop_vehicle),width=14, bg='#29465B',foreground='white')
stop_vcle.grid(row=6, column=1, padx=10,sticky='w')

#Continue the cycle button
run_vcle = tk.Button(control_frame, text="Continue Cycle", command=lambda: continue_cycle(cnt_val), width=14,bg='#29465B',foreground='white')
run_vcle.grid(row=6, column=2, padx=10,sticky='w')

#Quit from the program button
quit_button = tk.Button(control_frame, text="Quit", command=quit_application, bg='#29465B', foreground='white',width=14, anchor='center')
quit_button.grid(row=6, column=3, padx=20,sticky='w')

#this used to bind the window resized
window.bind('<Configure>', on_resize)


#tkinter loop
window.mainloop()

