package lime;
import JSON.*;
import net.NetMessage;
import net.NetMessageQueue;

public class MessageHandler implements Runnable {
	LimeController control;
	NetMessageQueue queue;
	boolean running = false;

	public MessageHandler(LimeController control, NetMessageQueue queue) {
		this.control = control;
		this.queue = queue;
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
		//try to parse json
		/*
		JSONNode json = JSONReader.generateFromString(msg.getMessage(), false);
		
		if(json == null){
			//TODO handle bad json
			return;
		}
		*/
		
		
	}
}
