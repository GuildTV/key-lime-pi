package lime;

import JSON.JSONException;
import JSON.JSONStringer;
import gui.control.TitleElement;

/**
 * Create JSON command messages to send to master-pi
 * 
 * @author julus
 * 
 */
public class CommandBuilder {
	// logger to log to
	private static Logger logger;

	/**
	 * Disable instantiation of this class
	 */
	private CommandBuilder() {
	}

	/**
	 * Set the logger to log to
	 * 
	 * @param l
	 *            Log to log to
	 */
	public static void setLog(Logger l) {
		logger = l;
	}

	/**
	 * Write to log
	 * 
	 * @param str
	 *            String to write
	 */
	private static void log(String str) {
		if (logger != null)
			logger.log(str);
	}

	/**
	 * Create command string for preview, and flatten to string
	 * 
	 * @param elm
	 *            TitleElement to be previewed
	 */
	public static String createPreview(TitleElement elm) {
		// create json stringer
		JSONStringer stringer = new JSONStringer();
		try {
			// create preview shell
			stringer.object();
			stringer.key("type").value("previewVideo");
			stringer.key("data");
			// add data
			elm.toJSON(stringer);
			// close up
			stringer.endObject();

			// return it
			return stringer.toString();

		} catch (JSONException e) {
			// log error
			log("Failed to generate preview command");
		}

		return null;
	}

	/**
	 * Create command string for preload, and flatten to string
	 * 
	 * @param elm
	 *            TitleElement to be preloaded
	 */
	public static String createPreload(TitleElement elm) {
		// create json stringer
		JSONStringer stringer = new JSONStringer();
		try {
			// create preload shell
			stringer.object();
			stringer.key("type").value("preloadVideo");
			stringer.key("data");
			// add data
			elm.toJSON(stringer);
			// close up
			stringer.endObject();

			// return it
			return stringer.toString();

		} catch (JSONException e) {
			// log command
			log("Failed to generate preload command");
		}

		return null;
	}

	/**
	 * Create command string for play, and flatten to string
	 * 
	 * @param elm
	 *            TitleElement to be played
	 */
	public static String createPlay(TitleElement elm) {
		// create json stringer
		JSONStringer stringer = new JSONStringer();
		try {
			// create play command
			stringer.object();
			stringer.key("type").value("playVideo");
			stringer.endObject();

			// return it
			return stringer.toString();

		} catch (JSONException e) {
			// log command
			log("Failed to generate play command");
		}

		return null;
	}

}
