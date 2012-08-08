package lime;

/**
 * Interface to handle network status changes
 * 
 * @author julus
 * 
 */
public interface NetHandler {
	/**
	 * Connection closed
	 */
	public void closed();

	/**
	 * Handshake completed
	 */
	public void handshaken();
}
