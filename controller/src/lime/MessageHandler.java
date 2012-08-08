package lime;

import JSON.JSONException;
import JSON.JSONObject;

import net.NetMessage;
import net.NetMessageQueue;

public class MessageHandler implements Runnable {
	private LimeController control;
	private NetMessageQueue queue;
	private boolean running = false;
	private Thread t;

	public MessageHandler(LimeController control, NetMessageQueue queue) {
		this.control = control;
		this.queue = queue;

		t = new Thread(this, "Message Handler");
		t.start();
	}

	/**
	 * Stop loop running
	 */
	public void stop() {
		running = false;
	}

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
			JSONObject json = new JSONObject(msg.getMessage());
			String type = json.getString("type");
			if (type.equals("preloadVideo")) {
				String status = json.getString("status");
				if (status.equals("video loaded")) {
					control.getFrame().getControlPanel().getTitlePanel().setLoadedByName(json.getString("name"));
					control.getFrame().getControlPanel().getConnectionPanel().setAction("video ready");
				} else
					control.log("Failed to load video with error '" + json.getString("status") + "'");
				control.getFrame().getControlPanel().getConnectionPanel().setAction("waiting");
				return;
			} else if (type.equals("playVideo")) {
				control.log("Failed to play video with error '" + json.getString("status") + "'");
				control.getFrame().getControlPanel().getConnectionPanel().setAction("waiting");
				return;
			} else if (type.equals("slaveConnected")){
				control.getFrame().getControlPanel().getConnectionPanel().pollSlaveResponse(json.getString("status"));
				return;
			}

		} catch (JSONException e) {
			control.log("Failed to parse message");
		}
	}
}
