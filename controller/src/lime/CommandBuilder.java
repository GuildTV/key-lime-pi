package lime;

import JSON.JSONException;
import JSON.JSONStringer;
import gui.control.TitleElement;

public class CommandBuilder {
	private static Logger logger;

	private CommandBuilder() {
	}

	public static void setLog(Logger l) {
		logger = l;
	}

	private static void log(String str) {
		if (logger != null)
			logger.log(str);
	}

	/**
	 * Create command string for preview, and flatten to string
	 * 
	 * @param elm
	 *            TitleElement to be previews
	 */
	public static String createPreview(TitleElement elm) {
		JSONStringer stringer = new JSONStringer();
		try {
			stringer.object();
			stringer.key("type").value("previewVideo");
			stringer.key("data");
			elm.toJSON(stringer);
			stringer.endObject();

			return stringer.toString();

		} catch (JSONException e) {
			log("Failed to generate preview command");
		}

		return null;
	}
	
	public static String createPreload(TitleElement elm) {
		JSONStringer stringer = new JSONStringer();
		try {
			stringer.object();
			stringer.key("type").value("preloadVideo");
			stringer.key("data");
			elm.toJSON(stringer);
			stringer.endObject();

			return stringer.toString();

		} catch (JSONException e) {
			log("Failed to generate preload command");
		}

		return null;
	}
	
	public static String createPlay(TitleElement elm) {
		JSONStringer stringer = new JSONStringer();
		try {
			stringer.object();
			stringer.key("type").value("playVideo");
			stringer.endObject();

			return stringer.toString();

		} catch (JSONException e) {
			log("Failed to generate play command");
		}

		return null;
	}

}
