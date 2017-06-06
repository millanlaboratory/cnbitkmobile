#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 14:02:52 2017

@author: Felix Bauer

Feedback
"""
#Adapt lines 339 and 361 when info about rest task is present in xml file
#This version captures a video of everything that is displayed and saves it as gif file

import pygame as pg
import sys
import numpy as np
import imageio as im
import key_logger as kl
from lxml import etree as et

print('this is the correct skript')
#Settings
path = '../mi_ad4/ad4_mi.xml'
expmt = 'mi'
task = 'bhbf'

#Rotation that corresponds to BCI value being 0 or 1 (in degrees)
max_rotation = 90
# Initial window height and width and coordinates of center
s_width, s_height = 800, 800
#ratio of radius of inner and outer circle
rad_ratio = 0.8
#colors
black = [0,0,0]
blue = [20,40,255]
green = [0,150,0]
red = [200,0,0]
dark_grey = [90,90,90]


def get_bci_val():
    """ determine the current (processed) bci raw probability from TiC"""

    if not pg.display.get_init():
        return    
 
    while True:
        for event in pg.event.get():
            #Exit program when closing window or hitting escape
            if (event.type  == pg.QUIT 
                    or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                pg.quit()
                return 0
            #Change window size
            elif event.type == pg.VIDEORESIZE:
                w, h = event.size
                make_window(w, h, 0.5, None)
            #Maximize window when pressing f
            elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                fullscreen(0.5, None)
        else:
            #Obtain probability from TiC
            prob = bci.readTicClassProb(expmt, task_event)
            if prob != None:
                prob = 0.35*prob+0.5*(1-0.35)
                #Correct if probability for left task is being received
                if bci_inverse:
                    prob = 1-prob
                return prob

def get_bci_class():
    """determine task that is being sent via TiC"""

    print('Obtaining BCI class')
    while True:
        #Obtain probability from TiC
        out = bci.readTicAllProb()
        if out != None:
            print('Successfull')
            return out[0]


def centered_rect(image, s_center_x, s_center_y):
    """return a rect object of the given image, with center in s_center_x, s_center_y"""
    if not pg.display.get_init():
        return
    width, height = image.get_rect()[2], image.get_rect()[3]
    return [s_center_x - width/2, s_center_y - height/2, width, height]


def ptr_update(bci_val, cue, boom=None):
    """draw pointer, rotated according to current bci value, cue in center"""
    
    if not pg.display.get_init():
        return
    
    #Determine size of window and coordinates of center
    screen = pg.display.get_surface()
    s_center_x = screen.get_size()[0]/2
    s_center_y = screen.get_size()[1]/2
    
    screen.fill(black)
                                
    #Radii to draw lines and some of the circles
    outer_radius = pointer_grey.get_rect()[2]/2
    inner_radius = outer_radius*rad_ratio
    
    #Outer dark grey circle
    pg.draw.circle(screen, dark_grey, 
                       [int(s_center_x),int(s_center_y)], int(outer_radius), 0)

    #Rotate pointer according to bci value and change color if boom
    rotation = - (2*bci_val - 1) * max_rotation
    if boom == None or boom == 'time out':
        new_pointer = pg.transform.rotate(pointer_grey, rotation)
    elif boom == 'left':
        new_pointer = pg.transform.rotate(pointer_red, rotation)
    elif boom == 'right':
        new_pointer = pg.transform.rotate(pointer_blue, rotation)        
    #Determine rect (position and size) of pointer and draw it
    new_rect = centered_rect(new_pointer, s_center_x, s_center_y)
    screen.blit(new_pointer, new_rect)
       
    #Green rest line
    pg.draw.line(screen, green, [s_center_x, s_center_y - inner_radius], [s_center_x, s_center_y - outer_radius], 3)
    
    #Red class 0-line
    angle0 = -(2*l_thr - 1) * max_rotation
    x0_out = -outer_radius*np.sin(np.pi*angle0/180.)
    y0_out = -outer_radius*np.cos(np.pi*angle0/180.)
    x0_in = rad_ratio*x0_out
    y0_in = rad_ratio*y0_out
    pg.draw.line(screen, red, [s_center_x+x0_in,s_center_y+y0_in], [s_center_x+x0_out,s_center_y+y0_out], 3)
    
    #Blue class 1-line
    angle1 = (2*r_thr - 1) * max_rotation
    x1_out = outer_radius*np.sin(np.pi*angle1/180.)
    y1_out = -outer_radius*np.cos(np.pi*angle1/180.)
    x1_in = rad_ratio*x1_out
    y1_in = rad_ratio*y1_out
    pg.draw.line(screen, blue, [s_center_x+x1_in,s_center_y+y1_in], [s_center_x+x1_out,s_center_y+y1_out], 3)
    
    #Inner black circle
    pg.draw.circle(screen, black, [int(s_center_x),int(s_center_y)], int(inner_radius), 0)
    
    #Outer black ring to reduce flickering
    screen.blit(ring, centered_rect(ring, s_center_x, s_center_y))
    
    #Cue
    if cue != None:
        screen.blit(cue_dict[cue], centered_rect(cue_dict[cue], s_center_x, s_center_y))

    #After time out show text
    if boom == 'time out':
        text_rect = centered_rect(text_timeout, s_center_x, s_center_y+250)
	screen.blit(text_timeout, text_rect)
    
    pg.display.update()
    pg.image.save(screen, './video/img.png')#.format(len(images)))
    images.append(im.imread('./video/img.png')) #.format(len(images))))

    return None


def make_window(width, height, bci_val, cue, boom=None):
    """Create a window with given size or resize if already exists, 
    draw pointer according to given data
    """
    pg.display.set_mode((width, height), pg.RESIZABLE)
    pg.display.set_caption('Feedback')
    ptr_update(bci_val, cue, boom)


def fullscreen(bci_val, cue, boom=None):
    """Maximize window if not maximized yet. If already max.ed then make smaller"""
    global maximized_window
    pg.display.quit()
    pg.display.init()
    if maximized_window:
        make_window(s_width, s_height, bci_val, cue, boom)
    else:
        make_window(0, 0, bci_val, cue, boom)
    maximized_window = not maximized_window


def boom(bci_val, cue, boom_type):
    """Draw pointer in boom, wait for given time"""
 
    if not pg.display.get_init():
        return
 
    time = pg.time.get_ticks()
    while pg.time.get_ticks() - time < t_boom:
        for event in pg.event.get():
            #Exit program when closing window or hitting escape
            if (event.type == pg.QUIT 
                    or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                im.mimsave('./video/vid.gif', images)
                pg.quit()
                return
            #Change window size
            elif event.type == pg.VIDEORESIZE:
                w, h = event.size
                make_window(w, h, bci_val, cue, boom_type)
            #Maximize window when pressing f
            elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                fullscreen(bci_val, cue, boom_type)
        else:
            ptr_update(bci_val, cue, boom_type)


def timeout(bci_val, cue):
    """Draw pointer, show text 'time out', wait for given time"""
 
    if not pg.display.get_init():
        return
 
    time = pg.time.get_ticks()
    while pg.time.get_ticks() - time < t_boom:
        for event in pg.event.get():
            #Exit program when closing window or hitting escape
            if (event.type == pg.QUIT 
                    or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                im.mimsave('./video/vid.gif', images)
                pg.quit()
                return
            #Change window size
            elif event.type == pg.VIDEORESIZE:
                w, h = event.size
                make_window(w, h, bci_val, cue, boom_type)
            #Maximize window when pressing f
            elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                fullscreen(bci_val, cue, boom_type)
        else:
            ptr_update(bci_val, cue, 'time out')


def fixation_phase(task):
    """show cross in center for given time, pointer is in neutral position"""

    if not pg.display.get_init():
        return

    time = pg.time.get_ticks()
    while pg.time.get_ticks() - time < t_fixation:
        for event in pg.event.get():
            #Exit program when closing window or hitting escape
            if (event.type  == pg.QUIT 
                    or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                im.mimsave('./video/vid.gif', images)
                pg.quit()
                return 
            #Change window size
            elif event.type == pg.VIDEORESIZE:
                w, h = event.size
                make_window(w, h, 0.5, 'fixation')
            #Maximize window when pressing f
            elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                fullscreen(0.5, 'fixation')
        else:
            ptr_update(0.5, 'fixation')
    

def cue_phase(task):
    """show cue for given time, pointer is in neutral position"""

    if not pg.display.get_init():
        return

    time = pg.time.get_ticks()
    while pg.time.get_ticks() - time < t_cue:
        for event in pg.event.get():
            #Exit program when closing window or hitting escape
            if (event.type  == pg.QUIT 
                    or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                im.mimsave('./video/vid.gif', images)
                pg.quit()
                return 
            #Change window size 
            elif event.type == pg.VIDEORESIZE:
                w, h = event.size
                make_window(w, h, 0.5, task)
            #Maximize window when pressing f
            elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                fullscreen(0.5, task)
        else:
            ptr_update(0.5, task)
 
   
    
def task_phase(task, duration):
    """Show cue. Pointer indicates bci value. 
    Return +/-1 if right/left threshold reached, 0 if time out
    """
 
    if not pg.display.get_init():
        return

    ptr_update(0.5, task)
    #Time for time out
    time = pg.time.get_ticks()
    #Timer for getting bci value every 20ms (will be replaced)
    pg.time.set_timer(pg.USEREVENT, 20)
    while pg.time.get_ticks() - time < duration:
        for event in pg.event.get():
            #Exit program when closing window or hitting escape
            if (event.type  == pg.QUIT 
            or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                im.mimsave('./video/vid.gif', images)
                pg.quit()
                return 'EXIT'
            #Change window size
            elif event.type == pg.VIDEORESIZE:
                w, h = event.size
                make_window(w, h, get_bci_val(), task)
            #Maximize window when pressing f
            elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                fullscreen(get_bci_val(), task)
            #Update pointer based on event (at the moment: every 20ms)
            elif event.type == pg.USEREVENT:
                x = get_bci_val()
                ptr_update(x, task)
                #When threshold passed during left/right task - boom 
                if x >= r_thr:
                    boom(x, task, 'right')
                    pg.time.set_timer(pg.USEREVENT, 0) 
                    return 'right threshold reached'
                elif x <= l_thr:
                    boom(x, task, 'left')
                    pg.time.set_timer(pg.USEREVENT, 0) 
                    return 'left threshold reached'
    pg.time.set_timer(pg.USEREVENT, 0)
    timeout(x, task)                    
    ptr_update(0.5, None)
    return 'TIME OUT'


def read_xml():    
    xml = et.parse(path).getroot()
    
    #durations
    global t_fixation, t_cue, t_timeout, t_boom
    times = xml.find('./protocol/{}/trial'.format(expmt))
    t_fixation = float(times.findtext('fixation'))
    t_cue = float(times.findtext('cue'))
    t_timeout = float(times.findtext('timeout'))
    t_boom = float(times.findtext('boom'))
    
    #Tasks
    taskset = xml.find('./tasksets/{}_{}'.format(expmt,task))
    right_task = taskset.find('./class[@id="0"]').attrib['key']
    left_task = taskset.find('./class[@id="1"]').attrib['key']
    #rest_task = taskset.find('./class[@id="2"]').attrib['key']
    
    #identify task that is being sent
    global task_event, bci_inverse
    task_event = get_bci_class()
    tasks_element = xml.find('./tasks')
    event_element = tasks_element.xpath('.//gdfevent[text()="{:#06x}"]'.format(int(task_event)))[0]
    bci_task = event_element.getparent().tag
    #if task corresponds to left, bci_get_val will correct for that
    if bci_task == right_task:
        bci_inverse = False
    elif bci_task == left_task:
        bci_inverse = True
    else:
        raise Exception('BCI probability does not correspond to given taskset')
    
    #identify reset event
    global reset_event
    reset_event = int(xml.findtext('./events/gdfevents/cfeedback'), 16)

    #numbers of trials
    global n_right, n_left, n_rest
    taskset_online = xml.find(
        './online/{0}/taskset[@ttype="{0}_{1}"]'.format(expmt, task))
    n_right = int(taskset_online.findtext('./trials/{}'.format(right_task)))
    n_left = int(taskset_online.findtext('./trials/{}'.format(left_task)))
    n_rest = 15#int(taskset_offline.findtext('./trials/{}'.format(rest_task)))
    #List to indicate order of trials
    global tasks, n_tasks
    tasks = n_rest*['rest'] + n_right*['right'] + n_left*['left']
    n_tasks = len(tasks)
    np.random.shuffle(tasks)
    
    #thresholds
    global r_thr, l_thr
    r_thr = float(taskset_online.findtext('./threshold/{}'.format(right_task)))
    l_thr = 1-float(taskset_online.findtext('./threshold/{}'.format(left_task)))

    print('Settings loaded')


def initialize_program():
    """Initialize keylogger object, list with trials and pygame, open window"""
    pg.init()
    #Ignore mouse motion and clicks
    pg.event.set_blocked((pg.MOUSEBUTTONUP, pg.MOUSEBUTTONDOWN, pg.MOUSEMOTION))
    print('Pygame initialized')
    global images
    images = []


    #Load images
    global pointer_grey, pointer_red, pointer_blue, ring, text_timeout
    #pointer in rest-position
    pointer_grey = pg.image.load('ptr.gif')
    pointer_red = pg.image.load('ptr_red.gif')
    pointer_blue = pg.image.load('ptr_blue.gif')
    #black ring around pointer
    ring = pg.image.load('blk_ring.gif')
    #Time out text
    text_timeout = pg.image.load('timeout.gif')

    #store graphics for cues in dict
    global cue_dict
    cue_dict = {}
    cue_dict['left'] = pg.image.load('l_arrow.gif')
    cue_dict['right'] = pg.image.load('r_arrow.gif')
    cue_dict['rest'] = pg.image.load('circle.gif')
    cue_dict['fixation'] = pg.image.load('cross.gif')
    
    print('Graphics loaded')

    #Open the window
    global maximized_window
    maximized_window = False
    make_window(s_width, s_height, 0.5, None)


def main():
    
    #Initialize KeyLogger object to communicate with BCI
    global app, bci
    app = kl.QtGui.QApplication(kl.sys.argv)
    bci = kl.KeyLogger()
    
    read_xml()
    initialize_program()
    
    #Main loop
    while True:
        count_tasks = 0
        for tsk in tasks:
            count_tasks += 1
            if not pg.display.get_init():
                return
            for event in pg.event.get():
                #Exit program when closing window or hitting escape
                if (event.type  == pg.QUIT 
                or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE)):
                    im.mimsave('./video/vid.gif', images)
                    pg.quit()
                    return
                #Change window size
                elif event.type == pg.VIDEORESIZE:
                    w, h = event.size
                    make_window(w, h, 0.5, None)
                #Maximize window when pressing f
                elif event.type == pg.KEYDOWN and event.key == pg.K_f:
                    fullscreen(0.5, None)
            #Run the actual feedback program
            else:
                ptr_update(0.5, None)
                fixation_phase(tsk)
                print('\nTask {} of {}: {}'.format(count_tasks, n_tasks, tsk))
                cue_phase(tsk)
                result = task_phase(tsk, t_timeout)
                bci.sendTidEvent(reset_event)
                print(result)


#call main function
if __name__ == '__main__': main()