import pylibtobiic, pylibtobiid, pylibtobicore, pytpstreamer
import atexit
from socket import *

class BciInterface:
    def __init__(self, mode):

		if (mode == "1"):	# For offline mode. no control
			# set up ID objects
			self.id_msg_bus = pylibtobiid.IDMessage()
			self.id_msg_bus.SetBlockIdx(100)
			self.id_msg_bus.SetDescription("KeyLogger")
			self.id_msg_bus.SetFamilyType(0)
			self.id_serializer_bus = pylibtobiid.IDSerializer(self.id_msg_bus, True)

			# Bind sockets for iD
			self.iDIP_bus = '127.0.0.1'
			self.iDport_bus = 8126
			self.idStreamer_bus = pytpstreamer.TPStreamer()
			self.iDsock_bus = socket(AF_INET, SOCK_STREAM)
			self.iDsock_bus.connect((self.iDIP_bus, self.iDport_bus))

			print 'Protocol is listening for iD event data on ip %s, port %d' % (self.iDIP_bus, self.iDport_bus)
			self.iDsock_bus.setblocking(0)
			atexit.register(self.iDsock_bus.close) # close socket on program termination, no matter what!
		
		elif (mode == "2"):	# For controlling the training machine
			# set up IC objects
			self.ic_msg = pylibtobiic.ICMessage()
			self.ic_serializer = pylibtobiic.ICSerializer(self.ic_msg, True)
			self.icStreamer = pytpstreamer.TPStreamer()
			
			# Bind sockets for iC
			self.iCIP = '127.0.0.1'
			self.iCport = 9501
			self.iCsock = socket(AF_INET, SOCK_STREAM)
			self.iCsock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
			self.iCsock.bind((self.iCIP, self.iCport))
			self.iCsock.listen(1)
			self.conn, self.address = self.iCsock.accept()

			print 'Protocol is listening for iC data on ip %s, port %d' % (self.iCIP, self.iCport)
			self.iCsock.setblocking(0)
			atexit.register(self.iCsock.close) # close socket on program termination, no matter what!

		

