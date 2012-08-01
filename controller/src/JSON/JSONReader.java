package JSON;

import java.io.File;

import common.*;

/**
 * Read JSON from a file
 * 
 * @author Julus
 * 
 */
public class JSONReader {
	/**
	 * Generate JSONNode from a string
	 * 
	 * @param json
	 *            String of JSON to parse
	 * @param onDemand
	 *            Boolean representing whether to generate data on demand
	 * @return JSONNode for the JSON string
	 */
	public static JSONNode generateFromString(String json, boolean onDemand) {
		try {
			// return created child
			return new JSONNode(null, json, onDemand);

		} catch (JSONException e) {
			// report error
			System.out.println("JSONException whilst reading json: " + e.getMessage());
		}
		return null;
	}

	/**
	 * Generate JSONNode from a file
	 * 
	 * @param file
	 *            String pathname of file to read
	 * @param onDemand
	 *            Boolean representing whether to generate data on demand
	 * @return JSONNode for the json string
	 */
	public static JSONNode generateFromFile(String file, boolean onDemand) {
		return generateFromFile(new File(file), onDemand);
	}

	/**
	 * Generate JSONNode from a file
	 * 
	 * @param file
	 *            File handle of file to read
	 * @param onDemand
	 *            Boolean representing whether to generate data on demand
	 * @return JSONNode for the json string
	 */
	public static JSONNode generateFromFile(File file, boolean onDemand) {
		try {
			// generate doc
			String json = IOUtil.readFile(file);

			// return created child
			return new JSONNode(null, json, onDemand);
		} catch (JSONException e) {
			// report error
			System.out.println("JSONException whilst reading json: " + e.getMessage());
		}
		return null;
	}
}
