package net;


/**
 * Simple class to hold a message recieved from the network
 * 
 * @author julus
 * 
 */
public class NetMessage {
	//length of message
	private int messageLength;
	//message body
	private String message;
	//whether the message is complete
	private boolean complete;

	NetMessage(int length, String msg){
		messageLength = length;
		message = msg;
		complete = false;
	}
		
	/**
	 * Get the length of the message
	 * @return length
	 */
	public int getMessageLength() {
		return messageLength;
	}

	/**
	 * Get the message
	 * @return message
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * Append to the end of the message, if not complete
	 * @param msg Content to add
	 */
	public void appendMessage(String msg) {
		if (!complete)
			message += msg;
	}

	/**
	 * Check if this message is complete
	 * @return complete
	 */
	public boolean isComplete() {
		return complete;
	}

	/**
	 * Set this message as completed
	 */
	public void setComplete() {
		complete = true;
	}

	@Override
	public String toString() {
		return "NetMessage [message=" + message + ", length=" + messageLength + ", complete=" + complete + "]";
	}
}
