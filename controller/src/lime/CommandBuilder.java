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
	 * Create command string for play, and flatten to string
	 * 
	 * @param elm
	 *            TitleElement to be played
	 */
	public static String createSelect(TitleElement elm) {
		// create json stringer
		JSONStringer stringer = new JSONStringer();
		try {
			// create play command
			stringer.object();
			stringer.key("type").value("selectVideo");
			stringer.key("script").value(elm.getNameValue());
			stringer.endObject();

			// return it
			return stringer.toString();

		} catch (JSONException e) {
			// log command
			log("Failed to generate play command");
		}

		return null;
	}
	
	public static String createPlay() {
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
	
	public static String refreshBurns() {
		// create json stringer
		JSONStringer stringer = new JSONStringer();
		try {
			// create play command
			stringer.object();
			stringer.key("type").value("dataList");
			stringer.endObject();

			// return it
			return stringer.toString();

		} catch (JSONException e) {
			// log command
			log("Failed to generate refresh command");
		}

		return null;
	}

}
