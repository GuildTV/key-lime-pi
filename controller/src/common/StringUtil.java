package common;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.util.ArrayList;

/**
 * Some useful generic string utilities
 * 
 * @author Julus
 * 
 */
public class StringUtil {
	// charset converters
	public static Charset charset = Charset.forName("UTF-8");
	public static CharsetEncoder encoder = charset.newEncoder();
	public static CharsetDecoder decoder = charset.newDecoder();

	/**
	 * Convert a String to a ByteBuffer
	 * 
	 * @param msg
	 *            String to be converted
	 * @return ByteBuffer of the String
	 */
	public static ByteBuffer string2bytebuffer(String msg) {
		try {
			return encoder.encode(CharBuffer.wrap(msg));
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * Convert a ByteBuffer to a String
	 * 
	 * @param buffer
	 *            ByteBuffer to be converted
	 * @return String of the ByteBuffer
	 */
	public static String bytebuffer2string(ByteBuffer buffer) {
		String data = "";
		try {
			int old_position = buffer.position();
			data = decoder.decode(buffer).toString();
			// reset buffer's position to its original so it is not altered:
			buffer.position(old_position);
		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
		return data;
	}

	/**
	 * add slashes to a string to make it safe for mysql, or json/xml
	 * 
	 * @param s
	 *            String to add slashes to
	 * @return 'Safe' String
	 */
	public static String addSlashes(String s) {
		if (s == null)
			return "";
		s = s.replaceAll("\\\\", "\\\\\\\\");
		s = s.replaceAll("\\n", "\\\\n");
		s = s.replaceAll("\\r", "\\\\r");
		s = s.replaceAll("\\00", "\\\\0");
		s = s.replaceAll("'", "\\\\'");
		return s;
	}

	/**
	 * trim whitespace off left of string
	 * 
	 * @param s
	 *            String to trim
	 * @return Trimmed string
	 */
	public static String ltrim(String s) {
		int i = 0;
		while (i < s.length() && Character.isWhitespace(s.charAt(i))) {
			i++;
		}
		return s.substring(i);
	}

	/**
	 * trim whitespace off right of string
	 * 
	 * @param s
	 *            String to trim
	 * @return Trimmed string
	 */
	public static String rtrim(String s) {
		int i = s.length() - 1;
		while (i > 0 && Character.isWhitespace(s.charAt(i))) {
			i--;
		}
		return s.substring(0, i + 1);
	}

	/**
	 * convert string to hex
	 * 
	 * @param string
	 *            String to convert
	 * @return Hex form of string
	 */
	public static String toHex(String string) {
		// get bytes
		byte[] data = string.getBytes();
		String hex = "";

		// for each character, convert the number to hex
		for (int i = 0; i < data.length; i++) {
			int val = data[i] & 0x000000FF;
			String ad = Integer.toHexString(val);
			while (ad.length() < 1)
				ad = "0" + ad;
			hex += ad;
		}
		// return completed string
		return hex;
	}

	/**
	 * split string into pieces of defined length
	 * 
	 * @param data
	 *            String to split
	 * @param length
	 *            Length of each piece
	 * @return Array of pieces
	 */
	public static String[] str_split(String data, int length) {
		int pieces = (int) Math.ceil((double) data.length() / (double) length);
		String[] compiled = new String[pieces];
		for (int i = 0; i < pieces; i++) {
			int start = i * length;
			int end = start + length;
			if (end > data.length())
				end = data.length();
			String piece = data.substring(start, end);
			compiled[i] = piece;
		}
		return compiled;
	}

	/**
	 * pad a string to the left with a certain character to specified legnth
	 * 
	 * @param data
	 *            String to pad
	 * @param character
	 *            Character to pad with
	 * @param length
	 *            Desired length
	 * @return String of length length or greater
	 */
	public static String str_pad_l(String data, String character, int length) {
		if (character.length() == 0)
			return data;
		while (data.length() < length) {
			data = character + data;
		}
		return data;
	}
	
	/**
	 * find the position of a string in an array
	 * 
	 * @param str
	 *            String to match
	 * @param array
	 *            Array to search
	 * @return integer position or -1 on failure
	 */
	public static int array_search(String str, String[] array){
		if(array.length == 0)
			return -1;
		
		for (int i = 0; i < array.length; i++) {
			if (array[i] != null && array[i].equals(str))
				return i;
		}
		
		return -1;
	}
	
	/**
	 * find the position of a string in an array
	 * 
	 * @param str
	 *            String to match
	 * @param array
	 *            Array to search
	 * @return integer position or -1 on failure
	 */
	public static int array_search(String str, ArrayList<String> array) {
		if (array.size() == 0)
			return -1;
		for (int i = 0; i < array.size(); i++) {
			if (array.get(i).equals(str))
				return i;
		}
		return -1;
	}
	
	/**
	 * check if string is in an array
	 * 
	 * @param str
	 *            String to match
	 * @param array
	 *            Array to search
	 * @return boolean whether str is in array
	 */
	public static boolean in_array(String str, String[] array) {
		return (array_search(str, array)!=-1);
	}

	/**
	 * check if string is in an array
	 * 
	 * @param str
	 *            String to match
	 * @param array
	 *            Array to search
	 * @return boolean whether str is in array
	 */
	public static boolean in_array(String str, ArrayList<String> array) {
		return (array_search(str, array)!=-1);
	}

	/**
	 * check if string is alphanumeric characters only
	 * 
	 * @param s
	 *            String to check
	 * @return true or false
	 */
	public static boolean checkAlphaNumeric(String s) {
		return checkAlphaNumeric(s, new char[0]);
	}

	/**
	 * check if string is alphanumeric characters only + bonus characters
	 * 
	 * @param s
	 *            String to check
	 * @param bonus
	 *            Array of extra characters
	 * @return true or false
	 */
	public static boolean checkAlphaNumeric(String s, char[] bonus) {
		char[] txt = s.toCharArray();

		main: for (int i = 0; i < txt.length; i++) {
			int c = (int) txt[i];
			if (c >= 48 && c <= 57)
				continue;
			if (c >= 65 && c <= 90)
				continue;
			if (c >= 97 && c <= 122)
				continue;
			for (int o = 0; o < bonus.length; o++) {
				if (c == (int) bonus[o])
					continue main;
			}
			return false;
		}
		return true;
	}

	/**
	 * check if string is alphabet characters only
	 * 
	 * @param s
	 *            String to check
	 * @return true or false
	 */
	public static boolean checkAlpha(String s) {
		return checkAlpha(s, new char[0]);
	}

	/**
	 * check if string is alphabet characters only + bonus characters
	 * 
	 * @param s
	 *            String to check
	 * @param bonus
	 *            Array of extra characters
	 * @return true or false
	 */
	public static boolean checkAlpha(String s, char[] bonus) {
		char[] txt = s.toCharArray();

		main: for (int i = 0; i < txt.length; i++) {
			int c = (int) txt[i];
			if (c >= 65 && c <= 90)
				continue;
			if (c >= 97 && c <= 122)
				continue;
			for (int o = 0; o < bonus.length; o++) {
				if (c == (int) bonus[o])
					continue main;
			}
			return false;
		}
		return true;
	}

	/**
	 * check if string is numeric characters only
	 * 
	 * @param s
	 *            String to check
	 * @return true or false
	 */
	public static boolean checkNumeric(String s) {
		return checkNumeric(s, new char[0]);
	}

	/**
	 * check if string is numeric characters only + bonus characters
	 * 
	 * @param s
	 *            String to check
	 * @param bonus
	 *            Array of extra characters
	 * @return true or false
	 */
	public static boolean checkNumeric(String s, char[] bonus) {
		char[] txt = s.toCharArray();

		main: for (int i = 0; i < txt.length; i++) {
			int c = (int) txt[i];
			if (c >= 48 && c <= 57)
				continue;
			for (int o = 0; o < bonus.length; o++) {
				if (c == (int) bonus[o])
					continue main;
			}
			return false;
		}
		return true;
	}

	/**
	 * check if string looks like an email address
	 * 
	 * @param s
	 *            String to check
	 * @return true or false
	 */
	public static boolean checkEmailAddress(String s) {
		char[] bonus = { 64, 45, 46, 95, 43 };

		if (!checkAlphaNumeric(s, bonus))
			return false;

		if (s.indexOf("@") != s.lastIndexOf("@") || s.indexOf("@") <= 1 || s.lastIndexOf(".") + 2 > s.length())
			return false;

		return true;
	}

	/**
	 * base 64 encode string in a simpler function
	 * 
	 * @param s
	 *            String to encode
	 * @return Base64 encoded form of s
	 */
	public static String base64_encode(String s) {
		byte[] b = s.getBytes();
		return Base64.encodeBytes(b);
	}

	/**
	 * base 64 decode string in a simpler function
	 * 
	 * @param s
	 *            Base64 to decode
	 * @return Base64 decoded form of s
	 */
	public static String base64_decode(String s) {
		try {
			return new String(Base64.decode(s));
		} catch (IOException e) {
			return "";
		}
	}
}
