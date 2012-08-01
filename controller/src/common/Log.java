package common;

/**
 * Log message to console
 * 
 * @author Julus
 * 
 */
public class Log {
	/**
	 * Log message
	 * 
	 * @param s
	 *            String message
	 */
	public static void log(String s) {
		System.out.println(s);
	}

	// store debug state
	public static boolean debug = false;

	/**
	 * Log message, if allowed
	 * 
	 * @param s
	 *            String message
	 */
	public static void debug(String s) {
		if (debug)
			System.out.println(s);
	}
}
