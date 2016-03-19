from gui import MyApp
import os.path
#import Tkinter as tk
import tkMessageBox

if __name__ == "__main__":
	makeNewConfig=True
	if os.path.isfile("config.cfg"):
		makeNewConfig=tkMessageBox.askyesno("Config Found", "A configuration file (config.cfg) was already found. Do you want to overwrite it?")
		if makeNewConfig==True:
			MyApp().mainloop()