package JSON;

import java.util.ArrayList;

import common.StringUtil;

/**
 * Class representing a single JSON tag
 * 
 * @author Julus
 * 
 */
public class JSONNode {
	// parse json on demand
	private boolean onDemand;
	// this tag name
	private String tagName = null;
	// have children been generated
	private boolean generatedChildren = false;
	// json string of self
	private String node;
	// this value
	private String value = null;
	// list of children
	private ArrayList<JSONNode> children = new ArrayList<JSONNode>(0);
	// number of child nodes
	private int childCount = 0;
	// parent node
	public JSONNode parent;

	/**
	 * Create a JSONNode from tag name
	 * 
	 * @param parent
	 *            JSONNode parent
	 * @param tagName
	 *            String new tag name
	 */
	public JSONNode(JSONNode parent, String tagName) {
		this.node = null;
		this.onDemand = true;
		this.generatedChildren = true;
		this.tagName = tagName;
		this.parent = parent;
	}

	/**
	 * Create a JSONNode from String of JSON
	 * 
	 * @param parent
	 *            JSONNode parent
	 * @param node
	 *            String JSON
	 * @param onDemand
	 *            Boolean generate on demand
	 * @throws JSONException
	 */
	public JSONNode(JSONNode parent, String node, boolean onDemand) throws JSONException {
		// save ondemand and parent
		this.onDemand = onDemand;
		this.parent = parent;
		// check if it is a json 'entry'
		if (node.substring(0, 1).equals("{")) {
			// node = {"foo":"bar",.....}
			this.node = node;
		} else {
			// node = "foo":"bar" or "foo":{...} or "foo":[{...},{...}]
			int index = node.indexOf(":");

			// check if it has a name and value, or just a value
			if (index == -1) {
				this.node = node;

			} else {
				// split into two pieces
				String piece = node.substring(0, index);
				this.node = node.substring(index + 1);

				// strip tagname of speech marks
				if (piece.charAt(0) == 34)
					piece = piece.substring(1);
				if (piece.charAt(piece.length() - 1) == 34 && piece.charAt(piece.length() - 2) != 92)
					piece = piece.substring(0, piece.length() - 1);
				// save tagname
				this.tagName = (piece.length() > 0 ? piece : null);
			}
		}

		// check if it has any children
		if (this.node.substring(0, 1).equals("{") || this.node.substring(0, 1).equals("[")) {
			// save as yes, but unknown number
			this.childCount = -1;
		}

		// if has no children
		if (this.childCount == 0) {
			// check if starts with speechmark
			if (this.node.charAt(0) == 34)// "
				this.node = this.node.substring(1);
			// check if ends with speechmark
			if (this.node.charAt(this.node.length() - 1) == 34 && this.node.charAt(this.node.length() - 2) != 92)
				this.node = this.node.substring(0, this.node.length() - 1);
			// save value
			this.value = this.node;
			// children have been generated
			this.generatedChildren = true;
			// clear old data
			this.node = null;
			// end
			return;
		}

		// if not ondemand, generate children now
		if (!onDemand)
			this.generateChildren();
	}

	/**
	 * Generate children of this JSONNode
	 * 
	 * @throws JSONException
	 */
	private synchronized void generateChildren() throws JSONException {
		// check if already generated
		if (this.generatedChildren)
			return;
		// check if it has children
		if (this.childCount != 0) {
			// check if is json array
			if (this.node.substring(0, 1).equals("[") || this.node.substring(0, 1).equals("{")) {
				// remove [] and surrounding whitespace
				String data = this.node.trim().substring(1, this.node.length() - 2).trim();
				char[] chars = data.toCharArray();

				// set entry level at 0
				int level = 0;
				// set as not in text
				boolean isText = false;
				// saving start of current piece
				int start = 0;
				// save of pieces
				ArrayList<String> pieces = new ArrayList<String>();

				// loop to find pieces
				for (int i = 0; i < chars.length; i++) {
					// increase/decrease level
					if ((chars[i] == 123 || chars[i] == 91) && !isText)
						level++;// {
					if ((chars[i] == 125 || chars[i] == 93) && !isText)
						level--;// }
					// toggle in text
					if (chars[i] == 34) {// "
						// count slashes
						int slash = 0;
						for (int j = i - 1; j >= 0; j--) {
							if (chars[j] == 92)
								slash++;
							else
								break;
						}
						// toggle istext value
						if (slash % 2 == 0)
							isText = !isText;
					}

					// check for split
					if (level == 0 && chars[i] == 44 && !isText) {
						// split and save
						pieces.add(data.substring(start, i).trim());
						start = i + 1;
					}
				}

				// add last piece
				pieces.add(data.substring(start).trim());
				// update child count
				this.childCount = pieces.size();
				// save pieces as new children
				for (int a = 0; a < pieces.size(); a++) {
					this.children.add(new JSONNode(this, pieces.get(a).trim(), this.onDemand));
				}
				// check if json entry
			} else {
				// unknown type, so say so
				System.out.println("unexpected first char for node");
			}
		}

		// children have been generated
		this.generatedChildren = true;
		// clear data
		this.node = null;
	}

