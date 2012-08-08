package gui.control;

/**
 * Store a data value pair
 * 
 * @author julus
 * 
 */
public class TitleData {

	// name
	private String name = "";
	// value
	private String value = "";

	/**
	 * Create empty data pair
	 */
	public TitleData() {
	}

	/**
	 * Create data pair
	 * 
	 * @param name
	 *            Starting name
	 * @param value
	 *            Starting value
	 */
	public TitleData(String name, String value) {
		this.name = name;
		this.value = value;
	}

	/**
	 * Get the name
	 * 
	 * @return String name
	 */
	public String getName() {
		return name;
	}

	/**
	 * Set the name
	 * 
	 * @param name
	 *            String name
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * Get the value
	 * 
	 * @return String value
	 */
	public String getValue() {
		return value;
	}

	/**
	 * Set the value
	 * 
	 * @param value
	 *            String value
	 */
	public void setValue(String value) {
		this.value = value;
	}

	/**
	 * Convert this to string
	 */
	@Override
	public String toString() {
		return name + ": " + value;
	}
}
