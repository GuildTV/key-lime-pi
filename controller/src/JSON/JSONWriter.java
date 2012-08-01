package JSON;

import java.io.File;
import java.io.IOException;

import common.*;

/**
 * Write JSONNode to a file or string
 * 
 * @author Julus
 * 
 */
public class JSONWriter {

	/**
	 * Convert JSONNode to a String
	 * 
	 * @param node
	 *            JSONNode tree to convert
	 * @return String of JSON
	 * @throws JSONException
	 */
	public static String JSONtoString(JSONNode node) throws JSONException {
		// parse to string
		String s = JSONNodeParse(node, "", 0, false).trim();
		// trim trailing comma
		s = s.substring(0, s.length() - 1);
		// return string
		return s;
	}

	/**
	 * Write JSONNode to a file
	 * 
	 * @param node
	 *            JSONNode tree to convert
	 * @param filename
	 *            String filename of file to save to
	 * @return Boolean success
	 * @throws JSONException
	 */
	public static boolean JSONtoFile(JSONNode node, String filename) throws JSONException {
		return JSONtoFile(node, new File(filename));
	}

	/**
	 * Write JSONNode to a file
	 * 
	 * @param node
	 *            JSONNode tree to convert
	 * @param file
	 *            File handle of file to save to
	 * @return Boolean success
	 * @throws JSONException
	 */
	public static boolean JSONtoFile(JSONNode node, File file) throws JSONException {
		try {
			// check if file exists
			if (!file.exists())
				// create file
				file.createNewFile();
			// check if it is a file
			if (!file.isFile())
				return false;

			// parse to string
			String s = JSONNodeParse(node, "\t", 0, true).trim();
			// trim trailing comma
			s = s.substring(0, s.length() - 1);

			// write to file
			IOUtil.writeFile(file, s);

			return true;
		} catch (IOException e) {
			// report error
			System.out.println("Failed to create json file");
			return false;
		}
	}

	/**
	 * Convert JSONNode to a string
	 * 
	 * @param node
	 *            JSONNode to convert
	 * @param offsetChar
	 *            Indent character
	 * @param offsetCount
	 *            Indent character count
	 * @param newLine
	 *            Boolean whether to put in newline characters
	 * @return String representing JSONNode and any children
	 * @throws JSONException
	 */
	private static String JSONNodeParse(JSONNode node, String offsetChar, int offsetCount, boolean newLine)
			throws JSONException {
		// if has no children, then is simple
		if (!node.hasChildren()) {
			if (node.getName() == null)
				return "\"" + node.getValue() + "\",";
			else
				return (node.parent.getChildCount() > 1 ? StringUtil.str_pad_l("", offsetChar,
						offsetCount * offsetChar.length()) : "")
						+ "\""
						+ node.getName()
						+ "\":\""
						+ StringUtil.addSlashes(node.getValue())
						+ "\","
						+ (node.parent.getChildCount() > 1 && newLine ? "\n" : "");
		}

		// has children
		String compiled = "";
		// get number of children
		int childCount = node.getChildCount();

		// if(newLine && childCount>1) compiled += "\n";

		// loop through children
		for (int i = 0; i < childCount; i++) {
			if (node.getName() != null && node.getChild(i).getName() != null) {
				String add = JSONNodeParse(node.getChild(i), offsetChar, offsetCount + 1, newLine);
				add = add.substring(0, add.length() - 1);
				if (add.lastIndexOf(",") == add.length() - 1)
					add = add.substring(0, add.length() - 1);
				compiled += "{" + add + "},";
			} else
				compiled += JSONNodeParse(node.getChild(i), offsetChar, offsetCount + 1, newLine);
		}
		compiled = compiled.substring(0, compiled.length() - 1);

		// if has multiple children and has a name, then wrap with []
		if (childCount > 1 && node.getName() != null) {
			if (compiled.lastIndexOf(",") == compiled.length() - 1)
				compiled = compiled.substring(0, compiled.length() - 1);
			compiled = "["
					+ compiled
					+ (compiled.lastIndexOf("}") == compiled.length() - 1 ? (newLine ? "\n" : "")
							+ StringUtil.str_pad_l("", offsetChar, offsetCount * offsetChar.length()) : "") + "]";
		} else if (compiled.indexOf("{") == 0
				&& (compiled.lastIndexOf("}") == compiled.length() - 1 || compiled.lastIndexOf("}") == compiled
						.length() - 2)) {
			compiled = "[" + compiled + "]";
		}

		// if parent has no name and this has no name and last isnt a comma,
		// then give a comma
		if (node.parent != null && node.parent.getName() == null && node.getName() != null
				&& compiled.lastIndexOf(",") != compiled.length() - 1)
			compiled += ",";

		// if has no name, then return only value
		if (node.getName() != null)
			compiled = StringUtil.str_pad_l("", offsetChar, offsetCount * offsetChar.length()) + "\"" + node.getName()
					+ "\":" + compiled + (newLine ? "\n" : "");
		else {
			if (compiled.lastIndexOf(",") == compiled.length() - 1)
				compiled = compiled.substring(0, compiled.length() - 1);
			// has name, check if has children
			if (node.getChildCount() > 1)
				compiled = (newLine ? "\n" : "")
						+ StringUtil.str_pad_l("", offsetChar, (offsetCount + 1) * offsetChar.length()) + "{"
						+ (newLine ? "\n" : "") + compiled + (newLine ? "\n" : "")
						+ StringUtil.str_pad_l("", offsetChar, offsetCount * offsetChar.length()) + "},";
			else if (node.parent != null && node.parent.getChildCount() > 1) {
				compiled = (newLine ? "\n" : "")
						+ StringUtil.str_pad_l("", offsetChar, offsetCount * offsetChar.length()) + "{" + compiled
						+ "},";
			} else {
				compiled = "{" + compiled + "},";
			}
		}

		// return
		return compiled;
	}
}
