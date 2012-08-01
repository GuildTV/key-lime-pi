package net;

import java.io.*;
import java.net.*;

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

	private NetMessageQueue messageQueue;

	public NetIO() {
		//create new NetMessageQueue
		messageQueue = new NetMessageQueue();
	}

	public NetMessageQueue getMessageQueue() {
		return messageQueue;
	}

	public boolean connect(String address, int port) {
		if (connected)
			return false;

		try {
			master = new Socket(address, port);
			outStream = master.getOutputStream();
			inStream = master.getInputStream();
		} catch (UnknownHostException e) {
			System.err.println("Don't know about host: " + address + ".");
			return false;
		} catch (IOException e) {
			System.err.println("Couldn't get I/O for the connection to: " + address + ".");
			return false;
		}

		connected = true;

		start();
		
		//initiate handshake
		return sendMessageRAW(SHAKECLIENT);
	}

	private boolean sendMessageRAW(String message) {
		if (!connected)
			return false;

		if (message.length() == 0)
			return false;

		try {
			outStream.write(message.getBytes());
		} catch (IOException e) {
			// TODO
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
			return null;// TODO
		}

		if (read == 0)
			return "";
		if (read == -1) {
			close();
			return null; // TODO
		}

		return new String(data);
	}
	
	private boolean processHandshake(String msg) {
		if(handshaken)
			return true;
		
		if(msg.compareTo(SHAKESERVER) == 0){
			handshaken = true;
			return true;
		}
		
		return false;
	}
	
	private void processMessage(String msg) {
		if(!connected || !handshaken)
			return;
		
		if(msg.length() <= 2){
			close();
			return;
		}
			
		
		
		
		
		
		
		
		
	}

	public void close() {
		if (!connected)
			return;

		running = false;

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
			
			if(!handshaken)
				processHandshake(m);
			else
				processMessage(m);
		}
		close();
	}
}
