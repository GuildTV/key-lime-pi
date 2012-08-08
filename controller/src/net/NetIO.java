package net;

import java.io.*;
import java.net.*;

import common.StringUtil;

import lime.Logger;
import lime.NetHandler;

public class NetIO implements Runnable {

	// handshake text
	private static final String SHAKESERVER = "I am ALPHA-PI!! :)";
	private static final String SHAKECLIENT = "I am RGB-PI!! :)";

	// socket to master
	private Socket master = null;
	// output stream
	private OutputStream outStream = null;
	// input stream
	private InputStream inStream = null;

	// connection status
	private boolean connected = false;
	// thread running status
	private boolean running = false;
	// handshake status
	private boolean handshaken = false;
	// listening thread
	private Thread thread;
	// logger
	private Logger logger;

	// message queue
	private NetMessageQueue messageQueue;
	// network handler
	private NetHandler handle;

	/**
	 * Create network thingy
	 * 
	 * @param handle
	 *            Network status handler
	 */
	public NetIO(NetHandler handle) {
		// create new NetMessageQueue
		messageQueue = new NetMessageQueue();
		this.handle = handle;
	}

	/**
	 * Get the message queue
	 * 
	 * @return queue of messages
	 */
	public NetMessageQueue getMessageQueue() {
		return messageQueue;
	}

	/**
	 * Check if is connected to master-pi
	 * 
	 * @return Connection status
	 */
	public boolean isConnected() {
		return connected;
	}

	/**
	 * Connect to server
	 * 
	 * @param address
	 *            Address
	 * @param port
	 *            Port
	 * @return Boolean success
	 */
	public boolean connect(String address, int port) {
		// fail if already connected
		if (connected)
			return false;

		try {
			// create socket and in/out streams
			master = new Socket(address, port);
			outStream = master.getOutputStream();
			inStream = master.getInputStream();
		} catch (UnknownHostException e) {
			// log failure
			log("Don't know about host: " + address + ".");
			return false;
		} catch (IOException e) {
			// log failure
			log("Couldn't get I/O for the connection to: " + address + ".");
			return false;
		}

		// set as connected
		connected = true;

		// start listening for messages
		start();

		// initiate handshake
		return sendMessageRAW(SHAKECLIENT);
	}

	/**
	 * Send message without header
	 * 
	 * @param message
	 *            Message to send
	 * @return boolean success
	 */
	private boolean sendMessageRAW(String message) {
		// fail if not connected
		if (!connected)
			return false;

		// end if no message to send
		if (message.length() == 0)
			return true;

		try {
			// try sending message
			outStream.write(message.getBytes());
		} catch (IOException e) {
			// log failure
			log("Failed to send RAW message");
			return false;
		}

		// return success
		return true;
	}

	/**
	 * Send a message with headers
	 * 
	 * @param message
	 *            Message to send
	 * @return boolean success
	 */
	public boolean sendMessage(String message) {
		// fail if not connected
		if (!connected)
			return false;

		// end if no message to send
		if (message.length() == 0)
			return true;

		// header up message
		int len = message.length();
		int opcode = 1;
		int len1 = len / 256;
		len1 *= 8;
		int len2 = len - len1 * 32;
		int bit1 = len1 + opcode;

		char b1 = (char) bit1;
		char b2 = (char) len2;

		// append headers to message
		message = b2 + message;
		message = b1 + message;

		try {
			// try sending message
			outStream.write(message.getBytes());
		} catch (IOException e) {
			// log failure
			log("Failed to send message");
			return false;
		}

		// return success
		return true;
	}

	/**
	 * Read a message from socket
	 * 
	 * @return String message
	 */
	private String readMessage() {
		// fail if not connected
		if (!connected)
			return null;

		// data stores
		int read = -2;
		byte data[] = new byte[500];
		try {
			// try and read message
			read = inStream.read(data);
		} catch (IOException e) {
			// close socket if failed
			close();
			return null;
		}

		// if read nothing, return empty
		if (read == 0)
			return "";
		// if failed, close socket
		if (read == -1) {
			close();
			return null;
		}

		// return string
		return new String(data);
	}

	/**
	 * Process handshake with server
	 * 
	 * @param msg
	 *            Handshake message recieved
	 * @return boolean success
	 */
	private boolean processHandshake(String msg) {
		// success if already handshaken
		if (handshaken)
			return true;

		// check if handshake matches
		if (msg.compareTo(SHAKESERVER) == 0) {
			// set success
			handshaken = true;
			handle.handshaken();
			return true;
		}

		// return failure
		return false;
	}

	/**
	 * Process message
	 * 
	 * @param msg
	 *            Message to process
	 */
	private void processMessage(String msg) {
		// fail if not connected or not handshaken
		if (!connected || !handshaken)
			return;

		// close socket if message was too short
		if (msg.length() <= 2) {
			close();
			return;
		}

		// de-header message
		int b1 = msg.charAt(0);
		int b2 = msg.charAt(1);
		int opcode = b1 & 7;
		int len1 = b1 & 248;
		int len = 32 * len1 + b2;

		// trim header
		msg = msg.substring(2);
		String trail = null;

		// check if message lengths match
		if (msg.length() > len) {
			// split and seperate
			trail = msg.substring(len);
			msg = msg.substring(0, len);
		}

		// TODO handle by opcode

		// store in queue
		NetMessage message = new NetMessage(len, msg);

		// check message lengths match
		if (len == msg.length())
			// mark as complete
			message.setComplete();

		// if message is complete, push to queue
		if (message.isComplete())
			messageQueue.push(message);
		else {
			// TODO
		}

		// process trailing piece, if exists
		if (trail != null)
			processMessage(trail);
	}

	/**
	 * Close connection to server
	 */
	public void close() {
		// fail if not connected
		if (!connected)
			return;

		// tidy up on close
		running = false;
		handshaken = false;
		connected = false;

		// log and push close elsewhere
		log("Disconnected from master-pi");
		handle.closed();

		// try and close streams and socket. ignore failures
		try {
			outStream.close();
		} catch (IOException e1) {
		}
		try {
			inStream.close();
		} catch (IOException e) {
		}
		try {
			master.close();
		} catch (IOException e) {
		}
	}

	/**
	 * Stop running listening thread
	 */
	public void stop() {
		running = false;
		// TODO - notify and pop waiting?
	}

	/**
	 * Start listening thread
	 */
	private void start() {
		// fail if already running
		if (running)
			return;

		// start thread
		thread = new Thread(this, "Net Listener");
		thread.start();
	}

	/**
	 * Run thread body
	 */
	@Override
	public void run() {
		running = true;

		String m;
		while (running) {
			// read message
			m = readMessage();
			// if null, close socket
			if (m == null) {
				running = false;
				close();
				return;
			}

			// trim trailing whitespace
			m = StringUtil.rtrim(m);

			// pass message to appropriate processer
			if (!handshaken)
				processHandshake(m);
			else
				processMessage(m);
		}
		close();
	}

	/**
	 * Set the logger
	 * 
	 * @param l
	 *            Logger to write to
	 */
	public void setLogger(Logger l) {
		logger = l;
	}

	/**
	 * Remove logger
	 */
	public void remLogger() {
		logger = null;
	}

	/**
	 * Write to log
	 * 
	 * @param str
	 *            Message to write
	 */
	public void log(String str) {
		if (logger != null)
			logger.log(str);
	}
}