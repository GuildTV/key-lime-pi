package lime;
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

import JSON.JSONException;
import JSON.JSONObject;

import net.NetMessage;
import net.NetMessageQueue;

/**
 * Handle and message that were received
 * 
 * @author julus
 * 
 */
public class MessageHandler implements Runnable {
	// controller
	private LimeController control;
	// message queue
	private NetMessageQueue queue;
	// is thread running
	private boolean running = false;
	// thread to handle messages
	private Thread t;

	/**
	 * Create message handler
	 * 
	 * @param control
	 *            Controller to fire events to
	 * @param queue
	 *            Queue of messages
	 */
	public MessageHandler(LimeController control, NetMessageQueue queue) {
		this.control = control;
		this.queue = queue;

		// run thread
		t = new Thread(this, "Message Handler");
		t.start();
	}

	/**
	 * Stop loop running
	 */
	public void stop() {
		running = false;
	}

	/**
	 * Run loop
	 */
	@Override
	public void run() {
		running = true;
		while (running) {
			NetMessage msg = queue.pop(true);
			// check got a message
			if (msg == null)
				continue;

			// discard incomplete message
			if (!msg.isComplete())
				continue;

			// handle message
			handle(msg);
		}

		running = false;
	}

	/**
	 * Handle message
	 * 
	 * @param msg
	 *            Message to handle
	 */
	private void handle(NetMessage msg) {
		System.out.println(msg.getMessage());
		// try to parse json
		try {
			// parse json message
			JSONObject json = new JSONObject(msg.getMessage());
			String type = json.getString("type");

			// check if response about playing
			if (type.equals("playVideo")) {
				String status = json.getString("status");
				// check status response
				if (status.equals("starting playback")) {
					// set text in action window
					control.getFrame().getControlPanel().getConnectionPanel().setAction("playing video");
					return;
				} else if (status.equals("playback finished")) {
					// set text in action window
					control.getFrame().getControlPanel().getConnectionPanel().setAction("waiting");
					return;
				} else if (status.equals("waiting for playback")) {
					// set text in action window
					control.getFrame().getControlPanel().getConnectionPanel().setAction("starting playback");
					return;
				}
				
				// log error
				control.log("Failed to play video with error '" + status + "'");
				// set text in action window
				control.getFrame().getControlPanel().getConnectionPanel().setAction("waiting");
				return;

				// check if response about slave connection status
			}

		} catch (JSONException e) {
			// log error
			control.log("Failed to parse message");
		}
	}
}
