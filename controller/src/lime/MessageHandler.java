package lime;

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

			// check if response about preloading
			if (type.equals("preloadVideo")) {
				String status = json.getString("status");
				// check status response
				if (status.equals("video loaded")) {
					// change load/play button
					control.getFrame().getControlPanel().getTitlePanel().setLoadedByName(json.getString("name"));
					// set text in action window
					control.getFrame().getControlPanel().getConnectionPanel().setAction("video ready");
					return;
				}

				// log error
				control.log("Failed to load video with error '" + json.getString("status") + "'");
				// set text in action window
				control.getFrame().getControlPanel().getConnectionPanel().setAction("waiting");
				return;

				// check if response about playing video
			} else if (type.equals("playVideo")) {
				// log error
				control.log("Failed to play video with error '" + json.getString("status") + "'");
				// set text in action window
				control.getFrame().getControlPanel().getConnectionPanel().setAction("waiting");
				return;

				// check if response about slave connection status
			} else if (type.equals("slaveConnected")) {
				// act on response from slave poll
				control.getFrame().getControlPanel().getConnectionPanel().pollSlaveResponse(json.getString("status"));
				return;
			}

		} catch (JSONException e) {
			// log error
			control.log("Failed to parse message");
		}
	}
}