	/**
	 * Get the name of this JSON tag
	 * 
	 * @return String tag name
	 */
	public String getName() {
		return this.tagName;
	}

	/**
	 * Value of this JSON tag
	 * 
	 * @return String value
	 */
	public String getValue() {
		return this.value;
	}

	/**
	 * Check whether this has children
	 * 
	 * @return Boolean whether has any children
	 */
	public boolean hasChildren() {
		return (this.childCount > 0 || this.childCount == -1 ? true : false);
	}

	/**
	 * Get an array of children names
	 * 
	 * @return String list of children tag names
	 */
	public String[] getChildrenNames() {
		// check if has any children
		if (this.childCount == 0)
			return (new String[0]);
		try {
			// check if children are generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report failure
			System.out.println(ex.getMessage());
			return null;
		}
		// start list
		String[] list = new String[children.size()];

		// add children to list
		for (int i = 0; i < children.size(); i++) {
			list[i] = children.get(i).getName();
		}

		// return list
		return list;
	}

	/**
	 * Get the count of children nodes
	 * 
	 * @return Children node counts
	 */
	public int getChildCount() {
		// if zero, report zero
		if (this.childCount == 0)
			return 0;
		try {
			// check if children are generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report failure
			System.out.println(ex.getMessage());
			return 0;
		}
		// return child count
		return this.children.size();
	}

	/**
	 * Get the child by array position
	 * 
	 * @param pos
	 *            Position in array
	 * @return JSONNode child
	 */
	public JSONNode getChild(int pos) {
		// if has no children, return empty
		if (this.childCount == 0)
			return null;
		try {
			// check if children are generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report failure
			System.out.println(ex.getMessage());
			return null;
		}

		// check if position is within valid range
		if (pos < 0 || pos > this.children.size())
			return null;

		// return child
		return this.children.get(pos);
	}

	/**
	 * Get child by tag name
	 * 
	 * @param name
	 *            String tag name of child
	 * @return JSONNode of child
	 */
	public JSONNode getChild(String name) {
		return this.getChild(name, 1);
	}

	/**
	 * Get child by tag name
	 * 
	 * @param name
	 *            String tag name of child
	 * @param occurance
	 *            Occurance of <code>name</code>
	 * @return JSONNode of child
	 */
	public JSONNode getChild(String name, int occurance) {
		// return empty if there are no children
		if (this.childCount == 0)
			return null;

		// check occurance number is valid int
		if (occurance < 1)
			occurance = 1;
		// get a list of names
		String[] names = this.getChildrenNames();

		// define variables
		int pos = -1;
		int occured = 0;

		// loop through children
		for (int i = 0; i < names.length; i++) {
			// check if names match
			if (names[i].equals(name))
				pos = i;
			// check if correct occurance
			if (occured == occurance)
				break;
		}

		// check found child actualy exists
		if (pos < 0 || pos > this.children.size())
			return null;

		// return the child
		return this.children.get(pos);
	}

	/**
	 * Count of children nodes with tag name <code>name</code>
	 * 
	 * @param name
	 *            String tag name to match
	 * @return Count of children nodes
	 */
	public int getChildNameCount(String name) {
		// check if has children
		if (this.childCount == 0)
			return 0;
		// get list of children
		String[] names = this.getChildrenNames();

		// set counter at 0
		int count = 0;

		// loop through children
		for (int i = 0; i < names.length; i++) {
			// check if names match
			if (names[i].equals(name))
				count++;
		}

		// return count
		return count;
	}

	/**
	 * Set name of this
	 * 
	 * @param name
	 *            String new name
	 * @return Boolean success
	 */
	public boolean setName(String name) {
		// check has length
		if (name.length() == 0)
			return false;
		// check is valid alphanumeric or null
		if (!StringUtil.checkAlphaNumeric(name) && !name.equals(null))
			return false;

		// set name
		this.tagName = name;
		return true;
	}

	/**
	 * Set value of this
	 * 
	 * @param value
	 *            Integer new value
	 * @return Boolean success
	 */
	public boolean setValue(int value) {
		return this.setValue(value + "");
	}

