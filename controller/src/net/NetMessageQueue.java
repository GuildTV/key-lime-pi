package net;


import java.util.LinkedList;
import java.util.Queue;

/**
 * Manage a queue of NetMessage objects
 * 
 * @author julus
 * 
 */
public class NetMessageQueue {
	//queue lock
	private Object messageLock = new Object();
	
	//message queue
	private Queue<NetMessage> queue = new LinkedList<NetMessage>();

	/**
	 * Push a message onto the queue
	 * @param msg NetMessage to add
	 * @return boolean success
	 */
	public boolean push(NetMessage msg) {
		//fail if message not complete
		if (!msg.isComplete())
			return false;

		//get lock, then add message
		synchronized (messageLock) {
			queue.add(msg);
			//notify any waiting threads
			messageLock.notify();
		}

		return true;
	}

	/**
	 * Get next message in queue
	 * @param wait Wait if there is no message
	 * @return NetMessage from queue
	 */
	public NetMessage pop(boolean wait) {
		//if not wait
		if (!wait) {
			//just return a NetMessage or null
			synchronized (messageLock) {
				return queue.poll();
			}
		}

		synchronized (messageLock) {
			//if there is an item, then return it
			if (queue.size() > 0)
				return queue.poll();

			try {
				//wait to be told there is a message waiting
				messageLock.wait();
			} catch (InterruptedException e) {
			}

			//return next NetMessage or null
			return queue.poll();
		}
	}
}
