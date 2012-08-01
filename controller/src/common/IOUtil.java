package common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Scanner;

/**
 * Some useful file read/write functions
 * 
 * @author Julus
 */
public class IOUtil {
	/**
	 * Read contents of file to String
	 * 
	 * @param path
	 *            Pathname of file to read
	 * @return String contents of file
	 */
	public static String readFile(String path) {
		return readFile(new File(path));
	}

	/**
	 * Read contents of file to String
	 * 
	 * @param file
	 *            File handle of file to read
	 * @return String contents of file
	 */
	public static String readFile(File file) {
		try {
			// load file
			StringBuilder text = new StringBuilder();
			String NL = System.getProperty("line.separator");
			Scanner scanner = new Scanner(new FileInputStream(file));

			try {
				// read lines to string
				while (scanner.hasNextLine()) {
					text.append(scanner.nextLine() + NL);
				}
				// return it
				return text.toString();
			} finally {
				// close file
				scanner.close();
			}
		} catch (Exception e) {
		}
		// return empty
		return "";
	}

	/**
	 * Write string to file
	 * 
	 * @param path
	 *            Pathname of file to write to
	 * @param s
	 *            String to write
	 * @return Boolean success
	 */
	public static boolean writeFile(String path, String s) {
		return writeFile(new File(path), s);
	}

	/**
	 * Write string to file
	 * 
	 * @param file
	 *            File handle of file to write to
	 * @param s
	 *            String to write
	 * @return Boolean success
	 */
	public static boolean writeFile(File file, String s) {
		try {
			// start stream
			FileOutputStream out = new FileOutputStream(file);
			// write it
			out.write(s.getBytes());
			// clear it
			out.flush();
			// close it
			out.close();
			// report it
			return true;
		} catch (Exception e) {
			// report error
			return false;
		}
	}
}
