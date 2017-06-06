#!/usr/bin/python
# -*- coding: utf-8 -*-

import ConfigParser
import sys
from PyQt4 import QtGui, QtCore
import BCI_offline
import time
import pylpttrigger
from websocket import create_connection
import math
import xml.etree.ElementTree as ET

class KeyLogger(QtGui.QWidget):

	global BestProb;
	BestProb = 0;

    
	def __init__(self):
		super(KeyLogger, self).__init__()
		self.initUI()
        
	def initUI(self):      
		
		
		self.connect_to_loop = 1
		mode = sys.argv[1];
		
		if (self.connect_to_loop):
			self.bci = BCI_offline.BciInterface(mode)

		self.ws = create_connection("ws://37.139.15.171:8082")	# Training socket

		if (mode == "1"):
			try:
				while True:
					self.offline_training()
			except KeyboardInterrupt:
				self.closing()
		elif (mode == "2"):
			try:
				while True:
					self.offline_control()
			except KeyboardInterrupt:
				tree = ET.parse("/home/cnbiuser1/Desktop/MentalWorkPrototypes/mi_protocol.xml")
				root = tree.getroot()
				root.find("online/mi/mi_max_prob").text = unicode(BestProb)
				tree.write("/home/cnbiuser1/Desktop/MentalWorkPrototypes/mi_protocol.xml")
				self.closing()
									              																
	def closing(self):
		self.ws.close()
		exit()

	def process_tmp(self, msg):  
		self.ws.send(msg)

	# Handle TOBI iD
	def offline_training(self):
		data = None
		
		# deserialize ID message
		try:
			data = self.bci.iDsock_bus.recv(512)
			self.bci.idStreamer_bus.Append(data)
		except:
			self.nS = False
			self.dec = 0
			pass
		
		if data:
			if self.bci.idStreamer_bus.Has("<tobiid","/>"):
				msg = self.bci.idStreamer_bus.Extract("<tobiid","/>")
				self.bci.id_serializer_bus.Deserialize(msg)
				self.bci.idStreamer_bus.Clear()
				tmpmsg = self.bci.id_msg_bus.GetEvent()
				self.process_tmp(unicode(tmpmsg))
				print tmpmsg

			elif self.bci.idStreamer_bus.Has("<tcstatus","/>"):
				MsgNum = self.bci.idStreamer_bus.Count("<tcstatus")
				for i in range(1,MsgNum-1):
					# Extract most of these messages and trash them		
					msg_useless = self.bci.idStreamer_bus.Extract("<tcstatus","/>")

	def offline_control(self):
		dataC_MI = None
		global BestProb;

		try:
			dataC_MI = self.bci.conn.recv(512)
		except OSError as msg:
			print msg
			pass

		if dataC_MI:
			self.bci.ic_serializer.Deserialize(dataC_MI);
			self.prob = self.bci.ic_msg.GetValue("mi","770")
			self.ws.send(unicode(self.prob))
		if (self.prob > BestProb):
			BestProb = self.prob;
		
def main():
    
    app = QtGui.QApplication(sys.argv)
    ex = KeyLogger()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
