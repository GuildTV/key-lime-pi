package common;

import java.security.MessageDigest;

public class Security {

	/**
	 * Generate MD5 hash of a string
	 * 
	 * @param data
	 *            String to make hash of
	 * @return String MD5 hash of data
	 */
	public static String MD5(String data) {
		try {
			// convert data to bytes
			byte[] convertme = data.getBytes();
			// digest message
			MessageDigest md = MessageDigest.getInstance("MD5");
			String str = new String(md.digest(convertme));
			// return it
			return str;
		} catch (Exception nsae) {
			// report failure
			Log.log("Failed MD5 Hash");
			// return empty
			return "";
		}
	}

	/**
	 * Generate MD5 hash of a string, and convert it to hex
	 * 
	 * @param data
	 *            String to make hash of
	 * @return Hex String MD5 hash of data
	 */
	public static String MD5HEX(String data) {
		// get hash
		String raw = MD5(data);
		// convert to hex
		return StringUtil.toHex(raw);
	}

	/**
	 * Generate SHA1 hash of a string, and convert it to hex
	 * 
	 * @param data
	 *            String to make hash of
	 * @return Hex String SHA1 hash of data
	 */
	public static String SHA1HEX(String data) {
		// get hash
		String raw = SHA1(data);
		// convet to hex
		return StringUtil.toHex(raw);
	}

	/**
	 * Generate SHA1 hash of a string
	 * 
	 * @param data
	 *            String to make hash of
	 * @return String SHA1 hash of data
	 */
	public static String SHA1(String data) {
		try {
			// convert data to bytes
			byte[] convertme = data.getBytes();
			// digest message
			MessageDigest md = MessageDigest.getInstance("SHA-1");
			String str = new String(md.digest(convertme));
			// return it
			return str;
		} catch (Exception nsae) {
			// report failure
			Log.log("Failed SHA1 Hash");
			// return empty
			return "";
		}
	}
}
