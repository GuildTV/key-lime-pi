package net;

import java.io.*;
import java.net.*;

import common.StringUtil;

import lime.LimeController;
import lime.Logger;
import lime.NetHandler;

public class NetIO implements Runnable {

	private static final String SHAKESERVER = "I am ALPHA-PI!! :)";
	private static final String SHAKECLIENT = "I am RGB-PI!! :)";

	private Socket master = null;
	private OutputStream outStream = null;
	private InputStream inStream = null;
	private boolean connected = false;
	private boolean running = false;
	private boolean handshaken = false;
	private Thread thread;
	private Logger logger;

	private NetMessageQueue messageQueue;
	private NetHandler handle;

	public NetIO(NetHandler handle) {
		// create new NetMessageQueue
		messageQueue = new NetMessageQueue();
		this.handle = handle;
	}

	public NetMessageQueue getMessageQueue() {
		return messageQueue;
	}

	public boolean isConnected() {
		return connected;
	}

	public boolean connect(String address, int port) {
		if (connected)
			return false;

		try {
			master = new Socket(address, port);
			outStream = master.getOutputStream();
			inStream = master.getInputStream();
		} catch (UnknownHostException e) {
			log("Don't know about host: " + address + ".");
			return false;
		} catch (IOException e) {
			log("Couldn't get I/O for the connection to: " + address + ".");
			return false;
		}

		connected = true;

		start();

		// initiate handshake
		return sendMessageRAW(SHAKECLIENT);
	}

	private boolean sendMessageRAW(String message) {
		if (!connected)
			return false;

		if (message.length() == 0)
			return true;

		try {
			outStream.write(message.getBytes());
		} catch (IOException e) {
			log("Failed to send RAW message");
			return false;
		}

		return true;
	}

	public boolean sendMessage(String message) {
		if (!connected)
			return false;

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

		message = b2 + message;
		message = b1 + message;

		try {
			outStream.write(message.getBytes());
		} catch (IOException e) {
			log("Failed to send message");
			return false;
		}

		return true;
	}

	private String readMessage() {
		if (!connected)
			return null;

		int read = -2;
		byte data[] = new byte[500];
		try {
			read = inStream.read(data);
		} catch (IOException e) {
			log("Failed to read message from server");
			return null;
		}

		if (read == 0)
			return "";
		if (read == -1) {
			close();
			return null;
		}

		return new String(data);
	}

	private boolean processHandshake(String msg) {
		if (handshaken)
			return true;

		if (msg.compareTo(SHAKESERVER) == 0) {
			handshaken = true;
			log("Successfully handshaken");
			return true;
		}

		return false;
	}

	private void processMessage(String msg) {
		if (!connected || !handshaken)
			return;

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

		msg = msg.substring(2);
		String trail = null;
		
		if(msg.length() > len){
			//split and seperate
			trail = msg.substring(len);
			msg = msg.substring(0, len);
		}
		
		//TODO handle by opcode
		
		//store in queue
		NetMessage message = new NetMessage(len, msg);
		if(len == msg.length())
			message.setComplete();
		
		if(message.isComplete())
			messageQueue.push(message);
		else {
			//TODO
		}
		
		//process trailing piece, if exists
		if(trail != null)
			processMessage(trail);
	}

	public void close() {
		if (!connected)
			return;

		running = false;
		
		handle.closed();

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

	public void stop() {
		running = false;
	}

	private void start() {
		if (running)
			return;
		thread = new Thread(this, "Net Listener");
		thread.start();
	}

	@Override
	public void run() {
		running = true;

		String m;
		while (running) {
			m = readMessage();
			if (m == null) {
				running = false;
				close();
				return;
			}

			// trim trailing whitespace
			m = StringUtil.rtrim(m);

			if (!handshaken)
				processHandshake(m);
			else
				processMessage(m);
		}
		close();
	}

	public void setLogger(Logger l) {
		logger = l;
	}

	public void remLogger() {
		logger = null;
	}

	public void log(String str) {
		if (logger != null)
			logger.log(str);
	}
}