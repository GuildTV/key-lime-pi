package JSON;

/**
 * Exception for JSON thingy :)
 * 
 * @author Julus
 */
public class JSONException extends Exception {
	private static final long serialVersionUID = -1291927002537339284L;

	/**
	 * Create instance of this exception
	 */
	JSONException() {
		super("Unknown JSON Exception!");
	}

	/**
	 * Create isntance of this exception with custom message
	 * 
	 * @param msg
	 *            String message to save
	 */
	JSONException(String msg) {
		super(msg);
	}
}
