#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Created on Tue June 13 

@author: Felix Bauer

Different subroutines that the visual interface goes through during the trials 
"""

def boom_stage(bci_val, cue, boom_type):
    """call ptr_update to draw pointer in boom, wait for given time"""
 
    if not go_on or not pg.display.get_init():
        return 0

    if cue == boom_type:
        evt = evtHit
        hit = 1
        print('hit')
    else:
        evt = evtMiss
        hit = 0
        print('miss')
    bci.sendTidEvent(evt)
    time = pg.time.get_ticks()
    while pg.time.get_ticks() - time < t_boom:
        if handle_events() == 'exit':
            bci.sendTidEvent(evt+evtOff)
            return hit
        ptr_update(bci_val, cue, boom_type)
    bci.sendTidEvent(evt+evtOff)
    return hit


def timeout_stage(bci_val, cue):
    """call ptr_update to draw pointer and show text 'time out', wait for given time"""
 
    if not go_on or not pg.display.get_init():
        return 0
    
    if cue == 'rest':
        evt = evtHit
        hit = 1
        print('hit')
    else:
        evt = evtMiss
        hit = 0
        print('miss')
    bci.sendTidEvent(evt)
    time = pg.time.get_ticks()
    ptr_update(bci_val, cue, 'time out')
    while pg.time.get_ticks() - time < t_boom:
        if handle_events() == 'exit':
            bci.sendTidEvent(evt+evtOff)
            return hit
    bci.sendTidEvent(evt+evtOff)
    return hit
        

def fixation_stage(task):
    """call ptr_update to show cross in center for given time, pointer is in neutral position"""

    if not go_on or not pg.display.get_init():
        return
    
    bci.sendTidEvent(evtFix)
    time = pg.time.get_ticks()
    ptr_update(0.5, 'fixation')
    while pg.time.get_ticks() - time < t_fixation:
        if handle_events() == 'exit':
            bci.sendTidEvent(evtFix+evtOff)
            return
    bci.sendTidEvent(evtFix+evtOff)
    return
            
    
def cue_stage(cue):
    """call ptr_update to show cue for given time, pointer is in neutral position"""

    if not go_on or not pg.display.get_init():
        return
    if cue == 'left':
        evtCue = evtLeft
    if cue == 'right':
        evtCue = evtRight
    if cue == 'rest':
        evtCue = evtRest
    bci.sendTidEvent(evtCue)
    time = pg.time.get_ticks()
    ptr_update(0.5, cue)
    while pg.time.get_ticks() - time < t_cue:
        if handle_events() == 'exit':
            bci.sendTidEvent(evtCue+evtOff)
            return
    bci.sendTidEvent(evtCue+evtOff)
    return
            
 
def feedback_stage(task, duration):
    """Show cue. Pointer indicates bci value. 
    Call boom_stage if right/left threshold reached, timeout if time out
    """
 
    if not go_on or not pg.display.get_init():
        return 0

    ptr_update(0.5, task)
    #Get current time for determining time out later on
    time = pg.time.get_ticks()
    if task == 'rest':
        IC_trial = False
    else:
        IC_trial = True
    #Consume older messages
    while bci.readTicAllProb() != None :
        pass
    bci.sendTidEvent(evtCFeed)
    #Repeat until timeout or forever if IC_trial
    while pg.time.get_ticks() - time < duration or IC_trial:
        x = get_bci_val()
        if x == 'exit' :
            bci.sendTidEvent(evtCFeed+evtOff)
            return 0
        ptr_update(x, task)
        #When threshold passed during left/right task - boom 
        if x >= r_thr:
            bci.sendTidEvent(evtCFeed+evtOff)
            print('right threshold reached')
            hit = boom_stage(x, task, 'right')
            return hit
        elif x <= l_thr:
            bci.sendTidEvent(evtCFeed+evtOff)
            print('left threshold reached')
            hit = boom_stage(x, task, 'left')
            return hit
    bci.sendTidEvent(evtCFeed+evtOff)
    print('time out')
    hit = timeout_stage(x, task)                    
    return hit