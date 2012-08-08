package net;
/*
 *      Copyright (C) 2012 GuildTV
 *      http://www.guildtv.co.uk
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

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
