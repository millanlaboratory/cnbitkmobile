#!/usr/bin/python
# -*- coding: utf-8 -*-

import ConfigParser
import sys
from PyQt4 import QtGui, QtCore
import BCI
import time
import pylpttrigger
#from websocket import create_connection
#import math
#import xml.etree.ElementTree as ET
from select import *


class KeyLogger(QtGui.QWidget):
    
    def __init__(self, iploop = '127.0.0.1'):
        super(KeyLogger, self).__init__()
        self.iploop = iploop
        self.initUI()
        
    def initUI(self):      	

        self.bci = BCI.BciInterface(self.iploop)
            #try:
             #   self.sendTidEvent(781)
               #print('sent')
             #   while True: 
              #      print('test')
               #     self.readTicClassProb("mi","773")
            #except KeyboardInterrupt:
             #   self.closing()

    def closing(self):
        exit()

    def sendTidEvent(self,event):
        self.bci.id_msg_bus.SetEvent(event);
        self.bci.id_serializer_bus.SetMessage(self.bci.id_msg_bus)
        idsmsg = self.bci.id_serializer_bus.Serialize()
        self.bci.iDsock_bus.send(idsmsg)

    def readTidEvent(self):
        data = None
        try:
            data = self.bci.iDsock_bus.recv(512)
            self.bci.idStreamer_bus.Append(data)     
        except:
            self.nS = False
            self.dec = 0
            pass
        # deserialize ID message
        if data:
            if (self.bci.idStreamer_bus.Has("<tobiid","/>")):
                msg = self.bci.idStreamer_bus.Extract("<tobiid","/>")
                self.bci.id_serializer_bus.Deserialize(msg)
                self.bci.idStreamer_bus.Clear()
                self.MyIDMessage = int(self.bci.id_msg_bus.GetEvent())  # Here is the value of the received message!
                print self.MyIDMessage

            elif (self.bci.idStreamer_bus.Has("<tcstatus","/>")):
                MsgNum = self.bci.idStreamer_bus.Count("<tcstatus")
                for i in range(1,MsgNum-1):
                # Extract most of these messages and trash them
                    msg_useless = self.bci.idStreamer_bus.Extract("<tcstatus","/>")

    def readTicAllProb(self):
        ready = select([self.bci.conn,],[], [],0.0)
        data = None
        if ready[0]:
            try:
                data = self.bci.conn.recv(512)
                self.bci.icStreamer.Append(data)
            except:
                print "I am passing! Socket empty!"
                pass # no data available

            # deserialize IC message
            if data:
                try:
                    while self.bci.icStreamer.Has("<tobiic","</tobiic>"):
                        msg = self.bci.icStreamer.Extract("<tobiic","</tobiic>")
                        self.bci.ic_serializer.Deserialize(msg)
                        #bci.icStreamer.Dump() # Check what's left, see how much I loose            
                        self.bci.icStreamer.Clear()
                except:
                    print self.bci.ic_serializer.Deserialize(data)
                    print '[TOBI-IC] Warning, received data not in iC format!'
                    return

                for classifier_name, classifier_data in self.bci.ic_msg.classifiers.map.items():
                    #print '[TOBI-IC] Received data from classifier "%s"' %(classifier_name)
                    for class_name, class_data in classifier_data.classes.map.items():
                        val = class_data.GetValue()
                        #print '[TOBI-IC] Received class "%s" = %.3f' %(class_name, val)
                        return class_name, val

    def readTicClassProb(self, classifier_name, class_name):
        ready = select([self.bci.conn,],[], [],0.0)
        data = None
        if ready[0]:
            try:
                data = self.bci.conn.recv(512)
                self.bci.icStreamer.Append(data)
            except:
                print "I am passing! Socket empty!"
                pass # no data available
            # deserialize IC message
            if data:
                try:
                    while self.bci.icStreamer.Has("<tobiic","</tobiic>"):
                        msg = self.bci.icStreamer.Extract("<tobiic","</tobiic>")
                        self.bci.ic_serializer.Deserialize(msg)
                        #bci.icStreamer.Dump() # Check what's left, see how much I loose            
                        self.bci.icStreamer.Clear()
                except:
                    print self.bci.ic_serializer.Deserialize(data)
                    print '[TOBI-IC] Warning, received data not in iC format!'
                    return

                return self.bci.ic_msg.GetValue(classifier_name,class_name)
                #print prob
            


def main():
    
    app = QtGui.QApplication(sys.argv)
    ex = KeyLogger()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
