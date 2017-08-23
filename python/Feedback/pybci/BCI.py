import pylibtobiic, pylibtobiid, pylibtobicore, pytpstreamer
import atexit
from socket import *

class BciInterface:
    def __init__(self, IPLOOP):
		# Setup TOBI interfaces iC and iD

		#------------------------------------------------------------------------------------
		# set up ID objects
		#------------------------------------------------------------------------------------
		
		# Bus Stream
		self.id_msg_bus = pylibtobiid.IDMessage()
		self.id_msg_bus.SetBlockIdx(100)
		self.id_msg_bus.SetDescription("KeyLogger")
		self.id_msg_bus.SetFamilyType(0)

		self.id_serializer_bus = pylibtobiid.IDSerializer(self.id_msg_bus, True)
		
		# Bind sockets for iD
		self.iDIP_bus = IPLOOP
		self.iDport_bus = 8126

		self.idStreamer_bus = pytpstreamer.TPStreamer()
		
		self.iDsock_bus = socket(AF_INET, SOCK_STREAM)
		self.iDsock_bus.connect((self.iDIP_bus, self.iDport_bus))

		print 'Protocol is listening for iD event data on ip %s, port %d' % (self.iDIP_bus, self.iDport_bus)
		self.iDsock_bus.setblocking(0)
		atexit.register(self.iDsock_bus.close) # close socket on program termination, no matter what!

		# Dev Stream (not so used, more for developpers)
		# self.id_msg_dev = pylibtobiid.IDMessage()
		# self.id_msg_dev.SetBlockIdx(100)
		# self.id_msg_dev.SetDescription("KeyLogger")
		# self.id_msg_dev.SetFamilyType(0)
		#self.id_serializer_dev = pylibtobiid.IDSerializer(self.id_msg_dev, True)
		#self.iDIP_dev = '127.0.0.1'
		#self.iDport_dev = 8127
		#self.idStreamer_dev = pytpstreamer.TPStreamer()
		#self.iDsock_dev = socket(AF_INET, SOCK_STREAM)
		#self.iDsock_dev.connect((self.iDIP_dev, self.iDport_dev))
		#print 'Protocol is listening for iD command data on ip %s, port %d' % (self.iDIP_dev, self.iDport_dev)
		#self.iDsock_dev.setblocking(0)
		#atexit.register(self.iDsock_dev.close) # close socket on program termination, no matter what!

		#------------------------------------------------------------------------------------
		# set up IC objects
		#------------------------------------------------------------------------------------
		self.ic_msg = pylibtobiic.ICMessage()
		self.ic_serializer = pylibtobiic.ICSerializer(self.ic_msg, True)
		self.icStreamer = pytpstreamer.TPStreamer()
		
		# Bind sockets for iC
		self.iCIP = IPLOOP

		# IC stream ctrl1
		self.iCport = 9501
		self.iCsock = socket(AF_INET, SOCK_STREAM)
		self.iCsock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
		self.iCsock.bind((self.iCIP, self.iCport))
		self.iCsock.listen(1)
		self.conn, self.address = self.iCsock.accept()
		print 'Protocol is listening for iC data on ip %s, port %d' % (self.iCIP, self.iCport)
		self.iCsock.setblocking(0)
		atexit.register(self.iCsock.close) # close socket on program termination, no matter what!
