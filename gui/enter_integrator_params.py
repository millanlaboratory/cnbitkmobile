#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun  7 09:44:46 2017

@author: Felix Bauer

Open a GUI for changing the parameters of the integrators and saves them to 
/tmp/cnbitk-$USER/$DATE.smrinc_parameters.txt
"""

from Tkinter import *
import ttk
from datetime import datetime
import os

date = datetime.now().timetuple()
user = os.environ['USER'] 
image_path = ''
save_path = '/tmp/cnbitk-{}/'.format(user)
filename = '{}{}{}.smrinc_parameters.txt'.format(
        date.tm_year, str(date.tm_mon).zfill(2), str(date.tm_mday).zfill(2))

if not os.path.exists(save_path):
    os.makedirs(save_path)

def save(param, *args):
    """Save changed parameter to file, include timestamp"""
    file = open(save_path+filename, 'a')
    time = datetime.now().timetuple()
    file.write('[{}{}{}{}{}{}] type|integrator|task|{}|{}|{}\n'.format(
            time.tm_year, str(time.tm_mon).zfill(2), str(time.tm_mday).zfill(2),
            str(time.tm_hour).zfill(2), str(time.tm_min).zfill(2), str(time.tm_sec).zfill(2),
            parameters[param][3], param, parameters[param][2].get()))
    parameters[param][0]['foreground'] = 'black'
    parameters[param][1].state(['disabled'])
    file.close()    
    
def val_changed(param):
    """Behavior when parameter is changed"""
    parameters[param][0]['foreground'] = '#B6B6B4'
    parameters[param][1].state(['!disabled'])

#Main window
root = Tk()
root.title('Enter parameters')
try:
    icon = Image("photo", file="icon.gif")
    root.tk.call('wm','iconphoto',root._w,icon)
except TclError:
    pass


#Initialize variables
alpha=StringVar()
chi=StringVar()
phi=StringVar()
gamma=StringVar()
rho=StringVar()
alpha.set(0.97)
phi.set(0.5)
chi.set(1.0)
gamma.set(1.0)
rho.set(0.9)

mainframe = ttk.Frame(root, padding='3 3 12 12')
mainframe.grid(column = 0,  row=0, sticky =(N, W, E, S))
mainframe.columnconfigure(0, weight=1)
mainframe.rowconfigure(0, weight=1)

#Graphics
try:
    cnbi_logo_img = PhotoImage(file='{}cnbi_logo.gif'.format(image_path))
    cnbi_logo = ttk.Label(mainframe, image=cnbi_logo_img)
    cnbi_logo.grid(column=0, row=0, columnspan=4)
except TclError:
    pass

#EMA
ttk.Label(mainframe, text='Exponential Smoothing').grid(row=1, columnspan=4, sticky=(NW))
ttk.Label(mainframe, text='Alpha:').grid(column=0, row=2, sticky=W)
alpha_button = ttk.Button(mainframe, text='Save', command 
           = lambda : save('alpha'))
alpha_entry = Spinbox(mainframe, width=5, from_=0.0, to=1.0, increment=0.01, textvariable=alpha)
alpha_saved = ttk.Label(mainframe, text='saved', foreground='#B6B6B4')
alpha_entry.grid(column=1, row=2)
alpha_button.grid(column=2, row=2, sticky=E)
alpha_saved.grid(column=3, row=2)

#Dyn. model
ttk.Label(mainframe, text='Dynamical').grid(row=3, columnspan=4, sticky=(NW))
#Parameter phi
ttk.Label(mainframe, text='Phi:').grid(column=0, row=4, sticky=W)
phi_button = ttk.Button(mainframe, text='Save', command 
           = lambda : save('phi'))
phi_entry = Spinbox(mainframe, width=5, from_=0.0, to=1.0, increment=0.05, textvariable=phi)
phi_saved = ttk.Label(mainframe, text='saved', foreground='#B6B6B4')
phi_entry.grid(column=1, row=4)
phi_button.grid(column=2, row=4, sticky=E)
phi_saved.grid(column=3, row=4)
#Parameter chi
ttk.Label(mainframe, text='Chi:').grid(column=0, row=5, sticky=W)
chi_button = ttk.Button(mainframe, text='Save', command 
           = lambda : save('chi'))
chi_entry = Spinbox(mainframe, width=5, from_=0.0, to=5.0, increment=0.1, textvariable=chi)
chi_saved = ttk.Label(mainframe, text='saved', foreground='#B6B6B4')
chi_entry.grid(column=1, row=5)
chi_button.grid(column=2, row=5, sticky=E)
chi_saved.grid(column=3, row=5)

#Adaptive exptl. smoothing
ttk.Label(mainframe, text='AdES').grid(row=6, columnspan=4, sticky=(NW))
#Parameter gamma
ttk.Label(mainframe, text='Gamma:').grid(column=0, row=7, sticky=W)
gamma_button = ttk.Button(mainframe, text='Save', command 
           = lambda : save('gamma'))
gamma_entry = Spinbox(mainframe, width=5, from_=0.0, to=5.0, increment=0.1, textvariable=gamma)
gamma_saved = ttk.Label(mainframe, text='saved', foreground='#B6B6B4')
gamma_entry.grid(column=1, row=7)
gamma_button.grid(column=2, row=7, sticky=E)
gamma_saved.grid(column=3, row=7)
#Parameter rho
ttk.Label(mainframe, text='Rho:').grid(column=0, row=8, sticky=W)
rho_button = ttk.Button(mainframe, text='Save', command 
           = lambda : save('rho'))
rho_entry = Spinbox(mainframe, width=5, from_=0.0, to=1.0, increment=0.1, textvariable=rho)
rho_saved = ttk.Label(mainframe, text='saved', foreground='#B6B6B4')
rho_entry.grid(column=1, row=8)
rho_button.grid(column=2, row=8, sticky=E)
rho_saved.grid(column=3, row=8)

#Dict for accessing objects pertaining each parameter
parameters = {'alpha' : [alpha_saved, alpha_button, alpha, 'ema'], 
          'phi' : [phi_saved, phi_button, phi, 'dynamical'],
          'chi' : [chi_saved, chi_button, chi, 'dynamical'],
          'gamma' : [gamma_saved, gamma_button, gamma, 'vema'],
          'rho' : [rho_saved, rho_button, rho, 'vema']}

#Behavior when parameter is changed
alpha.trace('w', lambda n1,n2,op,parameter='alpha' : val_changed(parameter))
phi.trace('w', lambda n1,n2,op,parameter='phi' : val_changed(parameter))
chi.trace('w', lambda n1,n2,op,parameter='chi' : val_changed(parameter))
gamma.trace('w', lambda n1,n2,op,parameter='gamma' : val_changed(parameter))
rho.trace('w', lambda n1,n2,op,parameter='rho' : val_changed(parameter))

for child in mainframe.winfo_children():
    child.grid_configure(padx=5, pady=5)


root.mainloop()