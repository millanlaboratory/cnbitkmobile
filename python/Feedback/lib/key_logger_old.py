#!/usr/bin/python
# -*- coding: utf-8 -*-

import ConfigParser
import sys
from PyQt4 import QtGui, QtCore
import BCI
import time
import pylpttrigger
from websocket import create_connection
import math

threshold = None

class KeyLogger(QtGui.QWidget):

    
	def __init__(self):
		super(KeyLogger, self).__init__()
		self.initUI()
        
	def initUI(self):      
		
		
		self.connect_to_loop = 1
		
		if (self.connect_to_loop):
			self.bci = BCI.BciInterface()
		
		# self.screen_width = 500
		# self.screen_height = 500
		
		# self.setGeometry(0,0, self.screen_width, self.screen_height)
		# self.setWindowTitle('KeyLogger')
		# self.setFocusPolicy(QtCore.Qt.WheelFocus)
		# self.setFocus()
		# self.show()

		layout = QtGui.QVBoxLayout()
		self.sl = QtGui.QSlider(QtCore.Qt.Horizontal,self)
		self.sl.setMinimum(60)
		self.sl.setMaximum(100)
		self.sl.setValue(100)
		layout.addWidget(self.sl)
		self.setLayout(layout)
		self.show()

		# QtCore.QCoreApplication.processEvents()
		# QtCore.QCoreApplication.flush()

		self.starting_time = time.time()

		timer = QtCore.QTimer(self)
		timer.timeout.connect(self.update_loop)
		timer.start(15);

		if (sys.argv[2] == '1'):
			self.ws = create_connection("ws://37.139.15.171:8080")	# Controller 
		elif (sys.argv[2] == '2'):
			self.ws = create_connection("ws://37.139.15.171:8081")	# Regulator

		
	def update_loop(self):
		self.handle_tobiid_input()	
		self.repaint()
		
        
	def paintEvent(self, e):
		
		qp = QtGui.QPainter()
		qp.begin(self)
		self.paintInterface(qp)
		qp.end()

        
	def paintInterface(self, qp):	
		self.sl.valueChanged.connect(self.thresholdUpdate)

		# qp.setPen(QtGui.QColor('grey'))
		# qp.setBrush(QtGui.QColor('grey'))
		# qp.drawRect(0, self.screen_height-100, self.screen_width, 100)
		
		# qp.setPen(QtGui.QColor('black'))
		# qp.setBrush(QtGui.QColor('black'))
		# qp.drawRect(self.screen_width/2-45,self.rectPosition(),90,self.rectHeight())
		# qp.setRenderHint(QtGui.QPainter.Antialiasing, True)

		# qp.setPen(QtGui.QPen(QtCore.Qt.darkGreen, 2, QtCore.Qt.DashLine))
		# qp.drawLine(0, 207, self.screen_width, 207)
		# qp.drawText(0, 205, 'MIN')

		# qp.setPen(QtGui.QPen(QtCore.Qt.darkRed, 2, QtCore.Qt.DashLine))
		# qp.drawLine(0, 68, self.screen_width, 68)
		# qp.drawText(0, 65, 'MAX')
		
	def thresholdUpdate(self):
		global threshold
		threshold = float(self.sl.value())/100
									              																
	def closeEvent(self, event):
		print "closing"
		self.ws.close()
		exit()

	def process_tmp(self, msg):  
		self.ws.send(msg)

	def rectPosition(self):
		return -350*self.prob + self.screen_height-100

	def rectHeight(self):
		return 350*self.prob

	# Handle TOBI iD
	def handle_tobiid_input(self):
		#data = None
		
		dataC_MI = None
		dataC_alpha = None
		
		try:
			dataC_MI = self.bci.conn.recv(512)
			dataC_alpha = self.bci.conn_alpha.recv(512)

			#self.bci.icStreamer.Append(dataC_MI)
		except OSError as msg:
			print msg
			pass

		# IC message
		if dataC_MI and dataC_alpha:
			global threshold

			self.bci.ic_serializer.Deserialize(dataC_MI);
			self.prob = self.bci.ic_msg.GetValue("mi","770")
			self.bci.ic_msg.classifiers.Clear()

			self.bci.ic_serializer.Deserialize(dataC_alpha);
			self.alpha = self.bci.ic_msg.GetValue("alpha","1")
			self.bci.ic_msg.classifiers.Clear()

			if sys.argv[1] == '0':
				self.process_tmp(unicode(str(self.prob)+';'+str((threshold))))

				if self.prob >= threshold:
					self.bci.id_msg_bus.SetEvent(781);
					self.bci.id_serializer_bus.SetMessage(self.bci.id_msg_bus)
					idsmsg = self.bci.id_serializer_bus.Serialize()
					self.bci.iDsock_bus.send(idsmsg)
			else:
				self.process_tmp(unicode(self.prob))


		# deserialize ID message

		# try:
		# 	data = self.bci.iDsock_bus.recv(512)
		# 	self.bci.idStreamer_bus.Append(data)
		# 	print data
		# except:
		# 	self.nS = False
		# 	self.dec = 0
		# 	pass
		#
		# if data:
		# 	if self.bci.idStreamer_bus.Has("<tobiid","/>"):
		# 		msg = self.bci.idStreamer_bus.Extract("<tobiid","/>")
		# 		self.bci.id_serializer_bus.Deserialize(msg)
		# 		self.bci.idStreamer_bus.Clear()
		# 		tmpmsg = self.bci.id_msg_bus.GetEvent()
		# 		print tmpmsg
		# 		if tmpmsg == unicode(32769):
		# 			print tmpmsg
		# 			self.process_tmp("10")
		# 		if tmpmsg == unicode(33665):
		# 			print tmpmsg
		# 		 	self.process_tmp("90")

		# 	elif self.bci.idStreamer_bus.Has("<tcstatus","/>"):
		# 		MsgNum = self.bci.idStreamer_bus.Count("<tcstatus")
		# 		for i in range(1,MsgNum-1):
		# 			# Extract most of these messages and trash them		
		# 			msg_useless = self.bci.idStreamer_bus.Extract("<tcstatus","/>")
		
def main():
    
    app = QtGui.QApplication(sys.argv)
    ex = KeyLogger()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