	/**
	 * Set value of this
	 * 
	 * @param value
	 *            String new value
	 * @return Boolean success
	 */
	public boolean setValue(String value) {
		// check doesnt have children
		if (this.childCount != 0)
			return false;
		// check if value is null
		if (value == null)
			value = "";
		// save value
		this.value = value;
		return true;
	}

	/**
	 * Create JSONNode child
	 * 
	 * @param name
	 *            Name of new node
	 * @param val
	 *            Value of new node
	 * @return JSONNode new node
	 */
	public synchronized JSONNode createChild(String name, String val) {
		// create child
		JSONNode node = this.createChild(name);
		// set value
		node.setValue(val);
		// return child
		return node;
	}

	/**
	 * Create JSONNode child
	 * 
	 * @return JSONNode new node
	 */
	public synchronized JSONNode createChild() {
		return this.createChild(null);
	}

	/**
	 * Create JSONNode child
	 * 
	 * @param name
	 *            String tag name
	 * @return JSONNode new node
	 */
	public synchronized JSONNode createChild(String name) {
		try {
			// check if children have been generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report error
			System.out.println(ex.getMessage());
			return null;
		}

		// create new child
		JSONNode child = new JSONNode(this, name);
		// save child
		this.children.add(child);
		// increase child count
		this.childCount++;

		// clear this value
		this.value = null;

		// return child
		return child;
	}

	/**
	 * Remove child by tag name
	 * 
	 * @param name
	 *            String tag name
	 * @return Boolean success
	 */
	public boolean removeChild(String name) {
		return this.removeChild(name, 1);
	}

	/**
	 * Remove child by tag name
	 * 
	 * @param name
	 *            String tag name
	 * @param occurance
	 *            Occurance of child with matching tag name
	 * @return Boolean success
	 */
	public synchronized boolean removeChild(String name, int occurance) {
		try {
			// check if children have been generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report error
			System.out.println(ex.getMessage());
			return false;
		}

		// check if has children
		if (this.childCount == 0)
			return false;
		// check if occurance is valid int
		if (occurance < 1)
			occurance = 1;

		// set occured at 0
		int occured = 0;

		// loop through children
		for (int i = 0; i < this.childCount; i++) {
			// check if names match
			if (this.children.get(i).getName().equals(name)) {
				occured++;
				// if correct occurance
				if (occured == occurance) {
					// remove child
					this.children.remove(i);
					break;
				}
			}
		}

		// decrease count
		this.childCount--;
		return true;
	}

	/**
	 * Remove child with array position <code>id</code>
	 * 
	 * @param id
	 *            Position in array
	 * @return Boolean success
	 */
	public synchronized boolean removeChild(int id) {
		try {
			// check children are generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report error
			System.out.println(ex.getMessage());
			return false;
		}

		// check has children
		if (this.childCount == 0)
			return false;

		// check id is in valid range
		if (id > this.children.size() || id < 0)
			return false;

		// decrease child count
		this.childCount--;
		// remove child
		this.children.remove(id);
		return true;
	}

	/**
	 * Remove all children with matching tag name
	 * 
	 * @param name
	 *            String tag name to match
	 * @return Boolean success
	 */
	public synchronized boolean removeChildName(String name) {
		try {
			// check children are generated
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report error
			System.out.println(ex.getMessage());
			return false;
		}

		// check if has children
		if (this.childCount == 0)
			return false;

		// loop through children
		int count = 0;
		for (int i = 0; i < this.childCount; i++) {
			// check if names match
			if (this.children.get(i).getName().equals(name)) {
				// remove child
				this.children.remove(i);
				count++;
			}
		}

		// update child count
		this.childCount = this.childCount - count;
		return true;
	}

	/**
	 * Remove all children
	 * 
	 * @return Boolean success
	 */
	public synchronized boolean removeChildAll() {
		try {
			// check children are loaded
			if (!this.generatedChildren)
				this.generateChildren();
		} catch (JSONException ex) {
			// report error
			System.out.println(ex.getMessage());
			return false;
		}

		// check has children
		if (this.childCount == 0)
			return false;

		// wipe storage
		this.children = new ArrayList<JSONNode>(0);
		// reset children count
		this.childCount = 0;
		return true;
	}

	/**
	 * Get value of child with matching tag name
	 * 
	 * @param name
	 *            String tag name to match
	 * @return String value of child
	 */
	public String getChildValue(String name) {
		// get child
		JSONNode node = this.getChild(name);
		// check if child is null
		if (node == null)
			return null;
		// return value
		return node.getValue();
	}
}
